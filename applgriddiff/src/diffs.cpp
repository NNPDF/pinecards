#include "applgriddiff.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <TH3.h>
#include <TKey.h>

bool diff_directory(TDirectory& dir1, TDirectory& dir2)
{
    auto dir1_keys = dir1.GetListOfKeys();
    auto dir2_keys = dir2.GetListOfKeys();

    std::vector<std::string> dir1_names;
    std::vector<std::string> dir2_names;

    for (auto const& key : *dir1_keys)
    {
        dir1_names.emplace_back(key->GetName());
    }

    for (auto const& key : *dir2_keys)
    {
        dir2_names.emplace_back(key->GetName());
    }

    if (dir1_names.size() != dir2_names.size())
    {
        // TODO: print message
        return true;
    }

    std::sort(dir1_names.begin(), dir1_names.end());
    std::sort(dir2_names.begin(), dir2_names.end());

    if (!std::equal(dir1_names.begin(), dir1_names.end(), dir2_names.begin()))
    {
        // TODO: print message
        return true;
    }

    for (auto const& name : dir1_names)
    {
        auto* cls1 = dir1.FindKey(name.c_str())->GetClassName();
        auto* cls2 = dir1.FindKey(name.c_str())->GetClassName();

        if ((cls1 != std::string("TFileString")) && (cls2 != std::string("TFileString")))
        {
            auto* obj1 = dir1.Get(name.c_str());
            auto* obj2 = dir2.Get(name.c_str());

            if ((obj1 == nullptr) && (obj2 == nullptr))
            {
                // TODO: print message
                continue;
            }

            if (dispatch_and_diff(*obj1, *obj2))
            {
                return true;
            }
        }
        else
        {
            // TODO: print warning that we are ignoring TFileString objects?
            return false;
        }
    }

    return false;
}

bool diff_histogram(TH1 const& obj1, TH1 const& obj2)
{
    if (obj1.GetNbinsX() != obj2.GetNbinsX())
    {
        // TODO: print message
        return true;
    }
    else if (obj1.GetNbinsX() != obj2.GetNbinsX())
    {
        // TODO: print message
        return true;
    }
    else if (obj1.GetNbinsX() != obj2.GetNbinsX())
    {
        // TODO: print message
        return true;
    }

    // remember: bin=0 is the underflow bin, last bin is overflow bin

    for (int x = 0; x != obj1.GetNbinsX() + 1; ++x)
    {
        if (obj1.GetXaxis()->GetBinLowEdge(x) != obj2.GetXaxis()->GetBinLowEdge(x))
        {
            // TODO: print message
            return true;
        }
    }

    for (int y = 0; y != obj1.GetNbinsY() + 1; ++y)
    {
        if (obj1.GetYaxis()->GetBinLowEdge(y) != obj2.GetYaxis()->GetBinLowEdge(y))
        {
            // TODO: print message
            return true;
        }
    }

    for (int z = 0; z != obj1.GetNbinsZ() + 1; ++z)
    {
        if (obj1.GetZaxis()->GetBinLowEdge(z) != obj2.GetZaxis()->GetBinLowEdge(z))
        {
            // TODO: print message
            return true;
        }
    }

    std::vector<std::tuple<int, int, int>> differing_bins;

    for (int x = 0; x != obj1.GetNbinsX() + 1; ++x)
    {
        for (int y = 0; y != obj1.GetNbinsY() + 1; ++y)
        {
            for (int z = 0; z != obj1.GetNbinsZ() + 1; ++z)
            {
                if (obj1.GetBinContent(x, y, z) != obj2.GetBinContent(x, y, z))
                {
                    differing_bins.emplace_back(x, y, z);
                }

                // TODO: are the more properties that need to be compared?
            }
        }
    }

    auto const& name1 = obj1.GetName();
    auto const& name2 = obj1.GetName();

    for (auto const& bin : differing_bins)
    {
        int const x = std::get<0>(bin);
        int const y = std::get<1>(bin);
        int const z = std::get<2>(bin);

        std::cout << std::scientific
            << std::setprecision(std::numeric_limits<Double_t>::max_digits10 - 1)
            << "--- " << name1 << " (" << x << ", " << y << ", " << z << ") = "
            << obj1.GetBinContent(x, y, z) << '\n'
            << "+++ " << name2 << " (" << x << ", " << y << ", " << z << ") = "
            << obj2.GetBinContent(x, y, z) << '\n';
    }

    return !differing_bins.empty();
}

template <typename T>
bool diff_vector_t(TVectorT<T> const& obj1, TVectorT<T> const& obj2)
{
    if (obj1.GetNoElements() != obj2.GetNoElements())
    {
        // TODO: print message
        return true;
    }

    std::vector<int> differing_indices;

    for (int i = 0; i != obj1.GetNoElements(); ++i)
    {
        if (obj1[i] != obj2[i])
        {
            differing_indices.push_back(i);
        }
    }

    auto const& name1 = obj1.GetName();
    auto const& name2 = obj1.GetName();

    for (auto const& index : differing_indices)
    {
        std::cout << std::scientific
            << std::setprecision(std::numeric_limits<Double_t>::max_digits10 - 1)
            << "--- " << name1 << " (" << index << ") = "
            << obj1[index] << '\n'
            << "+++ " << name2 << " (" << index << ") = "
            << obj2[index] << '\n';
    }

    return !differing_indices.empty();
}

bool dispatch_and_diff(TObject& obj1, TObject& obj2)
{
    auto const& name1 = std::string(obj1.ClassName());
    auto const& name2 = std::string(obj2.ClassName());

    if (name1 != name2)
    {
        // TODO: print message
        return true;
    }

    auto const& name = name1;

    if ((name == "TDirectoryFile") || (name == "TFile"))
    {
        auto& dir1 = dynamic_cast <TDirectory&> (obj1);
        auto& dir2 = dynamic_cast <TDirectory&> (obj2);

        return diff_directory(dir1, dir2);
    }
    else if (name == "TH1D")
    {
        auto const& hist1 = dynamic_cast <TH1D&> (obj1);
        auto const& hist2 = dynamic_cast <TH1D&> (obj2);

        return diff_histogram(hist1, hist2);
    }
    else if (name == "TH3D")
    {
        auto const& hist1 = dynamic_cast <TH3D&> (obj1);
        auto const& hist2 = dynamic_cast <TH3D&> (obj2);

        return diff_histogram(hist1, hist2);
    }
    else if (name == "TVectorT<double>")
    {
        auto const& vector1 = dynamic_cast <TVectorT<double>&> (obj1);
        auto const& vector2 = dynamic_cast <TVectorT<double>&> (obj2);

        return diff_vector_t(vector1, vector2);
    }
    else
    {
        // in that case the implementation is incomplete
        assert( false );
    }

    return false;
}

// --------------------------------- TEMPLATE INSTATIATIONS ----------------------------------------

template bool diff_vector_t(TVectorT<double> const&, TVectorT<double> const&);
