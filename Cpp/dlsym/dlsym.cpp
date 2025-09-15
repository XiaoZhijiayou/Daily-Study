#include <iostream>
#define _GNU_SOURCE 1
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <dlfcn.h>

using strlen_t = size_t (*)(const char*);

static strlen_t strlen_f  = nullptr; // 指向本程序里的 strlen（RTLD_DEFAULT）
static strlen_t strlen_f1 = nullptr; // 指向系统库里的 strlen（RTLD_NEXT）

// 用 C 链接名导出，保证符号名就是 "strlen"
extern "C" size_t strlen(const char* str) {
    std::printf("%s strlen\n", __FILE__);
    std::printf("hello dlsym");
    return strlen_f1 ? strlen_f1(str) : 0;
}

int main(int argc, char** argv) {
    dlerror(); // 清空可能的旧错误
    strlen_f = reinterpret_cast<strlen_t>(dlsym(RTLD_DEFAULT, "strlen"));
    if (!strlen_f) {
        std::printf("default load error %s\n", dlerror());
        return 1;
    }

    dlerror();
    strlen_f1 = reinterpret_cast<strlen_t>(dlsym(RTLD_NEXT, "strlen"));
    if (!strlen_f1) {
        std::printf("next load error %s\n", dlerror());
        return 1;
    }

    // 打印函数指针地址（通过 uintptr_t 转成可打印的整型再转指针）
    std::printf("strlen_f  = %p\n", (void*)(uintptr_t)strlen_f);
    std::printf("strlen_f1 = %p\n", (void*)(uintptr_t)strlen_f1);

    std::printf("strlen_f(\"xuedaon\")  = %zu\n", strlen_f("xuedaon"));   // 命中你自己的 strlen
    std::printf("=>>>>>>>>>> <<<<<<<<<<<=\n");
    std::printf("strlen_f1(\"xuedaon\") = %zu\n", strlen_f1("xuedaon"));  // 命中系统库 strlen
    return 0;
}
