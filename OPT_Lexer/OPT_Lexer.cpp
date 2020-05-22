#include <iostream>
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include "Translator.h"
#include <vector>
#include <string>

std::string getOutputPath(std::string& inputPath, std::string);

void start(std::string& path);

void generateTestFiles(int count);

std::string getNumberOfTest(int number);

int main(int argc, char** argv) {
    if (argc < 2 || argv[1] == nullptr) {
        std::cout << "Wrong path!" << std::endl;
        exit(0);
    }

    std::cout << "PATH: " << argv[1] << std::endl;
    std::string path = argv[1];
    //start(path);
    generateTestFiles(9);
}

void generateTestFiles(int count) {
    std::string root_path = "E:\\GitHub\\Lexer_OPT\\OPT_Lexer\\tests";

    for (int i = 1; i <= count; i++) {
        std::string m_path = root_path + "\\test" + getNumberOfTest(i) + "\\input.sig";
        start(m_path);
    }

}

std::string getNumberOfTest(int number) {
    if (number < 10) {
        return "0" + std::to_string(number);
    }
    return std::to_string(number);
}

void start(std::string& path) {
    std::string analyzingFile = path;
    std::string syntaxOutputFile = getOutputPath(analyzingFile, "expected.txt");
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

    int index = stringInputPath.find_last_of('\\', stringInputPath.size());
    if (index < 0 || index >= stringInputPath.size()) return stringInputPath.append(output);
    return stringInputPath.substr(0, index + 1).append(output);
}