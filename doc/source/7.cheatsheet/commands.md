# 命令速查

## 编译器命令

### GCC/G++

```bash
# 基本编译
g++ -o program source.cpp

# 常用选项
-std=c++20       # C++ 标准版本
-O0/-O1/-O2/-O3  # 优化级别
-g               # 调试信息
-Wall            # 所有警告
-Wextra          # 额外警告
-Werror          # 警告视为错误
-pedantic        # 严格标准
-I/path          # 包含目录
-L/path          # 库目录
-lname           # 链接库
-DNAME=value     # 定义宏
-fPIC            # 位置无关代码
-shared          # 生成共享库
-static          # 静态链接

# 推荐开发选项
g++ -std=c++20 -Wall -Wextra -Wpedantic -g source.cpp

# 推荐发布选项
g++ -std=c++20 -O2 -DNDEBUG source.cpp
```

### Clang

```bash
# 基本使用与 GCC 相同
clang++ -std=c++20 -o program source.cpp

# Clang 特有选项
-stdlib=libc++       # 使用 libc++
-fcolor-diagnostics  # 彩色输出
```

## CMake

```bash
# 基本构建
mkdir build && cd build
cmake ..
cmake --build .

# 指定生成器
cmake -G "Ninja" ..
cmake -G "Unix Makefiles" ..

# 指定构建类型
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 指定编译器
cmake -DCMAKE_CXX_COMPILER=clang++ ..

# 安装
cmake --install . --prefix /usr/local
```

## Make

```bash
make                # 默认目标
make -j8            # 8 个并行任务
make clean          # 清理
make install        # 安装
make VERBOSE=1      # 显示命令
make -n             # 干运行（不执行）
```

## 调试

### GDB

```bash
gdb ./program       # 启动 GDB
gdb -tui ./program  # TUI 模式

# 常用命令
run                 # 运行
break main          # 断点
break file.cpp:10   # 文件行断点
next / n            # 下一行
step / s            # 进入函数
continue / c        # 继续运行
print var           # 打印变量
backtrace / bt      # 调用栈
quit / q            # 退出
```

### LLDB

```bash
lldb ./program

# 常用命令
run
breakpoint set -n main
breakpoint set -f file.cpp -l 10
next / n
step / s
continue / c
frame variable
thread backtrace
quit
```

## 性能分析

```bash
# perf
perf record ./program
perf report
perf stat ./program

# Valgrind
valgrind ./program
valgrind --leak-check=full ./program
valgrind --tool=callgrind ./program
valgrind --tool=massif ./program

# gprof
g++ -pg source.cpp
./a.out
gprof a.out gmon.out > analysis.txt
```

## Sanitizers

```bash
# AddressSanitizer
g++ -fsanitize=address -g source.cpp

# ThreadSanitizer
g++ -fsanitize=thread -g source.cpp

# UndefinedBehaviorSanitizer
g++ -fsanitize=undefined -g source.cpp

# MemorySanitizer (Clang only)
clang++ -fsanitize=memory -g source.cpp
```

## 包管理器

### Conan

```bash
# 安装
pip install conan

# 创建配置
conan profile detect

# 安装依赖
conan install . --build=missing

# 搜索包
conan search boost -r conancenter
```

### vcpkg

```bash
# 安装包
vcpkg install boost:x64-linux
vcpkg install jsoncpp:x64-linux

# 集成到 CMake
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

## Git 常用命令

```bash
git clone url
git status
git add .
git commit -m "message"
git push
git pull

git branch feature
git checkout feature
git merge feature

git log --oneline
git diff
git stash
git stash pop
```

## Docker

```bash
# 构建镜像
docker build -t myapp .

# 运行容器
docker run -it --rm myapp

# 挂载卷
docker run -v $(pwd):/app myapp

# 查看
docker ps
docker images
docker logs container_id
```
