/*
  This code is part of the aMCfast library but it is compiled separately and the 
  executable "amcfast-psgrid" created. Such an executable takes the aMCfast event 
  file "aMCfast_obs.event" produced by MG5_aMC@NLO as an input and produces as many
  interpolation grids as observables defined in the event file.

  Author: Valerio Bertone
 */

// Standard libs
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>
#include <glob.h>
#include <iomanip>
#include <locale>
#include <math.h>
#include <cmath>
#include "stdio.h"
#include "stdlib.h"

// LHAPDF
#include "LHAPDF/LHAPDF.h"

// aMCfast libs
#include "amcatnlo_common.h"
#include "amcfast_interface.h"

using namespace std;

// Common structures
__amcatnlo_common_fixed__    appl_common_fixed_;
__amcatnlo_common_lumi__     appl_common_lumi_;
__amcatnlo_common_weights__  appl_common_weights_;
__amcatnlo_common_grid__     appl_common_grid_;
__amcatnlo_common_histokin__ appl_common_histokin_;
__amcatnlo_common_reco__     appl_common_reco_;

// Hook functions
void (*appl_initptr)();
void (*appl_fillptr)();
void (*appl_fillrefptr)();
void (*appl_termptr)();
void (*appl_fillrefoutptr)();
void (*appl_recoptr)();

// Set up the interpolation grids using the information in the event file
void setup_grids(string eventfile);

// Fill the interpolation grids
void fill_grids(string eventfile);

// Dump interpolation grids to file
void write_grids();

// Reconstruct weights
bool enable_reconstruction = false;
string PDFset;
double reconstruction(double weight, int t, int g, int id1, int id2, double x1, double x2, double muF, double muR);

// Number of observables
int nobs = 0;

// Number of events
int nevents = 0;

int main(int argc, char* argv[]) {
  // Check that the input is correct
  if(argc < 3 || strcmp(argv[1],"--help") == 0) {
    cout << " " << endl;
    cout << "Invalid Parameters:" << endl;
    cout << "Syntax: ./amcfast-psgrid <aMCfast event file> <PDF set>" << endl;
    cout << " " << endl;
    exit(-10);
  }

  string eventfile = argv[1];
  PDFset = argv[2];

  // If the name of the PDF set in not "none" enable the weight reconstruction
  if(PDFset != "none") enable_reconstruction = true;

  cout << "\naMCfast INFO: Processing event file " << eventfile << " ...\n" << endl;

  // Set up grids
  setup_grids(eventfile);

  // Fill grids
  fill_grids(eventfile);

  // Write grids
  write_grids();
}

