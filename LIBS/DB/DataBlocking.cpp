#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <utility>

#include "DataBlocking.h"

using namespace std;

DataBlocking :: DataBlocking() : 
        m_namefile(""), 
        m_L(0), 
        m_current_step(0), 
        m_block_count(0), 
        m_sum_block(0.0), 
        m_sum_A(0.0), 
        m_sum_A2(0.0) 
{}

DataBlocking :: DataBlocking(const string& filename) : 
        m_namefile(filename), 
        m_L(0), 
        m_current_step(0), 
        m_block_count(0), 
        m_sum_block(0.0), 
        m_sum_A(0.0), 
        m_sum_A2(0.0) 
{}

DataBlocking :: DataBlocking(const string& filename, int L) : 
        m_namefile(filename), 
        m_L(L), 
        m_current_step(0), 
        m_block_count(0), 
        m_sum_block(0.0), 
        m_sum_A(0.0), 
        m_sum_A2(0.0) 
    {
        m_fileout.open(m_namefile);
        if (m_fileout.is_open()) {
            m_fileout << "# n.block   mean for n.block   error for n.block" << endl;
            m_fileout << fixed << setprecision(6);
        } else {
            cerr << "Error: unable to open output file " << m_namefile << endl;
        }
}

DataBlocking::~DataBlocking() {
    if (m_fileout.is_open()) {
        m_fileout.close();
    }
}

void DataBlocking :: AnalizeData(const vector<double>& data, int L){

    int M = data.size();
    if (L <= 0 || M < L) {
        cerr << "Error: Invalid block size L = " << L << " for data size M = " << M << endl;
        return;
    }

    ofstream fout(m_namefile);
    if(!fout.is_open()){
        cerr << "Error: unable to open output file " << m_namefile << endl;
        return;
    }

    fout << "# n.block   mean for n.block   error for n.block" << endl;
    fout << fixed << setprecision(6);

    double sum_A = 0.;
    double sum_A2 = 0.;

    double sum_block = 0.;
    int current_block = 0;

    for (int i = 0; i < M; i++){

        sum_block += data[i];
        
        if ( (i+1)%L == 0 ){
            current_block ++;

            double A = sum_block / L;
            
            sum_A += A;
            sum_A2 += A * A;

            double mean = sum_A / current_block;
            double variance = (current_block > 1) ? (sum_A2 - (sum_A * sum_A) / current_block) / (current_block - 1) : 0.;
            double error = (current_block > 1) ? sqrt(abs(variance) / current_block) : 0.;
            
            fout << setw(1) << current_block 
                 << setw(20) << mean 
                 << setw(25) << error << endl;

            sum_block = 0.0; 
        }

    }

    fout.close();

}

void DataBlocking :: AnalizeBlocksAverages (const std::vector<double>& blocks_averages){

    ofstream fout(m_namefile);
    if (!fout.is_open()) {
        cerr << "Error: the file has not been opened!" << endl;
        return;
    }

    fout << "# n.block   mean for n.block   error for n.block" << endl;
    fout << fixed << setprecision(6);

    double sum_A = 0.0;
    double sum_A2 = 0.0;
    int n_block = blocks_averages.size();

    for (int n = 1; n <= n_block; n++) {

        double A = blocks_averages[n - 1];

        sum_A += A;
        sum_A2 += A * A;

        double mean = sum_A / n;
        double variance = (n > 1) ? (sum_A2 - (sum_A * sum_A) / n) / (n - 1) : 0.0;
        double error = (n > 1) ? sqrt(abs(variance) / n) : 0.0;

        fout << setw(1) << n << setw(20) << mean << setw(25) << error << endl;
    }

    fout.close();

}

void DataBlocking :: AddMeasure (double x){

    if (m_L <= 0) {
        cerr << "Error: L step size was not properly set for streaming mode!" << endl;
        return;
    }

    m_sum_block += x;
    m_current_step ++;

    if ( m_current_step == m_L){
        m_block_count ++;

        double A = m_sum_block / static_cast<double>(m_L);
            
        m_sum_A += A;
        m_sum_A2 += A * A;

        double mean = m_sum_A / m_block_count;
        double variance = (m_block_count > 1) ? (m_sum_A2 - (m_sum_A * m_sum_A) / m_block_count) / (m_block_count - 1) : 0.;
        double error = (m_block_count > 1) ? sqrt(abs(variance) / m_block_count) : 0.;
            
        if (m_fileout.is_open()) {
            m_fileout << setw(8) << m_block_count 
                      << setw(20) << mean 
                      << setw(25) << error << endl;
        }

        m_sum_block = 0.;
        m_current_step = 0;

    }

}

pair<double, double> DataBlocking :: GetFinalStats(const std::vector<double>& blocks_averages){

    int N = blocks_averages.size();

    if (N == 0) {
        cerr << "Error: blocks_averages vector is empty in GetFinalStats!" << endl;
        return {0.0, 0.0};
    }

    double sum_A = 0.0;
    double sum_A2 = 0.0;

    for (int i = 0; i < N; i++) {
        double A = blocks_averages[i];
        sum_A += A;
        sum_A2 += A * A;
    }

    double mean = sum_A / static_cast<double>(N);
    double variance = (N > 1) ? (sum_A2 - (sum_A * sum_A) / N) / (N - 1) : 0.0;
    double error = (N > 1) ? sqrt(abs(variance) / N) : 0.0;

    return {mean, error};

}

void DataBlocking::Close() {
    if (m_fileout.is_open()) {
        m_fileout.close();
    }
}


