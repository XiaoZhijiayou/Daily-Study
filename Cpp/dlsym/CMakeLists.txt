cmake_minimum_required(VERSION 3.12)
project(dlsym_strlen CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

add_executable(dlsym_strlen dlsym.cpp)

# 启用 GNU 扩展（等价于你代码里的 __USE_GNU，但推荐只保留这一处）
target_compile_definitions(dlsym_strlen PRIVATE _GNU_SOURCE)

# 防止编译器把 strlen 当内建优化掉
target_compile_options(dlsym_strlen PRIVATE -fno-builtin -fno-builtin-strlen)

# 让可执行文件符号可被 dlsym(RTLD_DEFAULT, ...) 解析到
target_link_options(dlsym_strlen PRIVATE -rdynamic)

# 用到 dlsym/dlerror，需链接 libdl
target_link_libraries(dlsym_strlen PRIVATE dl)
