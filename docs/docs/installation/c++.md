
# C++

Same as in C version library can be installed system wide or included directly in a cmake project.

## Header only mode

As the library is just a single header file it can be included directly in a project

```bash
wget https://raw.githubusercontent.com/Imn0/timber-cpp/refs/heads/master/include/tmb/tmb.hpp
```

However for it to work the C version has to be in a include path, also  ⚠️ c++ wrapper requires full version of the C library ⚠️.

## System install

1. Clone the repository

    ```bash
    git clone --recurse-submodules https://github.com/Imn0/timber-cpp.git
    cd timber-cpp
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
git submodule add https://github.com/Imn0/timber-cpp.git libs/timber
```

Example `CMakeLists.txt` to link with a static version

```cmake
target_link_libraries(${PROJECT_NAME} timber-cpp::static)
```
