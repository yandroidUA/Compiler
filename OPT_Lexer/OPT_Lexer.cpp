#include <iostream>
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include <vector>

int main() {
    Lexer lexer;
    lexer.scanFile("program.yandroidUA");

    std::cout << std::endl << std::endl << std::endl << "\t\t\t\t" << "RESULT" << std::endl;
    // lexer.printScanResult();
    std::vector<LexerResult> results = lexer.getResults();
    SyntaxAnalyzer syntaxAnalizer(results);
    syntaxAnalizer.analyze();
}