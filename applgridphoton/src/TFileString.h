// emacs: this is -*- c++ -*-
//
//   TFileString.h        
//
//    root TObject std::string std::vector class for writing std::string std::vectors
//    to root files               
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TFileString.h, v0.0   Sat Mar 15 19:49:16 GMT 2008 sutt


#ifndef __TFILESTRING_H
#define __TFILESTRING_H

#include <iostream>

#include <string>

#include <vector>

#include "TObjString.h"
#include "TObject.h"


class TFileString : public TObjString { 

public:
  
  TFileString(const std::string& name="") : TObjString(name.c_str()) { } 

  TFileString(const std::string& name, const std::string& tag) : 
    TObjString(name.c_str())
  { mstring.push_back(tag.c_str()); } 
 
  std::vector<std::string>&       tags()       { return mstring; }
  const std::vector<std::string>& tags() const { return mstring; }

  // get the name
  std::string  name() const { return GetName(); } 

  // get a value 
  std::string& operator[](int i)       { return mstring[i]; }
  std::string  operator[](int i) const { return mstring[i]; }
  
  // get the size
  size_t size()           const { return mstring.size(); } 

  // add an element
  void push_back(const std::string& s) { mstring.push_back(s); }
  void add(const std::string& s)       { mstring.push_back(s); }

private:
  
  std::vector<std::string> mstring;

  ClassDef(TFileString, 1)

}; 


std::ostream& operator<<(std::ostream& s, const TFileString& fs);



#endif  // __TFILESTRING_H 










