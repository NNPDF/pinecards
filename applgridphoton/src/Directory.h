// emacs: this is -*- c++ -*-
/**************************************************************************
 **
 **   File:         appl_grid/Directory.h  
 **
 **   Description:  class to keep a directory for each object 
 **                 in a root sort of way, but needed to keep 
 **                 the root objects out of the actual code.   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Wed May  4 17:54:25 BST 2005
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __DIRECTORY_H
#define __DIRECTORY_H

#include "TDirectory.h"

// #include "utils.h"

class Directory {

 public:

  Directory() : mDir(NULL), mPop(NULL) { } 
  Directory(std::string n) : mDir(NULL) { 
    mPop = gDirectory;
    //    depunctuate(n);
    mDir = gDirectory->mkdir(n.c_str());
  } 

  void push() { 
    mPop = gDirectory;
    if (mDir) mDir->cd();
  } 

  void pop() { if (mPop) mPop->cd(); }

  void Write() { 
    push();
    mDir->Write();
    pop();
  } 

 private:
  
  TDirectory* mDir;
  TDirectory* mPop;

};



#endif  /* __DIRECTORY_H */










