#include <iostream>
#include "Lexer.h"

int main() {
    Lexer lexer;
    lexer.scanFile("program.yandroidUA");

    std::cout << std::endl << std::endl << std::endl << "\t\t\t\t" << "RESULT" << std::endl;
    lexer.printScanResult();
}