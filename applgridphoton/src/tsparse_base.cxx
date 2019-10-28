#include "appl_grid/tsparse_base.h"

#include <iostream>

tsparse_base::out_of_range::out_of_range(const std::string& s)
{
    std::cerr << s << std::endl;
}
