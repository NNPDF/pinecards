#!/usr/bin/env python3

from itertools import product
import logging
logging.basicConfig(level=logging.INFO)

import ROOT


def abs_name(obj):
    try:
        directory = obj.GetDirectory()
    except AttributeError:
        directory = None
    result = ""
    if directory:
        result += directory.GetPath().split(":")[1] + "/" + obj.GetName()
    else:
        result += obj.GetName()
    result += " ({})".format(obj.GetTitle())
    return result


def diff_axis(axis1, axis2):
    logging.debug("comparing axis %s and %s", axis1.GetName(), axis2.GetName())
    if (axis1.GetNbins() != axis2.GetNbins()):
        print("< {} n bins = {}".format(abs_name(axis1), axis1.GetNbins()))
        print("> {} n bins = {}".format(abs_name(axis2), axis2.GetNbins()))
        return False
    for ibin in range(1, (axis1.GetNbins() + 1)):
        if axis1.GetBinLowEdge(ibin) != axis2.GetBinLowEdge(ibin):
            print("< {} different binning (bin {} = {})".format(abs_name(axis1), ibin,
                                                                axis1.GetBinLowEdge(ibin)))
            print("> {} different binning (bin {} = {})".format(abs_name(axis2), ibin,
                                                                axis2.GetBinLowEdge(ibin)))
            return False
    return True


def diff_graph(graph1, graph2):
    def get_x(graph):
        x = graph.GetX()
        x.SetSize(graph.GetN())
        return x

    def get_y(graph):
        y = graph.GetY()
        y.SetSize(graph.GetN())
        return y

    if graph1.GetN() != graph2.GetN():
        print("< {} npoints = {}".format(graph1.GetN()))
        print("> {} npoints = {}".format(graph2.GetN()))
        return False
    # TODO(sort the points?)
    for ipoint, (x1, y1, x2, y2) in enumerate(zip(get_x(graph1), get_y(graph1),
                                                  get_x(graph2), get_y(graph2))):
        if (x1, y1) != (x2, y2):
            print("< {} point {} = ({}, {})".format(abs_name(graph1), ipoint, x1, y1))
            print("> {} point {} = ({}, {})".format(abs_name(graph2), ipoint, x2, y2))
            return False


def diff_graph_asym_errors(graph1, graph2):
    if not diff_graph(graph1, graph2):
        return False
    for i in range(graph1.GetN()):
        if graph1.GetErrorXhigh(i) != graph2.GetErrorXhigh(i):
            print("< {} point {} error x-up = {}".format(abs_name(graph1), i, graph1.GetErrorXhigh(i)))
            print("> {} point {} error x-up = {}".format(abs_name(graph2), i, graph2.GetErrorXhigh(i)))
            return False
        if graph1.GetErrorXlow(i) != graph2.GetErrorXlow(i):
            print("< {} point {} error x-low = {}".format(abs_name(graph1), i, graph1.GetErrorXlow(i)))
            print("> {} point {} error x-low = {}".format(abs_name(graph2), i, graph2.GetErrorXlow(i)))
            False
        if graph1.GetErrorYhigh(i) != graph2.GetErrorYhigh(i):
            print("< {} point {} error y-up = {}".format(abs_name(graph1), i, graph1.GetErrorYhigh(i)))
            print("> {} point {} error y-up = {}".format(abs_name(graph2), i, graph2.GetErrorYhigh(i)))
            return False
        if graph1.GetErrorYlow(i) != graph2.GetErrorYlow(i):
            print("< {} point {} error y-low = {}".format(abs_name(graph1), i, graph1.GetErrorYlow(i)))
            print("> {} point {} error y-low = {}".format(abs_name(graph2), i, graph2.GetErrorYlow(i)))
            False
        return True


