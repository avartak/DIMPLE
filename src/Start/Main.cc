#include <iostream>
#include <Start/Compile.h>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: dmp source.file output.file" << std::endl;
        return 0;
    }

    dmp::compile(argv[1], argv[2]);
    return 0;
}

