#pragma once

#include "params.h"
#include "utils.h"
#include <thread>
#include <string>
#include <random>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>

/* Thread to compute sets of points that contribute to the final image. */
class WorkerThread {
    std::thread t;
    int seed, samples_so_far;
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;
    std::uniform_int_distribution<int> hint_dist;
	std::vector<Complex>* hints;

    void record(Complex& o);
    Complex get_rand();
    bool iteration();
    void compute();

public:
	std::unordered_map<int, unsigned long> point_map;
    WorkerThread(int id, std::vector<Complex>* _hints);
    ~WorkerThread();

    void start();
    void finish();

};
