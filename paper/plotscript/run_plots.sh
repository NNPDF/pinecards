#!/bin/bash

# ATLAS Z HIGH Mass
python pineappl_plot.py \
    --qcd ATLASZHIGHMASS49FB-20200625102151 \
    --qcdew ATLASZHIGHMASS49FB-20200623223328 \
    --nbins 0 12 \
    --bins 116 130 150 170 190 210 230 250 300 400 500 700 1000 1500 \
    --title "ATLAS high-mass Drell-Yan at $\sqrt{s}=7$ TeV" \
    --output "pineappl_ATLASZHIGHMASS49FB.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} M_{\ell \bar{\ell}} $ [pb]" \
    --xlabel "$ M_{\ell \bar{\ell}}$ [GeV]" \
    --yscale "log" \
    --xscale "log" \
    --legend "upper right" \
    --legend-lower "upper left" \
    --xlim 100 1800

# CMS bin1
python pineappl_plot.py \
    --qcd CMSDY2D11-20200625180039 \
    --qcdew CMSDY2D11-20200626080745  \
    --nbins 0 23 \
    --bins 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 \
    --title "CMS 2D Drell-Yan 2011, $ 20 < M_{\ell \bar{\ell}} < 30 $ GeV" \
    --output "pineappl_CMSDY2D11_bin1.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} y_{\ell \bar{\ell}} $ [pb]" \
    --xlabel " $ y_{\ell \bar{\ell}} $ " \
    --legend "lower left" \
    --legend-lower "upper left" \
    --multiply 10

# CMS bin2
python pineappl_plot.py \
    --qcd CMSDY2D11-20200625180039 \
    --qcdew CMSDY2D11-20200626080745  \
    --nbins 24 47 \
    --bins 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 \
    --title "CMS 2D Drell-Yan 2011, $ 30 < M_{\ell \bar{\ell}} < 45 $ GeV" \
    --output "pineappl_CMSDY2D11_bin2.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} y_{\ell \bar{\ell}} $ [pb]" \
    --xlabel " $ y_{\ell \bar{\ell}} $ " \
    --legend "lower left" \
    --legend-lower "upper left" \
    --multiply 10

# CMS bin3
python pineappl_plot.py \
    --qcd CMSDY2D11-20200625180039 \
    --qcdew CMSDY2D11-20200626080745  \
    --nbins 48 71 \
    --bins 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 \
    --title "CMS 2D Drell-Yan 2011, $ 45 < M_{\ell \bar{\ell}} < 60 $ GeV" \
    --output "pineappl_CMSDY2D11_bin3.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} y_{\ell \bar{\ell}} $ [pb]" \
    --xlabel " $ y_{\ell \bar{\ell}} $ " \
    --legend "lower left" \
    --legend-lower "upper left" \
    --multiply 10

# CMS bin4
python pineappl_plot.py \
    --qcd CMSDY2D11-20200625180039 \
    --qcdew CMSDY2D11-20200626080745  \
    --nbins 72 95 \
    --bins 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 \
    --title "CMS 2D Drell-Yan 2011, $ 60 < M_{\ell \bar{\ell}} < 120 $ GeV" \
    --output "pineappl_CMSDY2D11_bin4.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} y_{\ell \bar{\ell}} $ [pb]" \
    --xlabel " $ y_{\ell \bar{\ell}} $ " \
    --legend "lower left" \
    --legend-lower "upper left" \
    --multiply 10

# CMS bin5
python pineappl_plot.py \
    --qcd CMSDY2D11-20200625180039 \
    --qcdew CMSDY2D11-20200626080745  \
    --nbins 96 119 \
    --bins 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 \
    --title "CMS 2D Drell-Yan 2011, $ 120 < M_{\ell \bar{\ell}} < 200 $ GeV" \
    --output "pineappl_CMSDY2D11_bin5.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} y_{\ell \bar{\ell}} $ [pb]" \
    --xlabel " $ y_{\ell \bar{\ell}} $ " \
    --legend "lower left" \
    --legend-lower "upper left" \
    --multiply 10

# CMS bin6
python pineappl_plot.py \
    --qcd CMSDY2D11-20200625180039 \
    --qcdew CMSDY2D11-20200626080745  \
    --nbins 120 131 \
    --bins 0.0 0.2 0.4 0.6 0.8 1.0 1.2 1.4 1.6 1.8 2.0 2.2 2.4 \
    --title "CMS 2D Drell-Yan 2011, $ 200 < M_{\ell \bar{\ell}} < 1500 $ GeV" \
    --output "pineappl_CMSDY2D11_bin6.pdf" \
    --ylabel "$ \mathrm{d} \sigma / \mathrm{d} y_{\ell \bar{\ell}} $ [pb]" \
    --xlabel " $ y_{\ell \bar{\ell}} $ " \
    --legend "lower left" \
    --legend-lower "upper right" \
    --multiply 5