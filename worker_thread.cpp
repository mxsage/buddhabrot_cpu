#include "worker_thread.h"

WorkerThread::WorkerThread(int id, std::vector<Complex>* _hints) :
    hints(_hints),
    samples_so_far(0),
    mt(rd()),
    dist(std::uniform_real_distribution<double>(-p.fuzz, p.fuzz)),
    hint_dist(std::uniform_int_distribution<int>(0, _hints->size() - 1)),
    seed(id)
{
    mt.seed(seed);
}

void WorkerThread::start(){
    t = std::thread(&WorkerThread::compute, this);
}


void WorkerThread::record(Complex& o) {
    int d = 0;

    Complex c {0.0, 0.0};

    while (!escaped(c) && d < p.depth) {
        recurse(c, o);

        int i = get_i(c.r);
        int j = get_j(c.i);

        if (in_bounds(i) && in_bounds(j)) {
			int index = p.dim*j + i;

			auto got = point_map.find(index);
			if (got == point_map.end()) {
				point_map.insert({index, 1});
			} else {
				got->second++;
			}
        }

        d++;
    }
}

Complex WorkerThread::get_rand() {
    int idx = hint_dist(mt);
    Complex hint = hints->at(idx);

    hint.r += dist(mt);
    hint.i += dist(mt);
    return hint;
}

void WorkerThread::compute() {
    bool finished = false;
    while (not finished) {
        finished = iteration();
    }
}

bool WorkerThread::iteration() {
    for (int i=0; i<p.samples_per_iteration; i++) {
        Complex cur {0.0, 0.0};
        Complex original = get_rand();

        for (int d=0; d<p.depth; d++) {
            recurse(cur, original);

            if (escaped(cur)) {
				if (d > p.min_depth) {
					record(original);
				}
				break;
			}
		}
	}

    samples_so_far += p.samples_per_iteration;

    return samples_so_far >= p.samples;
}

void WorkerThread::finish(){
    if (t.joinable()){
        t.join();
    }
}

WorkerThread::~WorkerThread() {
    if (t.joinable()) {
        t.join();
    }
}
