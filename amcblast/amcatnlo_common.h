// is -*- c++ -*-
//
//   @file    amcatnlo_common.h
//            aMC@NLO - APPLgrid commom block interface
//
//            c++ implementation of from appl_common.inc
//            from amcatnlo: Template/NLO/SubProcess/appl_common.inc
//
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)
//
//   $Id: amcatnlo.h, v0.0   Thu  4 Jul 2013 09:51:20 CEST sutt $

#ifndef  AMCATNLO_COMMON_H
#define  AMCATNLO_COMMON_H

// Maximum number of allowed external particles
const int __maxparticles__ = 20;
// Maximum number of (pairwise) suprocesses
const int __max_nproc__ = 121;

/*
  Common blocks for NLO mode of aMC@NLO
*/

// Information defined at the generation (configuration) step, that does
// not vary event by event
typedef struct {
  int bpower; // Born level alphas order
} __amcatnlo_common_fixed__;

// Map of the PDF combinations from aMC@NLO - structure for each
// "subprocess" i, has some number nproc[i] pairs of parton
// combinations. To be used together with the info in appl_flavmap.
typedef struct {
  int lumimap[__max_nproc__][__max_nproc__][2]; // (paired) subprocesses per combination
  int nproc[__max_nproc__];                     // number of separate (pairwise) subprocesses for this combination
  int nlumi;                                    // overall number of combinations ( 0 < nlumi <= __mxpdflumi__ )
} __amcatnlo_common_lumi__;

// Event weights, kinematics, etc. that are different event by event
typedef struct {
  double  x1[4],x2[4];
  double  muF2[4],muR2[4],muQES2[4];
  double  W0[4],WR[4],WF[4],WB[4];
  int     flavmap[4];
} __amcatnlo_common_weights__;

// Parameters of the grids.
// These parameters can optionally be singularly specified by the user,
// but if no specification is given, the code will use the default values.
typedef struct {
  double Q2min,Q2max;
  double xmin,xmax;
  int    nQ2,Q2order;
  int    nx,xorder;
} __amcatnlo_common_grid__;

// Parameters of the histograms
typedef struct {
  double www_histo,norm_histo;
  double obs_histo;
  double obs_min,obs_max;
  double obs_bins[101];
  int    obs_nbins;
  int    itype_histo;
  int    obs_num;
} __amcatnlo_common_histokin__;

// Event weight and cross section
typedef struct {
  double event_weight,vegaswgt;
  double xsec12,xsec11,xsec20;
} __amcatnlo_common_reco__;

// Declare the external common blocks as external
extern "C" __amcatnlo_common_fixed__    appl_common_fixed_;
extern "C" __amcatnlo_common_lumi__     appl_common_lumi_;
extern "C" __amcatnlo_common_weights__  appl_common_weights_;
extern "C" __amcatnlo_common_grid__     appl_common_grid_;
extern "C" __amcatnlo_common_histokin__ appl_common_histokin_;
extern "C" __amcatnlo_common_reco__     appl_common_reco_;

#endif  // AMCATNLO_COMMON_H
