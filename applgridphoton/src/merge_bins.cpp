#include <appl_grid/appl_grid.h>

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: merge_bins outgrid grid1 [grid2 ...]\n";
    }

    std::vector<std::string> args(&argv[2], &argv[argc]);

    std::vector<appl::grid> grids;
    grids.reserve(args.size());

    for (auto const& arg : args)
    {
        grids.emplace_back(arg);
    }

    appl::grid result(std::move(grids));
    result.Write(argv[1]);
}
