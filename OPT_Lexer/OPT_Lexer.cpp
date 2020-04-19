#include <iostream>
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include "Translator.h"
#include <vector>
#include <string>

std::string getOutputPath(std::string& inputPath);

int main() {
    std::string analyzingFile = "tests//test09//input.sig";
    std::string outputFile = getOutputPath(analyzingFile);
    Lexer lexer;
    Lexer::AnalyzeResult lexerAnalyzerResult = lexer.scanFile(analyzingFile);
    std::cout << std::endl << std::endl << std::endl << "\t\t\t\t" << "RESULT" << std::endl;
    // lexer.printScanResult();
    std::vector<LexerResult> results = lexer.getResults();
    SyntaxAnalyzer syntaxAnalizer(results);
    syntaxAnalizer.analyze();
    syntaxAnalizer.dumpTreeIntoFile(outputFile, lexerAnalyzerResult.getErrorMassage());
    std::cout << std::endl << std::endl;
    Translator translator = Translator(syntaxAnalizer.getResultTree());
    translator.analyze(outputFile);
}

std::string getOutputPath(std::string& inputPath) {
    std::string stringInputPath = inputPath;

    int index = stringInputPath.find_last_of('/', stringInputPath.size());
    if (index < 0 || index >= stringInputPath.size()) return stringInputPath.append(".txt");
    return stringInputPath.substr(0, index + 1).append("output.txt");
}