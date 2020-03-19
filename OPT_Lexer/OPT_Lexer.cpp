#include <iostream>
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include <vector>

int main() {
    const char* analyzingFile = "tests//test01//input.sig";
    const char* outputFile = "output//output.txt";
    Lexer lexer;
    Lexer::AnalyzeResult lexerAnalyzerResult = lexer.scanFile(analyzingFile);

    std::cout << std::endl << std::endl << std::endl << "\t\t\t\t" << "RESULT" << std::endl;
    // lexer.printScanResult();
    std::vector<LexerResult> results = lexer.getResults();
    SyntaxAnalyzer syntaxAnalizer(results);
    syntaxAnalizer.analyze(outputFile);
    syntaxAnalizer.dumpTreeIntoFile(outputFile, lexerAnalyzerResult.getErrorMassage());
}