#ifndef APPL_GRID_HISTO1D_HPP
#define APPL_GRID_HISTO1D_HPP

//#include <cstddef>

namespace appl
{

class histo1d
{
public:
    histo1d(void* pointer);

    int Fill(double x, double w);
    void Scale(double c);
    double GetBinContent(int bin) const;
    void SetBinContent(int bin, double content);
    int GetNbinsX() const;
    double GetBinWidth(int bin) const;

    void* pointer() const;

private:
    void* pointer_;
};

}

#endif