def diff_histos(histo1, histo2):
    logging.debug("checking %s", histo1.GetName())
    
    lines = []

    if (histo1.GetNbinsX() != histo2.GetNbinsX()):
        lines.append("< nbins = {}".format(histo1.GetNbinsX()))
        lines.append("> nbins = {}".format(histo2.GetNbinsX()))
        
        
    for ibin in range(1, (histo1.GetNbinsX() + 1)):
        if histo1.GetBinLowEdge(ibin) != histo2.GetBinLowEdge(ibin):
            lines.append("< different binning (bin {} = {})".format(ibin, histo1.GetBinLowEdge(ibin)))
            lines.append("> different binning (bin {} = {})".format(ibin, histo2.GetBinLowEdge(ibin)))

    bins_different = []
    for ibin in range(1, (histo1.GetNbinsX() + 1)):
        if histo1.GetBinContent(ibin) != histo2.GetBinContent(ibin):
            bins_different.append(ibin)

    if bins_different:
        to_write = bins_different
        if len(bins_different) > 10:
            to_write = bins_different[:5]
        for ibin in to_write:
            bin_lo_edge = histo1.GetBinLowEdge(ibin)
            bin_hi_edge = histo1.GetBinLowEdge(ibin + 1)
            lines.append("< different content (bin {} ({}, {}) = {})".format(ibin,
                                                                             bin_lo_edge, bin_hi_edge,
                                                                             histo1.GetBinContent(ibin)))
            lines.append("> different content (bin {} ({}, {}) = {})".format(ibin,
                                                                             bin_lo_edge, bin_hi_edge,
                                                                             histo2.GetBinContent(ibin)))
        if (len(bins_different) > 10):
            lines.append(" other {} different bins".format(len(bins_different) - len(to_write)))

    bins_different = []
    for ibin in range(1, (histo1.GetNbinsX() + 1)):
        if histo1.GetBinError(ibin) != histo2.GetBinError(ibin):
            bins_different.append(ibin)

    if bins_different:
        to_write = bins_different
        if len(bins_different) > 10:
            to_write = to_write[:5]
        for ibin in to_write:
            bin_lo_edge = histo1.GetBinLowEdge(ibin)
            bin_hi_edge = histo1.GetBinLowEdge(ibin + 1)
            lines.append("< different error (bin {} ({}, {}) = {})".format(ibin,
                                                                           bin_lo_edge, bin_hi_edge,
                                                                           histo1.GetBinError(ibin)))
            lines.append("> different error (bin {} ({}, {}) = {})".format(ibin,
                                                                           bin_lo_edge, bin_hi_edge,
                                                                           histo2.GetBinError(ibin)))
        if (len(bins_different) > 10):
            lines.append(" other {} different bins".format(len(bins_different) - len(to_write)))


    if histo1.GetEntries() != histo2.GetEntries():
        lines.append("< different entries ({})".format(histo1.GetEntries()))
        lines.append("> different entries ({})".format(histo2.GetEntries()))

    if lines:
        print("< {}".format(abs_name(histo1)))
        print("> {}".format(abs_name(histo2)))
        print('\n'.join(("  " + l for l in lines)))

    return len(lines) > 0

def diff_histos3(histo1, histo2):
    logging.debug("checking %s", histo1.GetName())

    lines = []

    if (histo1.GetNbinsX() != histo2.GetNbinsX()):
        lines.append("< nbinsx = {}".format(histo1.GetNbinsX()))
        lines.append("> nbinsx = {}".format(histo2.GetNbinsX()))
    elif (histo1.GetNbinsY() != histo2.GetNbinsY()):
        lines.append("< nbinsy = {}".format(histo1.GetNbinsY()))
        lines.append("> nbinsy = {}".format(histo2.GetNbinsY()))
    elif (histo1.GetNbinsZ() != histo2.GetNbinsZ()):
        lines.append("< nbinsz = {}".format(histo1.GetNbinsZ()))
        lines.append("> nbinsz = {}".format(histo2.GetNbinsZ()))
    else:
        ran_x = range(1, (histo1.GetNbinsX() + 1))
        ran_y = range(1, (histo1.GetNbinsY() + 1))
        ran_z = range(1, (histo1.GetNbinsZ() + 1))
        prod = product(ran_x, ran_y, ran_z)

        for ibin in prod:
            if histo1.GetBinLowEdge(ibin) != histo2.GetBinLowEdge(ibin):
                lines.append("< different binning (bin {} = {})".format(ibin, histo1.GetBinLowEdge(ibin)))
                lines.append("> different binning (bin {} = {})".format(ibin, histo2.GetBinLowEdge(ibin)))

        if len(lines) == 0:
            bins_different = []
            for ibin in prod:
                if histo1.GetBinContent(ibin) != histo2.GetBinContent(ibin):
                    bins_different.append(ibin)

            if bins_different:
                to_write = bins_different

                # if there's more than ten different bins, pick the last five
                if len(bins_different) > 10:
                    to_write = bins_different[:5]
                for ibin in to_write:
                    lines.append("< different content (bin {} = {})".format(ibin, histo1.GetBinContent(ibin)))
                    lines.append("> different content (bin {} = {})".format(ibin, histo2.GetBinContent(ibin)))
                if (len(bins_different) > 10):
                    lines.append(" other {} different bins".format(len(bins_different) - len(to_write)))
#
#    bins_different = []
#    for ibin in range(1, (histo1.GetNbinsX() + 1)):
#        if histo1.GetBinError(ibin) != histo2.GetBinError(ibin):
#            bins_different.append(ibin)
#
#    if bins_different:
#        to_write = bins_different
#        if len(bins_different) > 10:
#            to_write = to_write[:5]
#        for ibin in to_write:
#            bin_lo_edge = histo1.GetBinLowEdge(ibin)
#            bin_hi_edge = histo1.GetBinLowEdge(ibin + 1)
#            lines.append("< different error (bin {} ({}, {}) = {})".format(ibin,
#                                                                           bin_lo_edge, bin_hi_edge,
#                                                                           histo1.GetBinError(ibin)))
#            lines.append("> different error (bin {} ({}, {}) = {})".format(ibin,
#                                                                           bin_lo_edge, bin_hi_edge,
#                                                                           histo2.GetBinError(ibin)))
#        if (len(bins_different) > 10):
#            lines.append(" other {} different bins".format(len(bins_different) - len(to_write)))
#
#
#    if histo1.GetEntries() != histo2.GetEntries():
#        lines.append("< different entries ({})".format(histo1.GetEntries()))
#        lines.append("> different entries ({})".format(histo2.GetEntries()))

    if lines:
        print("< {}".format(abs_name(histo1)))
        print("> {}".format(abs_name(histo2)))
        print('\n'.join(("  " + l for l in lines)))

    return len(lines) > 0


