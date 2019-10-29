#include "appl_grid/histo1d.hpp"

#include <TH1D.h>

namespace appl
{

histo1d::histo1d(void* pointer)
    : pointer_{pointer}
{
}

int histo1d::Fill(double x, double w)
{
    return static_cast <TH1D*> (pointer_)->Fill(x, w);
}

void histo1d::Scale(double c)
{
    static_cast <TH1D*> (pointer_)->Scale(c);
}

double histo1d::GetBinContent(int bin) const
{
    return static_cast <TH1D*> (pointer_)->GetBinContent(bin);
}

void histo1d::SetBinContent(int bin, double content)
{
    static_cast <TH1D*> (pointer_)->SetBinContent(bin, content);
}

int histo1d::GetNbinsX() const
{
    return static_cast <TH1D*> (pointer_)->GetNbinsX();
}

double histo1d::GetBinWidth(int bin) const
{
    return static_cast <TH1D*> (pointer_)->GetBinWidth(bin);
}

void* histo1d::pointer() const
{
    return pointer_;
}

}
