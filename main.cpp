#include "worker.h"
#include <sstream>
#include <thread>

bool parse_input(int& num_threads, int argc, char* argv[])
{
    bool valid_input = false;

    if (argc == 1)
    {
        num_threads = std::thread::hardware_concurrency() - 1;
        valid_input = true;
    }

    if (argc == 3)
    {
        std::string s(argv[1]);
        std::istringstream iss(argv[2]);
        if (iss >> num_threads)
        {
            if ((num_threads> 0) and (s == "--threads"))
            {
                valid_input = true;
            }
        }
    }

    if (!valid_input)
    {
        std::cout << "Usage: ./program  \nor ./program --threads <nb_of_threads>"
            << std::endl;
    }

    return valid_input;
}


int main(int argc, char* argv[])
{

    int num_threads;
    std::string filename = "chesnut";

    if (!parse_input(num_threads, argc, argv))
    {
        return EXIT_SUCCESS;
    }

    std::cout << "Initialize." << std::endl;
    Worker w;

    // must load hints or compute hints
    std::cout << "Hint stuff." << std::endl;
    w.calculate_and_save_hints(num_threads, "./data/chesnut_hints.bin");
    //w.read_hints_from_file("./data/chesnut_hints.bin");

    std::cout << "Computation with " << num_threads << " threads."  << std::endl;
    w.computation(num_threads);

    std::cout << "Saving image." << std::endl;
    w.save_image(filename);

    return EXIT_SUCCESS;
}