def diff_tf1(obj1, obj2):
    if obj1.GetExpFormula() != obj2.GetExpFormula():
        print("< different formula {}".format(obj1.GetExpFormula()))
        print("> different formula {}".format(obj2.GetExpFormula()))
        return False
    if obj1.GetNpar() != obj2.GetNpar():
        print("< different number of parameters {}".format(obj1.GetNpar))
        print("> different number of parameters {}".format(obj2.GetNpar))
        return False

    result = True
    if obj1.GetMaximumX() != obj2.GetMaximumX() or obj1.GetMinimumX() != obj2.GetMinimumX():
        print("< different range {} - {}".format(obj1.GetMinimumX(), obj1.GetMaximumX()))
        print("> different range {} - {}".format(obj2.GetMinimumX(), obj2.GetMaximumX()))
        result = False

    for ipar in range(obj1.GetNpar()):
        par1 = obj1.GetParameter(ipar)
        par2 = obj2.GetParameter(ipar)
        if (par1 != par2):
            print("< different parameter {} = {}".format(ipar, par1))
            print("> different parameter {} = {}".format(ipar, par2))
            result = False

    return result


def diff_list(list1, list2):
    result = True
    for obj1, obj2 in zip(list1, list2):
        result &= diff_obj(obj1, obj2)
    return result

def diff_tvector(vector1, vector2):
    if vector1.GetNrows() != vector2.GetNrows():
        print("< entries {}".format(vector1.GetNrows()))
        print("> entries {}".format(vector2.GetNrows()))

        return False
    else:
        for i in range(vector1.GetNrows()):
            if vector1[i] != vector2[i]:
                print("vectors are different")
                return False

    return True


def diff_obj(obj1, obj2):
    if type(obj1) != type(obj2):
        print("< {} type = {}".format(abs_name(obj1), type(obj1)))
        print("> {} type = {}".format(abs_name(obj2), type(obj2)))
        return False

    if issubclass(type(obj1), ROOT.TDirectory):
        return diff_directory(obj1, obj2)

    if type(obj1) is ROOT.TAxis:
        return diff_axis(obj1, obj2)

    if type(obj1) is ROOT.TGraph:
        return diff_graph(obj1, obj2)

    if type(obj1) in (ROOT.TGraphErrors, ROOT.TGraphAsymmErrors):
        return diff_graph_asym_errors(obj1, obj2)

    if issubclass(type(obj1), ROOT.TH2):
        logging.warning("cannot compare %s: type %s not supported", abs_name(obj1), type(obj1))
        return True

    if type(obj1) in (ROOT.TH1, ROOT.TH1F, ROOT.TH1D, ROOT.TH1I, ROOT.TProfile):
        return diff_histos(obj1, obj2)

    if type(obj1) in (ROOT.TH3, ROOT.TH3F, ROOT.TH3D, ROOT.TH3D):
        return diff_histos3(obj1, obj2)

    if type(obj1) is ROOT.TF1:
        return diff_tf1(obj1, obj2)

    if type(obj1) is ROOT.TVectorD:
        return diff_tvector(obj1, obj2)

    if type(obj1) is ROOT.TList:
        return diff_list(obj1, obj2)

    logging.warning("cannot compare %s: type %s not supported", abs_name(obj1), type(obj1))
    return True


def diff_directory(directory1, directory2, different_names=False):
    if different_names:
        # uses for the TFile
        logging.debug("checking %s and %s", directory1.GetName(), directory2.GetName())
    else:
        logging.debug("checking %s", directory1.GetName())
    keys1 = [k.GetName() for k in directory1.GetListOfKeys()]
    keys2 = [k.GetName() for k in directory2.GetListOfKeys()]

    d12 = set(keys1) - set(keys2)
    if d12:
        for k in sorted(d12):
            print("< {}".format(k))
    d21 = set(keys2) - set(keys1)
    if d21:
        for k in sorted(d21):
            print("> {}".format(k))

    common_keys = set(keys1).intersection(keys2)

    all_equal = True
    for k in common_keys:
        try:
            obj1 = directory1.Get(k)
            obj2 = directory2.Get(k)

            all_equal &= diff_obj(obj1, obj2)
        except (TypeError):
            print("ignore custom type")

    return all_equal


def diff(first_file, second_file):
    f1 = ROOT.TFile.Open(first_file)
    f2 = ROOT.TFile.Open(second_file)

    if not f1:
        print("{} do not exists. Exit".format(first_file))
        return 1
    if not f2:
        print("{} do not exists. Exit".format(second_file))
        return 1

    return diff_directory(f1, f2, True)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("first_file", help="first file")
    parser.add_argument("second_file", help="second file")

    args = parser.parse_args()

    diff(args.first_file, args.second_file)
