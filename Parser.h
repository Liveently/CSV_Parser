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
    int linesTotal{};

    void setLinesNumber(int n) {
        linesTotal = 0;
        std::string tmp;
        while (getline(csvFile, tmp)) {
            linesTotal += 1;
        }
        tmp.clear();
        csvFile.clear();
        csvFile.seekg(0, std::ios::beg);


        for (int i=0 ; i < n; ++i) {
            std::getline(csvFile, tmp);
        }

        linesTotal -= n;

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
        int endIdx;
    public:
        ParserIterator(
                int idx,
                int endIdx,
                Parser<Args...>* parser) :
                idx(idx), endIdx(endIdx), parser(parser) {}

        ParserIterator& operator++ () {
            if (idx < endIdx) {
                idx++;
            }
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

        setLinesNumber(_numOfSkippedLines);

    }

    ~Parser() {
        csvFile.close();
    }

    ParserIterator begin() {
        return ParserIterator(0, linesTotal, this);
    }

    ParserIterator end() {
        return ParserIterator(linesTotal, linesTotal, this);
    }

};
