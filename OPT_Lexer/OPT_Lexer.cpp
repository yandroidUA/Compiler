// OPT_Lexer.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Lexer.h"

int main() {
    std::cout << "Hello World!\n";
    Lexer lexer;
    lexer.scanFile("program.yandroidUA");
}