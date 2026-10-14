#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <utility>

class DataBlocking{

    private:

    std::string m_namefile;
    std::ofstream m_fileout;

    int m_L;
    int m_current_step;
    int m_block_count;
    double m_sum_block;
    double m_sum_A;
    double m_sum_A2;

    public:

    DataBlocking();
    DataBlocking(const std::string& filename); //Data Blocking from a vector of data
    DataBlocking(const std::string& filename, int L); //Data blocking from a stream of data

    ~DataBlocking();

    //methods:
    void AnalizeData (const std::vector<double>& data, int L);
    void AnalizeBlocksAverages (const std::vector<double>& blocks_averages);
    void AddMeasure (double x);
    std::pair<double, double> GetFinalStats(const std::vector<double>& blocks_averages);
    void Close();

};