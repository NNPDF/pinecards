#include "applgriddiff.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <TFile.h>

std::unique_ptr<TFile> load_root_file(std::string const& file)
{
    return std::unique_ptr<TFile>{new TFile{file.c_str()}};
}

void print_help()
{
    std::cout << "Usage: applgriddiff file1 file2\n";
}

int main(int argc, char* argv[])
{
    // store arguments in a vector
    std::vector<std::string> args(argv, argv + argc);

    if (args.size() != 3)
    {
        print_help();

        return -1;
    }

    // TODO: make sure both files actually exist

    auto file1 = load_root_file(args[1]);
    auto file2 = load_root_file(args[2]);

    bool const diff = dispatch_and_diff(*file1, *file2);

    return diff ? 1 : 0;
}
