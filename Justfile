test: build
    ctest -C Debug --test-dir build --output-on-failure

build:
    cmake -B build
    cmake --build build -j8
