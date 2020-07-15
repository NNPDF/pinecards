#!/usr/env/bin python
"""
Plotting script for pineappl grid analysis.
"""
import os
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


parser = argparse.ArgumentParser()
parser.add_argument('--qcd', default=None, type=str, required=True)
parser.add_argument('--qcdew', default=None, type=str, required=True)
parser.add_argument('--nbins', default=None, nargs="+", required=True)
parser.add_argument('--bins', default=None, nargs="+", required=True)
parser.add_argument('--xscale', default='linear', type=str)
parser.add_argument('--yscale', default='linear', type=str)
parser.add_argument('--title', default='Use --title option', type=str)
parser.add_argument('--ylabel', default='Use --ylabel option', type=str)
parser.add_argument('--xlabel', default='Use --xlabel option', type=str)
parser.add_argument('--ylim', nargs="+", default=None, type=str)
parser.add_argument('--xlim', nargs="+", default=None, type=str)
parser.add_argument('--legend', default='best', type=str)
parser.add_argument('--legend-lower', default='best', type=str)
parser.add_argument('--multiply', default=1, type=float)
parser.add_argument('--output', default='pineappl_accuracy.pdf', type=str)
args = parser.parse_args()


def load_results_log(logfolder, bins, nbins):
    """Load data from results.log.

    Args:
        logfolder (str): path to pineappl output.

    Returns:
        Pandas dataframe containing the results.log data.
    """
    with open(f'{logfolder}/results.log', 'r') as f:
        minbin = int(nbins[0])
        maxbin = int(nbins[1])
        data_from_file = f.readlines()[3+minbin:3+maxbin+1]
        data = np.zeros(shape=(maxbin-minbin+2, 7))

        bindex = minbin
        for w, line in enumerate(data_from_file):
            index = 0
            for j in line.split(' '):
                if j == '':
                    continue
                data[w, index] = np.float64(j)
                index += 1
            data[w, index] = np.float64(bins[w])
            data[w, index+1] = np.float64(bins[w+1])
            bindex += 1
        data[-1] = data[-2]
        data[-1, -2] = data[-2, -1]
        data[:, 0:3] *= args.multiply
        columns = ['pineappl', 'mg5_amc', 'mg5_amc_unc', 'sigmas',
                   'permille', 'bin']
        df = pd.DataFrame(data[:,:-1], columns=columns)
    return df


def plot_pineappl(qcd_data, qcdew_data):
    """Plot a comparison between the MC reference and pineappl convolution.

    Args:
        qcd_data (pandas.DataFrame): dataframe containing the QCD data.
        qcdew_data (pandas.DataFrame): dataframe containin the QCD+EW data.
    """
    alpha = 0.5
    colors = ['C0', 'C1']
    plt.figure(figsize=(6, 9))
    axs = []
    axs.append(plt.subplot2grid((4, 1), (0, 0), rowspan=2))
    axs.append(plt.subplot2grid((4, 1), (2, 0), sharex=axs[0]))
    axs.append(plt.subplot2grid((4, 1), (3, 0), sharex=axs[0]))

    # physics plot
    axs[0].fill_between(qcdew_data['bin'],
                        qcdew_data['pineappl'],
                        qcdew_data['pineappl'],
                        step='post', color=colors[0])

    leg1 = mpatches.Patch(color=colors[0], label='PineAPPL NLO QCD+EW')
    leg2 = mpatches.Patch(color=colors[1], label='PineAPPL NLO QCD')

    axs[0].legend(handles=[leg1, leg2], loc=args.legend, frameon=False)

    # physics plot ratio
    axs[1].fill_between(qcd_data['bin'],
                        (qcd_data['mg5_amc']-qcd_data['mg5_amc_unc'])/qcdew_data['mg5_amc'],
                        (qcd_data['mg5_amc']+qcd_data['mg5_amc_unc'])/qcdew_data['mg5_amc'],
                        step='post', color=colors[0], alpha=alpha)
    axs[1].fill_between(qcd_data['bin'],
                        qcd_data['pineappl']/qcdew_data['mg5_amc'],
                        qcd_data['pineappl']/qcdew_data['mg5_amc'],
                        step='post', color=colors[0])
    axs[1].fill_between(qcdew_data['bin'],
                        (qcdew_data['mg5_amc']-qcdew_data['mg5_amc_unc'])/qcdew_data['mg5_amc'],
                        (qcdew_data['mg5_amc']+qcdew_data['mg5_amc_unc'])/qcdew_data['mg5_amc'],
                        step='post', color=colors[1], alpha=alpha)
    axs[1].fill_between(qcdew_data['bin'],
                        qcdew_data['pineappl']/qcdew_data['mg5_amc'],
                        qcdew_data['pineappl']/qcdew_data['mg5_amc'],
                        step='post', color=colors[1])
    axs[1].set_ylabel('NLO QCD+EW / NLO QCD\n(MC uncertainty)')

    # ratio pineappl/mg5 qcd
    axs[2].fill_between(qcdew_data['bin'],
                        (qcdew_data['pineappl']/qcdew_data['mg5_amc'] - 1)*1000,
                        (qcdew_data['pineappl']/qcdew_data['mg5_amc'] - 1)*1000,
                        step='post', color=colors[0],
                        label='NLO QCD+EW')

    axs[2].fill_between(qcd_data['bin'],
                        (qcd_data['pineappl']/qcd_data['mg5_amc'] - 1)*1000,
                        (qcd_data['pineappl']/qcd_data['mg5_amc'] - 1)*1000,
                        step='post', color=colors[1],
                        label='NLO QCD')
    axs[2].set_ylabel('PineAPPL / MC - 1 [‰]')
    axs[2].legend(loc=args.legend_lower, frameon=False)

    for i in range(2):
        plt.setp(axs[i].get_xticklabels(), visible=False)

    # lim
    if args.ylim is not None:
        axs[2].set_ylim([ np.float64(l) for l in args.ylim])
    if args.xlim is not None:
        axs[0].set_xlim([ np.float64(l) for l in args.xlim])

    # scales
    axs[0].set_yscale(args.yscale)
    axs[0].set_xscale(args.xscale)

    # labels
    axs[0].set_title(args.title)
    axs[0].set_ylabel(args.ylabel)
    axs[2].set_xlabel(args.xlabel)

    print(f'Saving plot in {args.output}')
    plt.savefig(f'{args.output}', bbox_inches='tight')


def main():
    # checking existance
    if not os.path.isdir(args.qcd):
        raise RuntimeError(f'Folder {args.qcd} does not exists.')
    if not os.path.isdir(args.qcdew):
        raise RuntimeError(f'Folder {args.qcdew} does not exists.')

    # load results log
    qcd_results_log = load_results_log(args.qcd, args.bins, args.nbins)
    qcdew_results_log = load_results_log(args.qcdew, args.bins, args.nbins)

    plot_pineappl(qcd_results_log, qcdew_results_log)


if __name__ == "__main__":
    main()
