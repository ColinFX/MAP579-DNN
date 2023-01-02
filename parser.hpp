#ifndef PROJECTNEURALNETWORKS_PARSER_HPP
#define PROJECTNEURALNETWORKS_PARSER_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "matrix.hpp"
#include "csv-parser/include/csv.hpp"


int arff2csv(std::string file_name)
{
    std::ifstream fin(file_name + ".arff");
    std::ofstream fout(file_name + ".csv");
    std::string line;
    std::vector<std::string> cols;
    size_t i = 0;
    while (getline(fin, line))
    {
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        if (line.find("@data") != std::string::npos)
        {
            i++;
            break;
        }
        else
        {
            i++;
            if (line.find("@attribute") != std::string::npos)
            {
                if (line.find('{') != std::string::npos)
                {
                    cols.push_back(line.substr(11, line.find('{') - 12));
                }
                else
                {
                    cols.push_back(line.substr(11, line.find(' ', 11) - 11));
                }
            }
        }
    }
    std::string headers;
    for (size_t j = 0; j < cols.size(); j++)
    {
        headers += cols[j];
        if (j != cols.size() - 1)
        {
            headers += ",";
        }
    }
    fout << headers << std::endl;
    while (getline(fin, line))
    {
        fout << line << std::endl;
    }
    fin.close();
    fout.close();
    return 0;
}



matrix& parser(std::string file_name)
{
    csv::CSVReader reader(file_name + ".csv");

    std::vector<float> vec = std::vector<float>();
    size_t m = 0;
    size_t n = 0;

    for (csv::CSVRow& row: reader) {
        m += 1;
        for (csv::CSVField& field: row) {
            if (m==1) n += 1;
            vec.push_back(field.get<float>());
        }
    }
    return matrix(vec, m, n);
}


#endif //PROJECTNEURALNETWORKS_PARSER_HPP


