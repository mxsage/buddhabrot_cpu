#pragma once

// Hold the parameters for the simulation
static struct Params {
    const int dim = 4096;
	const int min_depth = 10000;
    const int depth = 1000000;
    const int samples = 100000;
	const int samples_per_iteration = 1000;

	const double fuzz = 0.00001;
    const double threshold = 5.0;
    const double offset_x = 2.2;
    const double offset_y = 1.25;
    const double x_range = 2.5;
    const double y_range = 2.5;
} p;
