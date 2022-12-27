#include <iostream>

#include "Parser.h"


int main() {

    std::ifstream csvFile(R"(C:\Users\Luna\Desktop\CSV_Parser\test.csv)");

    try {
        Parser<int, std::string> parser(csvFile, 2);

        for (const auto& row : parser) {
            std::cout << row << std::endl;
        }
    }
    catch (simple_exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}