//
// Set up the interpolation grids using the information in the event file
//
void setup_grids(string eventfile) {
  cout << "aMCfast INFO: Setting up grids ..." << endl;

  // Set the grid parameters (if equal to -1 ==> use the default values)
  /*
  appl_common_grid_.nQ2     = -1;
  appl_common_grid_.Q2min   = -1;
  appl_common_grid_.Q2max   = -1;
  appl_common_grid_.Q2order = -1;
  appl_common_grid_.nx      = -1;
  appl_common_grid_.xmin    = -1;
  appl_common_grid_.xmax    = -1;
  appl_common_grid_.xorder  = -1;
  */
  appl_common_grid_.nQ2     = -1;
  appl_common_grid_.Q2min   =  2;
  appl_common_grid_.Q2max   =  2000000;
  appl_common_grid_.Q2order = -1;
  appl_common_grid_.nx      = -1;
  appl_common_grid_.xmin    = -1;
  appl_common_grid_.xmax    = -1;
  appl_common_grid_.xorder  = -1;

  // Luminosity vector parameters
  vector<int> id_parton1,id_parton2;

  // g power at LO
  int g_lo = 100;

  // Open the event file
  ifstream evt(eventfile.c_str());
  string line;

  // Loop over the event file
  while(getline(evt,line)) {
    // Update number of events
    if(line == "<event>") nevents++;
    // Find where the relevant information starts
    if(line == "<applgrid>") {
      while(getline(evt,line)) {
	if(line == "</applgrid>") break;
	else {
	  istringstream linestream(line);
	  int idum;
	  double ddum;
	  string sdum;
	  int g;
	  int id1,id2;
	  linestream >> ddum >> sdum >> idum >> g >> id1 >> id2;

	  // Find the power of g at LO
	  if(g < g_lo) g_lo = g;

	  if(id_parton1.size() == 0 && id_parton2.size() == 0) {
	    id_parton1.push_back(id1);
	    id_parton2.push_back(id2);
	  }

	  // Accumulate luminosities
	  bool present = false;
	  for(unsigned i=0; i<id_parton1.size(); i++) {
	    if(id_parton1[i] == id1 && id_parton2[i] == id2)  {
	      present = true;
	      break;
	    }
	  }
	  if(!present) {
	    id_parton1.push_back(id1);
	    id_parton2.push_back(id2);
	  }
	}
      }
    }
  }

  // Check that the power of g at Born level is an even number
  if(g_lo%2 != 0) {
    cout << "aMCfast ERROR: Odd leading power of g" << endl;
    cout << "g_lo = " << g_lo << endl;
    exit(-10);
  }

  // Check that the size of the ID parton vectors is the same
  if(id_parton1.size() != id_parton2.size()) {
    cout << "aMCfast ERROR: Parton luminosity mismatch" << endl;
    exit(-10);
  }

  // Born level alphas power
  appl_common_fixed_.bpower = g_lo / 2;

  // Set up the APPLgrid PDF luminosities
  appl_common_lumi_.nlumi = id_parton1.size();

  // Loop over parton luminosities
  for(int ilumi=0; ilumi<appl_common_lumi_.nlumi; ilumi++) {
    // When considering the parton shower we cannot (yet) gather together
    // different parton luminosities into groups having the same weight,
    // thus we must set nproc = 1
    appl_common_lumi_.nproc[ilumi] = 1;

    appl_common_lumi_.lumimap[ilumi][0][0] = id_parton1[ilumi];
    appl_common_lumi_.lumimap[ilumi][0][1] = id_parton2[ilumi];
  }

  // Rewind the event file
  evt.clear();
  evt.seekg(evt.beg);

  // Read binning from the event file
  while(getline(evt,line)) {
    if(line == "<histoinfo>") {
      while(getline(evt,line)) {
	if(line == "</histoinfo>") break;
	else {
	  nobs++;
	  istringstream linestream(line);
	  unsigned nbins;
	  vector<double> obsbins;
	  double var;
	  linestream >> nbins;
	  while(linestream >> var) obsbins.push_back(var);

	  // Check that the number of bins found is equal to that expected
	  if(nbins != obsbins.size() - 1) {
	    cout << "aMCfast ERROR: Number of bins mismatch" << endl;
	    cout << "expected = " << nbins << endl;
	    cout << "found    = " << obsbins.size() - 1 << endl;
	    exit(-10);
	  }

	  // Define binning
	  appl_common_histokin_.obs_nbins = nbins;
	  appl_common_histokin_.obs_min   = obsbins[0];
	  appl_common_histokin_.obs_max   = obsbins[nbins];

	  // Construct array of the bin edges
	  for(unsigned i=0; i<=nbins; i++) appl_common_histokin_.obs_bins[i] = obsbins[i];

	  // Initialize grids
	  amcfast::init();
	}
      }
    }
  }
  evt.close();

  // Report number of events
  cout << "\naMCfast INFO: " << nobs << " observables found" << endl;

  // Report number of events
  cout << "\naMCfast INFO: " << nevents << " events found" << endl;

  return;
}

