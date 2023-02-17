// SClass.cxx:

#include "SClass.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"

//stdlib include(s)
#include "MinBiasTree.h"
#include <stddef.h> 
#include <stdio.h>
#include <stdlib.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>


TH1F *hist = new TH1F("hist", "Histogram",7,1,7);
TFile *f = new TFile("hist.root", "RECREATE");

SClass* SClass::gMBT =  0;

SClass* SClass::getInstance()
{
  if (!gMBT) gMBT=new SClass();
  
 return gMBT;
}

//----------------------------------------------
SClass::SClass(){}
//----------------------------------------------

//----------------------------------------------
SClass::~SClass(){}
//----------------------------------------------




ClassImp(SClass);

//----------------------------------------------
void SClass::Fill_hist() {
//----------------------------------------------
  MinBiasTree  *evt = MinBiasTree::getInstance();


  std::cout<<"Execute Fill_hist"<<std::endl;
  hist->Fill(evt->EventNumber ); 

  hist->Write();
 
}



void SClass::Draw_hist()
{
  hist->Print(); 
  f->Close();
}

//f->Close();

