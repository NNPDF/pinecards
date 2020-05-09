#include <pineappl_capi.h>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: merge_bins outgrid grid1 [grid2 ...]\n";

        return 1;
    }

    std::vector<std::string> args(&argv[3], &argv[argc]);

    auto* grid = pineappl_grid_read(argv[2]);

    for (auto const& arg : args)
    {
        auto* other = pineappl_grid_read(arg.c_str());
        pineappl_grid_merge_and_delete(grid, other);
    }

    pineappl_grid_write(grid, argv[1]);
}
