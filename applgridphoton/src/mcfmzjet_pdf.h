// emacs: this is -*- c++ -*-
//
//   mcfmzjet_pdf.h        
//
//   pdf transform functions                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: mcfmz_pdf.h, v1.0   Mon Dec 10 01:36:04 GMT 2007 sutt $


#ifndef __MCFMZJET_PDF_H
#define __MCFMZJET_PDF_H

#include <cmath>

#include "appl_grid/appl_pdf.h" 

#include "TFile.h"
#include "TVectorT.h"
#include "TMatrixT.h"

//
// MCFM Z-jet production
//
class mcfmzjet_pdf : public appl::appl_pdf { 

public:
	mcfmzjet_pdf() : appl_pdf("mcfm-zjet") { m_Nproc = 33; } 
	
	~mcfmzjet_pdf() { } 
	
	virtual void evaluate(const double* fA, const double* fB, double* H);
	
};


// actual funtion to evaluate the pdf combinations 

inline  void mcfmzjet_pdf::evaluate(const double* fA, const double* fB, double* H) {
	//  const int nQuark = 6;
	const int iQuark = 5; 
	
	// offset psd ptrs so can use [-6..6] indexing rather than [nQuark+..] indexing
	fA += 6;
	fB += 6;
	
	double GA=fA[0];
	double GB=fB[0];
	
	double UpA=0; double UpB=0; double DnA=0; double DnB=0;
	double UpbarA=0; double UpbarB=0; double DnbarA=0; double DnbarB=0;
	
	for (int i = 1; i <= iQuark; i++) {
		if ((i % 2) == 0) {
			UpA += fA[i];
			UpB += fB[i];
		}else{
			DnA += fA[i];
			DnB += fB[i];
		}
	}
	
	for(int i = -iQuark; i < 0; i++) {
		if (((int)(std::abs((double)i)) % 2) == 0) {
			UpbarA += fA[i];
			UpbarB += fB[i];
		} else {
			DnbarA += fA[i];
			DnbarB += fB[i];
		}
	}
	
	// zero H first
	for(int i = 0; i<m_Nproc;++i) H[i]=0;
	
	static int  _choice[13] = { 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0 } ;
	static int*  choice     = _choice+6;
	
	for(int i = -iQuark; i <= iQuark; i++) {
		if (i == 0) continue;
		//    int Choice = ( i>0 ? i%2 : abs(i%2)+2 );
		int Choice = choice[i];
		H[Choice] += fA[i] * fB[-i];
	}
	
	H[4] = GA * UpB;
	H[5] = GA * UpbarB;
	H[6] = GA * DnB;
	H[7] = GA * DnbarB;
	H[8] =    UpA *GB;
	H[9] = UpbarA *GB;
	H[10]=    DnA *GB;
	H[11]= DnbarA *GB;
	
	H[12] = GA*GB;
	
	
	for(int i = -iQuark; i <= iQuark; i++) {
		if(i==0) continue;
		for(int j = -iQuark; j <= iQuark; j++) {
			if(j==0 || i==j || i==-j) continue;
			H[13+choice[i]+4*choice[j]] += fA[i] * fB[j];
		}
	}
	
	for(int i = -iQuark; i <= iQuark; i++) {
		if (i == 0) continue;
		//    int Choice = ( i>0 ? i%2 : abs(i%2)+2 );
		int Choice = choice[i];
		H[29+Choice] += fA[i] * fB[i];
	}
	
	
	return;
}


extern "C" void fmcfmzjet_pdf__(const double* fA, const double* fB, double* H);

#endif  // __MCFMZJET_PDF_H

