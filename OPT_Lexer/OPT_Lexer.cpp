#include <iostream>
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include "Translator.h"
#include <vector>
#include <string>

std::string getOutputPath(std::string& inputPath, std::string);

int main() {
    std::string analyzingFile = "tests//test01/input.sig";
    std::string syntaxOutputFile = getOutputPath(analyzingFile, "output.txt");
    std::string translatorOutputFile = getOutputPath(analyzingFile, "output.asm");
    Lexer lexer;
    Lexer::AnalyzeResult lexerAnalyzerResult = lexer.scanFile(analyzingFile);
    std::cout << std::endl << std::endl << std::endl << "\t\t\t\t" << "RESULT" << std::endl;
    // lexer.printScanResult();
    std::vector<LexerResult> results = lexer.getResults();
    SyntaxAnalyzer syntaxAnalizer(results);
    syntaxAnalizer.analyze();
    syntaxAnalizer.dumpTreeIntoFile(syntaxOutputFile, lexerAnalyzerResult.getErrorMassage());
    std::cout << std::endl << std::endl;
    Translator translator = Translator(syntaxAnalizer.getResultTree());
    translator.analyze();
    translator.dumpIntoFile(translatorOutputFile);
}

std::string getOutputPath(std::string& inputPath, std::string output) {
    std::string stringInputPath = inputPath;

    int index = stringInputPath.find_last_of('/', stringInputPath.size());
    if (index < 0 || index >= stringInputPath.size()) return stringInputPath.append(output);
    return stringInputPath.substr(0, index + 1).append(output);
}