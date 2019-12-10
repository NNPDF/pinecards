#include <appl_grid/appl_grid.h>
#include <appl_grid/appl_igrid.h>
#include <appl_grid/appl_pdf.h>
#include <appl_grid/lumi_pdf.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: pdf_info grid\n";

        return 1;
    }

    appl::grid const grid(argv[1]);

    std::vector<std::string> strings;

    for (int i = 0; i != grid.order_ids().size(); ++i)
    {
        auto const& pdf = *dynamic_cast <lumi_pdf const*> (grid.genpdf(i));

        for (std::size_t k = 0; k != pdf.size(); ++k)
        {
            for (int j = 0; j != grid.Nobs_internal(); ++j)
            {
                auto const& sparse_grid = *grid.weightgrid(i, j)->weightgrid(k);

                if (sparse_grid.size() != 0)
                {
                    if (strings.size() <= k)
                    {
                        strings.resize(k + 1);
                    }

                    std::ostringstream out;

                    out << "O(as^" << grid.order_ids().at(i).alphs() << " a^"
                        << grid.order_ids().at(i).alpha() << ") iobs=" << j << " pdf=" << k
                        << " non-zero combination (size=" << sparse_grid.size() << ") = " << pdf[k]
                        << '\n';

                    strings.at(k).append(out.str());
                }
            }
        }
    }

    for (auto const& string : strings)
    {
        std::cout << string;
    }
}