//
// Fill the interpolation grids
//
void fill_grids(string eventfile) {
  //cout << "\naMCfast INFO: Filling grids ...\n" << endl;

  // Initialize PDF file in the reconstruction check is enabled
  if(enable_reconstruction) LHAPDF::initPDFSet(PDFset.c_str());

  // Set elements to zero (only k=1 will be used)
  for(int k=0; k<4; k++) {
    appl_common_weights_.x1[k]      = 0;
    appl_common_weights_.x2[k]      = 0;
    appl_common_weights_.muF2[k]    = 0;
    appl_common_weights_.muR2[k]    = 0;
    appl_common_weights_.muQES2[k]  = 0;
    appl_common_weights_.W0[k]      = 0;
    appl_common_weights_.WR[k]      = 0;
    appl_common_weights_.WF[k]      = 0;
    appl_common_weights_.WB[k]      = 0;
    appl_common_weights_.flavmap[k] = 0;
  }

  // Open event file
  ifstream evt(eventfile.c_str());
  string line;

  // Internal event counter
  int ievent = 0;

  cout << endl;
  while(getline(evt,line)) {
    if(line == "<event>") {
      ievent++;
      double perc = (double) 100 * ievent / nevents;
      cout << "aMCfast INFO: Filling grids ... " << setprecision(3) << setw(4) << perc << " %" << endl;
      if(ievent != nevents) cout << "\x1b[A";

      // Read values of the observables
      while(getline(evt,line)) {
	if(line == "</event>") break;
	else {
	  // Put observables in a vector (first entry = global weight)
	  vector<double> observable;
	  for(int iobs=0; iobs<nobs+1; iobs++) {
	    istringstream linestream(line);
	    double obs;
	    linestream >> obs;

	    observable.push_back(obs);
	    getline(evt,line);
	  }

	  // Recontructed weight (used only if "enable_reconstruction" is true)
	  double reco_weight = 0;

	  // Read kinematics and weights
	  if(line == "<applgrid>") {
	    while(getline(evt,line)) {
	      if(line == "</applgrid>") break;
	      else {
		istringstream linestream(line);

		int t;
		int g;
		int id1,id2;
		double weight;
		double x1,x2;
		double muF,muR;
		string sdum;

		linestream >> weight >> sdum >> t >> g >> id1 >> id2 >> x1 >> x2 >> muF >> muR;
		//cout << t << "\t" << weight << "\t" << x1 << "\t" << x2 << "\t" << muF << "\t" << muR << endl;

		// Recontruct global weight
		if(enable_reconstruction) reco_weight += reconstruction(weight,t,g,id1,id2,x1,x2,muF,muR);

		// Find luminosity to assign the the weight
		int ilumi = -1;
		for(int i=0; i<appl_common_lumi_.nlumi; i++) {
		  if(appl_common_lumi_.lumimap[i][0][0] == id1 &&
		     appl_common_lumi_.lumimap[i][0][1] == id2) {
		    ilumi = i;
		    break;
		  }
		}

		// Check that a luminosity has been found
		if(ilumi == -1) {
		  cout << "aMCfast ERROR: Luminosity not found" << endl;
		  exit(-10);
		}

		// For convenience, we set k = 1.
		// This is due to the structure of the filling function that was originally
		// conceived to work in the FO mode.
		int k = 1;
		// Set kinematics
		appl_common_weights_.x1[k]      = x1;
		appl_common_weights_.x2[k]      = x2;
		appl_common_weights_.muF2[k]    = pow(muF,2);
		appl_common_weights_.flavmap[k] = ilumi + 1;

		// Set weights
		// t = 1 -> weight corresponding to alpha_s^(born+1)
		if(t == 1) {
		  appl_common_histokin_.itype_histo = 2;
		  appl_common_weights_.W0[k] = weight;
		  appl_common_weights_.WR[k] = 0;
		  appl_common_weights_.WF[k] = 0;
		  appl_common_weights_.WB[k] = 0;
		}
		// t = 2 -> weight corresponding to alpha_s^(born)
		else if(t == 2) {
		  appl_common_histokin_.itype_histo = 3;
		  appl_common_weights_.W0[k] = 0;
		  appl_common_weights_.WR[k] = 0;
		  appl_common_weights_.WF[k] = 0;
		  appl_common_weights_.WB[k] = weight;
		}
		// t = 3 -> weight corresponding to alpha_s^(born+1) * log (muF/QES)
		else if(t == 3) {
		  appl_common_histokin_.itype_histo = 2;
		  appl_common_weights_.W0[k] = 0;
		  appl_common_weights_.WR[k] = 0;
		  appl_common_weights_.WF[k] = weight;
		  appl_common_weights_.WB[k] = 0;
		}
		// t = 4 -> weight corresponding to alpha_s^(born+1) * log (muR/QES)
		else if(t == 4) {
		  appl_common_histokin_.itype_histo = 2;
		  appl_common_weights_.W0[k] = 0;
		  appl_common_weights_.WR[k] = weight;
		  appl_common_weights_.WF[k] = 0;
		  appl_common_weights_.WB[k] = 0;
		}
		else {
		  cout << "aMCfast ERROR: Invalid weight ID" << endl;
		  cout << "t = " << t << endl; 
		  exit(-10);
		}

		// Now ready to fill the grids
		for(int iobs=0; iobs<nobs; iobs++) {
		  appl_common_histokin_.obs_histo = observable[iobs+1];
		  appl_common_histokin_.obs_num   = iobs + 1;
		  amcfast::fill();
		}
	      }
	    }
	  }
	  else {
	    cout << "aMCfast ERROR: Unexpected line in the event file" << endl;
	    cout << "Should be <applgrid>, while " << line << " found" << endl; 
	    exit(-10);
	  }

	  //if(enable_reconstruction) {
	  //  if(abs((reco_weight-appl_common_histokin_.www_histo)/reco_weight)>1e-2) {
	  //    cout << "aMCfast WARNING: weight reconstruction of the " << ievent << "-th event failed" << endl;
	  //    cout << "expected weight      = " << appl_common_histokin_.www_histo << endl; 
	  //    cout << "reconstructed weight = " << reco_weight << endl; 
	  //  }
	  //}

	  // Fill reference histogram
	  // (replace the global weight with the reconstructed one if the reconstruction is enabled).
	  appl_common_histokin_.www_histo = observable[0];
	  if(enable_reconstruction) appl_common_histokin_.www_histo = reco_weight;
	  for(int iobs=0; iobs<nobs; iobs++) {
	      appl_common_histokin_.obs_histo = observable[iobs+1];
	      appl_common_histokin_.obs_num   = iobs + 1;
	      amcfast::fill_ref();
	  }
	}
      }
    }
  }

  // Check that the number of events found during the filling stage is equal to that
  // found in the initialization stage.
  if(ievent != nevents) {
    cout << "aMCfast ERROR: Event number mismatch" << endl;
    cout << "expected number = " << nevents << endl; 
    cout << "found number    = " << ievent << endl; 
    exit(-10);
  }

  // Restore precision and field width
  cout << setprecision(16); 
  cout << setw(16);
  return;
}

