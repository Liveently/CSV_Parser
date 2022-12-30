#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>


#include "print_t.h"


template<typename... Args>
class Parser {
private:
    std::ifstream& csvFile;
    char colSep; // (,)
    char rowSep;  // (\n)


    void skip_lines(int n) {
        std::string tmp;

        for (int i=0 ; i < n; ++i) {
            std::getline(csvFile, tmp);
        }

    }

    std::string getRow(int lineIdx) {
        std::string rowStr;
        char ch;

        while (csvFile.get(ch) && ch != rowSep ) {
            rowStr += ch;
        }

        return rowStr;
    }


    std::vector<std::string> splitRow(const std::string& line) {
        std::vector<std::string> fields;
        std::string fieldStr;

        for (auto& ch : line) {
            if (ch == colSep) {
                fields.push_back(fieldStr);
                fieldStr.clear();
            }
            else {
                fieldStr += ch;
            }
        }

        if (!fieldStr.empty()) {
            fields.push_back(fieldStr);
        }

        return fields;
    }

    std::tuple<Args...> getTuple(std::string line, int lineIdx) {

        if (line.empty()) {
            throw simple_exception(
                    "Line " + std::to_string(lineIdx+1) + ": "
                                                          "Empty line."
            );
        }

        auto fields = splitRow(line);
        auto fieldsN = sizeof...(Args);

        if (fields.size() != fieldsN) {
            throw simple_exception(
                    "Line " + std::to_string(lineIdx+1) + ": "
                                                          "Wrong fields number."
            );
        }

        std::tuple<Args...> parsedFields;
        try {
            parsedFields = toTuple<Args...>(fields);
        }
        catch (simple_exception& e) {
            throw simple_exception(
                    "Line " + std::to_string(lineIdx+1) + ": " +
                    e.what()
            );
        }

        return parsedFields;
    }

    class ParserIterator {
    private:
        Parser* parser;
        int idx;

    public:

        ParserIterator(
                int idx,
                Parser<Args...>* parser) :
                idx(idx), parser(parser) {}


        ParserIterator& operator++ () {
                idx++;

            if (parser->csvFile.eof()) idx=-1;

            return *this;

        }

        bool operator== (const ParserIterator& other) {
            return this->parser == other.parser && this->idx == other.idx;
        }

        bool operator!= (const ParserIterator& other) {
            return !operator==(other);
        }
        std::tuple<Args...> operator* () {
            return parser->getTuple(parser->getRow(idx), idx);
        }

    };

public:
    explicit Parser(
            std::ifstream& csvFile,
            int _numOfSkippedLines,
            char colSep=',',
            char rowSep='\n') :
            csvFile(csvFile), colSep(colSep), rowSep(rowSep)
    {
        if (!csvFile) {
            throw simple_exception("Can't find specified file.");
        }

        skip_lines(_numOfSkippedLines);

    }

    ~Parser() {
        csvFile.close();
    }

    ParserIterator begin() {
        return ParserIterator(0, this);
    }

    ParserIterator end() {
        return ParserIterator(-1, this);
    }

};