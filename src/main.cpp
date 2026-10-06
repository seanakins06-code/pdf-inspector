#include <iostream>
#include "Formatter.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: pdf-inspector <file.pdf>\n";
        return 1;
    }
    try {
        PdfInfo info = PdfDocument::fromFile(argv[1]).inspect();
        std::cout << makeFormatter()->format(info);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}