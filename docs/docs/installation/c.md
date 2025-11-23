---
sidebar_position: 2
---

# C

Library can be either be used as a [STB↗](https://github.com/nothings/stb) style single header library[^1] or as a linked library.
[^1]: When used in single header mode it has a limited feature set.

Tested compilers:

- gcc >= 13.3.0
- clang >= 18.1.3
- MSVC >= 19.44.35219.0

## Single header

1. download `tmb.h` file and place it in your source tree.

    ```bash
    wget https://raw.githubusercontent.com/Imn0/timber/refs/heads/master/include/tmb/tmb.h
    ```

2. then use it as follows

    ```c
    #define TMB_LOGGING_IMPLEMENTATION
    #include "tmb.h"

    int main(void) {
        LOG_ERROR("Cannot build river");
    }
    ```

`#define TMB_LOGGING_IMPLEMENTATION` is needed in only one .c file, generally your main.  
In every other file just include the `tmb.h`

## System install

1. Clone the repository

    ```bash
    git clone https://github.com/Imn0/timber.git
    cd timber
    ```

2. Setup cmake

    ```bash
    mkdir build
    cd build
    cmake ..
    ```

    to change the install prefix

    ```bash
    cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/home/$USER/opt/
    ```

3. Compile and install

    ```bash
    cmake --build . --target install
    ```

    then link as normal

## Include in a project

Library can be also directly included as in cmake a project.  
In an existing project either clone it

```bash
git clone https://github.com/Imn0/timber.git libs/timber
```

or add as a submodule

```bash
git submodule add https://github.com/Imn0/timber.git libs/timber
```

Example `CMakeLists.txt` to link with a static version

```cmake
add_subdirectory(libs/timber)
target_link_libraries(${PROJECT_NAME} timber::static)
```
