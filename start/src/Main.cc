#include <iostream>
#include <Compile.h>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: avl source.file output.file" << std::endl;
        return 0;
    }

    avl::compile(argv[1], argv[2]);
    return 0;
}

