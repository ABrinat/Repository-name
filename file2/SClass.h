#ifndef SClass_h
#define SClass_h
#include <iostream>
#include "TObject.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"

//stdlib include(s)
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


using namespace std;


class SClass : public TObject {
   
   public:
	SClass();
	virtual ~SClass();
	void Fill_hist(); 
	void Draw_hist();     
	static SClass* getInstance();

      
   private:
      static SClass* gMBT; 

ClassDef(SClass, 1)
};
#endif
