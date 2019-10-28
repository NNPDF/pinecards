#include "sparse_matrix3d_root_interface.hpp"

#include "tsparse2d.h"
#include "tsparse1d.h"

#include <TH1D.h>

SparseMatrix3d* sparse_matrix3d_from_th3d(TH3D const& histogram)
{
    // TODO: are the following casts neccessary?
    TH1D* hx = (TH1D*)(&histogram)->Project3D("x");
    TH1D* hy = (TH1D*)(&histogram)->Project3D("y");
    TH1D* hz = (TH1D*)(&histogram)->Project3D("z");

    auto* matrix = new SparseMatrix3d(
        hx->GetNbinsX(), hx->GetBinCenter(1), hx->GetBinCenter(hx->GetNbinsX()),
        hy->GetNbinsX(), hy->GetBinCenter(1), hy->GetBinCenter(hy->GetNbinsX()),
        hz->GetNbinsX(), hz->GetBinCenter(1), hz->GetBinCenter(hz->GetNbinsX())
    );

    delete hx;
    delete hy;
    delete hz;

    for (std::size_t i = 0; i < matrix->xaxis().N(); ++i)
    {
        for (std::size_t j = 0; j < matrix->yaxis().N(); ++j)
        {
            for (std::size_t k = 0; k < matrix->zaxis().N(); ++k)
            {
                (*matrix)(i,j,k) = histogram.GetBinContent(i + 1, j + 1, k + 1);
            }
        }
    }

    return matrix;
}

TH3D* sparse_matrix3d_to_th3d(SparseMatrix3d const& matrix, std::string const& s)
{
    double const delx = matrix.xaxis().delta();
    double const dely = matrix.yaxis().delta();
    double const delz = matrix.zaxis().delta();

    TH3D* h = new TH3D(
        s.c_str(),
        s.c_str(),
        matrix.xaxis().N(),
        matrix.xaxis().min()-0.5*delx,
        matrix.xaxis().max()+0.5*delx,
        matrix.yaxis().N(),
        matrix.yaxis().min()-0.5*dely,
        matrix.yaxis().max()+0.5*dely,
        matrix.zaxis().N(),
        matrix.zaxis().min()-0.5*delz,
        matrix.zaxis().max()+0.5*delz
    );

    for (std::size_t i = matrix.lo(); i <= matrix.hi(); ++i)
    {
        tsparse2d<double> const* s2d = matrix[i];

        if (s2d == nullptr)
        {
            continue;
        }

        for (std::size_t j = s2d->lo(); j <= s2d->hi(); ++j)
        {
            tsparse1d<double> const* s1d = (*s2d)[j];

            if (s1d == nullptr)
            {
                continue;
            }

            for (std::size_t k = s1d->lo(); k <= s1d->hi(); ++k)
            {
                h->SetBinContent(i + 1, j + 1, k + 1, (*s1d)(k));
            }
        }
    }

    return h;
}
