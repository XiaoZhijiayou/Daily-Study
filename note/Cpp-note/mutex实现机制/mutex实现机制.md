## 参考资料

- [mutex实现](https://chengxumiaodaren.com/docs/operate-system/mutex-impl/)

## 需要理解的概念

### 临界区
- [多线程（11）临界区](https://juejin.cn/post/7350261439913181236)


#### 1. 临界区的定义

- 临界区，又称为临界段，是指在并发编程环境下，能够访问共享资源的那部分代码。这些共享资源可能包括共享内存、文件、数据库记录等。临界区的存在是并发程序设计中需要重点关注的问题之一，因为不当的访问控制可能会引起数据不一致、竞态条件等一系列问题。

##### 1.2 为何会有临界区

- `在并发环境下，多个线程或进程可能需要读写同一片内存区域或文件。如果这些操作没有被适当地同步，那么数据的完整性和一致性就无法保证`。例如，如果两个线程同时对同一个变量进行写操作，最终该变量的值取决于哪个线程最后写入，这就是典型的竞态条件。因此，需要有一种机制来保证在任何时刻，只有一个执行线程能够进入临界区进行操作，以此来保护共享资源。


### 具体细节部分

#### TAS (Test-And-Set) 锁

- 1. 为何TAS可以保证公平调度？

```cpp
// 定义锁结构
typedef struct __lock_t {
    int ticket;  // 排队号：下一个到来的线程会拿到的号码
    int turn;    // 当前轮到的号码
} lock_t;

// 初始化
void lock_init(lock_t *lock) {
    lock->ticket = 0;
    lock->turn   = 0;
}

// 加锁
void lock(lock_t *lock) {
    int myturn = FetchAndAdd(&lock->ticket);  // 原子操作，获得唯一票号
    while (lock->turn != myturn)              // 等待轮到自己
        ; // 自旋
}

// 解锁
void unlock(lock_t *lock) {
    lock->turn = lock->turn + 1;  // 轮到下一个号码
}
```

- 2. 如何基于上面的实现，实现cpu不会出现死循环？ 或者说不会浪费时间自旋？ 
    - **避免使用自旋锁的一个重要原因是性能：正如正文中所描述的，如果一个线程在持有锁时被中断，其他使用自旋锁的线程就会浪费大量 CPU 时间，仅仅是等待锁的释放。**
    - 使用队列:让线程休眠而不是自旋锁

- 3. 如何解决优先级反转的问题？ 还有优先级翻转--主要是针对于线程的优先级来做区别操作！！！

    - 在自旋锁导致问题的场景，可以通过 避免使用自旋锁 来解决（后文会进一步介绍）。
    - 更普遍的方法是：当高优先级线程等待低优先级线程时，可以临时提升低优先级线程的优先级，让它先运行并尽快释放锁。这种技术称为 优先级继承（priority inheritance）。

###### 针对于2 3 的问题， 采用队列 + park/unpark机制,让等待线程不用浪费CPU，并且避免饥饿
    - 队列的作用：当一个线程抢不到锁时，不是傻傻地自旋，而是把自己加入等待队列，然后睡眠。
    -  唤醒机制：释放锁的线程会从队列中挑一个等待者，并直接把锁“传递”给它，而不是先把锁设为可用（flag=0），再让大家去抢。这样避免了“插队”和资源浪费。
```cpp
// 锁的数据结构
typedef struct __lock_t {
    int flag;          // 标志位：是否被持有（1=持有，0=未持有）
    queue_t waiters;   // 等待线程队列
    int guard;         // 小型自旋锁，保护 flag 和队列操作
} lock_t;

// 初始化
void lock_init(lock_t *lock) {
    lock->flag = 0;
    queue_init(&lock->waiters);
    lock->guard = 0;
}

// 获取 guard（简单自旋锁）
void acquire_guard(int *guard) {
    while (TestAndSet(guard, 1) == 1) {
        ; // 自旋
    }
}

void release_guard(int *guard) {
    *guard = 0;
}

// 加锁
void lock(lock_t *lock) {
    acquire_guard(&lock->guard);

    if (lock->flag == 0) {
        // 锁空闲，直接占有
        lock->flag = 1;
        release_guard(&lock->guard);
    } else {
        // 锁被占用，把自己加入队列
        int tid = gettid();               // 获取线程 ID
        enqueue(&lock->waiters, tid);

        release_guard(&lock->guard);

        // 阻塞自己，等待被 unpark 唤醒
        park();
        // 被唤醒时，相当于直接接手锁
    }
}

// 解锁
void unlock(lock_t *lock) {
    acquire_guard(&lock->guard);

    if (queue_empty(&lock->waiters)) {
        // 没有等待者，锁可以完全释放
        lock->flag = 0;
    } else {
        // 有等待者，把锁直接交给队列中的下一个
        int next = dequeue(&lock->waiters);
        // 注意：这里不设 flag = 0！
        // 因为锁被直接传给 next，而不是空闲
        unpark(next); // 唤醒下一个线程
    }

    release_guard(&lock->guard);
}
```
    - 这个里面在unlock的时候是不需要设置flag为0的，因为锁被直接传给队列中的下一个线程，而不是空闲。
    - 其中通过guard来保护flag和队列的操作，避免竞态条件。


-------------------------
###### `linux下的futex互斥锁实现的思想`

```c
void mutex_lock(int *mutex) {
    int v;

    // Bit 31 was clear, we got the mutex (fastpath)
    if (atomic_bit_test_set(mutex, 31) == 0)
        return;

    atomic_increment(mutex);

    while (1) {
        if (atomic_bit_test_set(mutex, 31) == 0) {
            atomic_decrement(mutex);
            return;
        }

        // Have to wait. First make sure futex value
        // we are monitoring is negative (locked).
        v = *mutex;
        if (v >= 0)
            continue;

        futex_wait(mutex, v);
    }
}

void mutex_unlock(int *mutex) {
    // Adding 0x80000000 to counter results in 0
    // if and only if there are not other interested threads
    if (atomic_add_zero(mutex, 0x80000000))
        return;

    // There are other threads waiting for this mutex,
    // wake one of them up.
    futex_wake(mutex);
}
```

## 工作机制解释

### 1. 数据结构与设计思路

* `mutex` 是一个整数（int）。它用 **最高位 bit (bit 31)** 来表示锁是否被持有，其余位表示 **等待线程数**。

  * **负数**：锁被持有（高位 1）。
  * **非负数**：锁空闲。
* 使用 **futex\_wait** 和 **futex\_wake** 来实现阻塞与唤醒。

---

### 2. `mutex_lock()`

* **快速路径（fastpath）**：

  ```c
  if (atomic_bit_test_set(mutex, 31) == 0)
      return;
  ```

  * 尝试直接把 bit 31 从 0 置为 1（锁空闲 → 被当前线程获取）。
  * 如果成功（返回 0），直接获取锁，函数返回。
  * 这是最快的情况，没有竞争。

* **慢路径（竞争时）**：

  ```c
  atomic_increment(mutex);
  ```

  * 增加等待计数，表示自己也在争用这个锁。

* **循环尝试**：

  ```c
  if (atomic_bit_test_set(mutex, 31) == 0) {
      atomic_decrement(mutex);
      return;
  }
  ```

  * 再次尝试获取锁，如果成功，就减少等待计数并返回。

* **睡眠等待**：

  ```c
  v = *mutex;
  if (v >= 0)   // 确保锁处于被占用状态
      continue;
  futex_wait(mutex, v);
  ```

  * 如果锁仍被占用（负数），调用 `futex_wait()` 进入睡眠，等待别人唤醒。
  * `futex_wait(mutex, v)` 会检查 `*mutex` 是否仍等于 v，如果不等，立即返回（避免丢失唤醒）。


### 3. `mutex_unlock()`

* **释放锁**：

  ```c
  if (atomic_add_zero(mutex, 0x80000000))
      return;
  ```

  * 给 `mutex` 加上 `0x80000000`（最高位 bit 清零，恢复非负数）。
  * 如果结果是 0，表示没有等待线程，直接返回。

* **唤醒等待线程**：

  ```c
  futex_wake(mutex);
  ```

  * 如果还有等待线程，则唤醒一个线程，让它来竞争锁。


## 整体流程

1. **锁空闲时**：线程通过 fastpath 一步拿到锁（高效）。
2. **锁被占用时**：线程会累加等待数，然后睡眠，避免自旋浪费 CPU。
3. **解锁时**：

   * 如果没人等待，直接清零退出。
   * 如果有人等待，则唤醒一个等待线程。


✅ **总结**：
这段代码展示了 Linux **futex 互斥锁的实现思想**：

* **快速路径**：低竞争下极快。
* **慢路径**：高竞争时避免忙等，用 futex 阻塞/唤醒机制节省 CPU。
* 用一个整数同时存储 **锁状态 + 等待者数量**，设计非常巧妙。
---

###### `双阶段锁`

双阶段锁分为 两个阶段：

    第一阶段（自旋阶段）

        线程在一小段时间里不断尝试获取锁。

        如果锁在这段时间内被释放，线程立即获取成功，避免了上下文切换的开销。

    第二阶段（阻塞阶段）

        如果在自旋阶段没能拿到锁，线程就进入睡眠（调用 futex 等待）。

        当锁被释放时，线程会被唤醒。