//
// Dump interpolation grids to file
//
void write_grids() {
  cout << "\naMCfast INFO: Writing grids ...\n" << endl;

  // Normalization factor equal to the inverse of the number
  // of events.
  appl_common_histokin_.norm_histo = (double) 1 / nevents;

  // Normalize grid and reference histogram
  for(int iobs=0; iobs<nobs; iobs++) {
    appl_common_histokin_.obs_num = iobs + 1;
    amcfast::fill_ref_out();
    amcfast::term();
  }
}

//
// Recontruct weights
//
double reconstruction(double weight, int t, int g, int id1, int id2, double x1, double x2, double muF, double muR) {
  // if the weight corresponds to the factorization or renormalization terms,
  // return automatically zero.
  if(t == 3 || t == 4) return 0;

  // convert the gluon ID to the LHAPDF convention
  if(id1 == 21) id1 = 0;
  if(id2 == 21) id2 = 0;

  // Reconstruct the weight
  double conv = 389379660;
  double as   = pow(4 * M_PI * LHAPDF::alphasPDF(muR),g/2);
  double pdf1 = LHAPDF::xfx(x1,muF,id1) / x1;
  double pdf2 = LHAPDF::xfx(x2,muF,id2) / x2;

  double gw = conv * as * weight * pdf1 * pdf2;

  return gw;
}
