#include "worker.h"

Worker::Worker() :
    points(p.dim*p.dim, 0)
{}

void Worker::calculate_and_save_hints(int num_threads, std::string filename)
{
    calculate_hints(num_threads);
    save_hints_to_file(filename);
}

void Worker::computation(int num_threads)
{
    for (int i=0; i<num_threads; i++)
    {
        threads.push_back(
            std::unique_ptr<WorkerThread>(new WorkerThread(i, &hints)));
        threads[i]->start();
    }

    for (auto& t : threads)
    {
        t->finish();
        update_points_from_thread(t);
    }
}


void Worker::save_hints_to_file(std::string filename)
{
    std::ofstream ofile(filename, std::ios::binary);

    if (ofile.is_open())
    {
        for (auto const& value : hints)
        {
            ofile.write(reinterpret_cast<const char*>(&value),
                        sizeof(value));
        }
        ofile.close();
    }
}

bool Worker::read_hints_from_file(std::string filename)
{
    std::ifstream ifile(filename, std::ios::binary);

    if (ifile.is_open())
    {
        Complex value{0.0, 0.0};
        while (true)
        {
            ifile.read(reinterpret_cast<char*>(&value),
                       sizeof(value));
            if (ifile.fail() || ifile.eof()) break;
            hints.push_back(value);
        }

        ifile.close();
    }

    return false;
}

int Worker::calculate_value(const double& x, const double& y) const
{
    Complex co {0.0, 0.0};
    Complex o {x, y};

    int iterations = p.depth;
    int escaped_after = 0;

    while (escaped_after < iterations && !escaped(co))
    {
        recurse(co, o);
        escaped_after++;
    }

    return escaped_after;
}

bool Worker::good_point(const double& x, const double& y) const
{
    auto val = calculate_value(x, y);
    return p.min_depth < val && val < p.depth;
}

void Worker::calculate_hints_range(int low_r, int high_r, std::deque<int>& row_queue)
{
    while (true)
    {
        int r = 0;
        {
            std::lock_guard<std::mutex> lk(mu);
            if (row_queue.empty())
            {
                return;
            } else {
                r = row_queue.back();
                row_queue.pop_back();
            }
        }
        std::cout << r << std::endl;
        for (int c=0; c<p.dim; c++)
        {
            double x = get_x(c);
            double y = get_y(r);
            if (good_point(x, y))
            {
                {
                    std::lock_guard<std::mutex> lk(mu);
                    hints.push_back(Complex{x, y});
                }
            }
        }
    }
}

void Worker::calculate_hints(int num_threads)
{
    std::vector<std::unique_ptr<std::thread>> hint_threads;

    std::deque<int> row_queue(p.dim);
    std::iota(row_queue.begin(), row_queue.end(), 0);

    int low_r = 0;
    for (int i=0; i<num_threads; i++)
    {
        int high_r = p.dim - (num_threads - i - 1) * (p.dim / num_threads);
        hint_threads.push_back(std::unique_ptr<std::thread>(
                    new std::thread(&Worker::calculate_hints_range, this, low_r, high_r, std::ref(row_queue))));
        low_r = high_r;
    }

    for (auto& t : hint_threads)
    {
        if (t->joinable())
        {
            t->join();
        }
    }

}

long Worker::calculate_histogram_interval(std::vector<long>& v) const
{
    long interval {0};
    std::for_each(std::begin(v), std::end(v),
            [&] (long n) { interval += n; } );
    return interval / 255;
}

std::vector<long> Worker::get_interval_points()
{
    std::vector<long> sorted(points.size());
    std::copy(std::begin(points), std::end(points), std::begin(sorted));
    std::sort(std::begin(sorted), std::end(sorted));

    std::vector<long> breakpoints;
    long current_point_number {0};
    auto interval = calculate_histogram_interval(points);
    for (auto& e : sorted)
    {
        current_point_number += e;
        if (current_point_number > interval)
        {
            breakpoints.push_back(e);
            current_point_number = 0;
        }
    }

    return breakpoints;
}

void Worker::save_image(std::string filename)
{
    bitmap_image img(p.dim, p.dim);

    auto breakpoints = get_interval_points();

    for (int r=0; r<p.dim; r++)
    {
        for (int c=0; c<p.dim; c++)
        {
            long val = points[r*p.dim + c];

            int color {0};

            auto it = std::begin(breakpoints);
            while (it != std::end(breakpoints) && (*it) < val)
            {
                color++;
                it++;
            }

            color = 255 - color;

            img.set_pixel(r, c, color, color, color);

        }
    }
    img.save_image("./images/" + filename + ".bmp");
}

void Worker::update_points_from_thread(std::unique_ptr<WorkerThread>& t)
{
    for (const auto& e : t->point_map)
    {
        points[e.first] += e.second;
    }
}
