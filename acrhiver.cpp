#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "Archivator.h"
#include "Writer.h"

void PrintHelp() {
    std::cout << "This is a simple archive program created by @vaaven" << std::endl;
    std::cout << "Type 'archiver -c filename file1 [file2 ...]' if you want to compress your files(file1, ...) into "
                 "filename archive"
              << std::endl;
    std::cout << "Type 'archiver -d filename' if you want to decompress your files(file1, ...) from "
                 "filename archive"
              << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Please add more arguments (for help write 'archive -h')" << std::endl;
    } else if (std::string(argv[1]) == "-h") {
        PrintHelp();
    } else if (std::string(argv[1]) == "-c") {
        std::ofstream stream(argv[2]);
        if (!stream.is_open()) {
            std::cout << "Can't open file " << argv[1] << " for writing" << std::endl;
            return 1;
        }
        std::cout << "Start to archive" << std::endl;
        Writer archive_writer(stream);
        for (size_t file = 3; file < argc; ++file) {
            bool is_last_file = file + 1 == argc;
            std::cout << "Start to compress file #" << file - 3 << std::endl;
            Compress(argv[file], archive_writer, is_last_file);
            std::cout << "Finish to compress file #" << file - 3 << std::endl;
        }
        archive_writer.Close();
        std::cout << "Archive created successfully!" << std::endl;
    } else if (std::string(argv[1]) == "-d") {
        std::cout << "Start to decompress" << std::endl;
        Decompress(argv[2]);
        std::cout << "Decompressed successfully!" << std::endl;
    } else {
        std::cout << "Invalid usage fo program please write 'archive -h' for help note" << std::endl;
    }
    return 0;
}
