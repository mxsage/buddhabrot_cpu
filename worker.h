#pragma once

#include "params.h"
#include "worker_thread.h"
#include "bitmap.h"

#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <numeric>

class Worker {
private:
    std::vector<std::unique_ptr<WorkerThread>> threads;

	std::mutex mu;
	std::vector<Complex> hints;
    std::vector<long> points;

	int calculate_value(const double& x, const double& y) const;
	bool good_point(const double& x, const double& y) const;
	void calculate_hints_range(int low_r, int high_r, std::deque<int>& row_queue);
	void calculate_hints(int num_threads);
    void update_points_from_thread(std::unique_ptr<WorkerThread>& t);

	void save_hints_to_file(std::string filename);

	long calculate_histogram_interval(std::vector<long>& v) const;
	std::vector<long> get_interval_points();

public:
    Worker();
	void calculate_and_save_hints(int num_threads, std::string filename);
	bool read_hints_from_file(std::string filename);
	void computation(int num_threads);
    void save_image(std::string filename);
};
