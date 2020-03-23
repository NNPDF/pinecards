#ifndef APPLGRIDDIFF_HPP
#define APPLGRIDDIFF_HPP

#include <TDirectory.h>
#include <TH1.h>
#include <TObject.h>
#include <TVectorT.h>

bool diff_directory(TDirectory& dir1, TDirectory& dir2);

bool diff_histogram(TH1 const& obj1, TH1 const& obj2);

template <typename T>
bool diff_vector_t(TVectorT<T> const& obj1, TVectorT<T> const& obj2);

bool dispatch_and_diff(TObject& obj1, TObject& obj2);

#endif
