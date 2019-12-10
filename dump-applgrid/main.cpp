#include <appl_grid/appl_grid.h>
#include <appl_grid/appl_igrid.h>

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: dump-applgrid path-to-applgrid-file\n";
        return 1;
    }

    appl::grid grid{std::string{argv[1]}};

    double const run = (grid.run() == 0.0) ? 1.0 : grid.run();

    for (std::size_t order = 0; order != grid.order_ids().size(); ++order)
    {
        auto const& order_id = grid.order_ids().at(order);

        std::cout << ">>> found O(as^" << order_id.alphs() << " a^" << order_id.alpha() << ")\n";

        auto const& order_name = "as" + std::to_string(order_id.alphs()) + "a"
            + std::to_string(order_id.alpha());

        for (int bin = 0; bin != grid.Nobs_internal(); ++bin)
        {
            auto const& igrid = *grid.weightgrid(order, bin);
            double bin_width = grid.deltaobs(bin);

            if (igrid.weightgrid() == nullptr)
            {
                continue;
            }

            auto const& obs_name = "_bin_" + std::to_string(bin);

            for (int k = 0; k != igrid.SubProcesses(); ++k)
            {
                if (igrid.weightgrid(k) == nullptr)
                {
                    continue;
                }

                auto const& sub_name = "_sub_" + std::to_string(k);
                std::ofstream file;
                auto const& sub_grid = *igrid.weightgrid(k);

                for (int tau = 0; tau != igrid.Ntau(); ++tau)
                {
                    double as = 0.118;
                    double factor = std::pow(4.0 * std::acos(-1.0) * as, order_id.alphs());

                    for (int y1 = 0; y1 != igrid.Ny1(); ++y1)
                    {
                        double x1 = igrid.fx(igrid.gety1(y1));

                        for (int y2 = 0; y2 != igrid.Ny2(); ++y2)
                        {
                            double x2 = igrid.fx(igrid.gety2(y2));

                            double value = sub_grid(tau, y1, y2);

                            if (value != 0.0)
                            {
                                value /= run * x1 * x2 * bin_width;
                                value *= factor;

                                if (!file.is_open())
                                {
                                    file.open(order_name + obs_name + sub_name);
                                }

                                if (igrid.reweight())
                                {
                                    value *= appl::igrid::weightfun(x1);
                                    value *= appl::igrid::weightfun(x2);
                                }

                                file << y1 << ' ' << y2 << ' ' << tau << ' ' << std::scientific
                                    << value << '\n';
                            }
                        }
                    }
                }
            }
        }
    }
}
