#include <pthread.h>
#include <stdio.h>

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
long long count = 0;

void* increment_count(void* arg) {
    pthread_mutex_lock(&count_mutex);
    count = count + 1;
    pthread_mutex_unlock(&count_mutex);
    return NULL;
}

int main() {
    pthread_t threads[10];

    // 创建10个线程
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, increment_count, NULL);
    }

    // 等待所有线程完成
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("最终计数值: %lld\n", count);
    return 0;
}