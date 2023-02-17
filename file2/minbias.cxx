 /**
* @file The main executable.
*
* @brief 
*
* @return 0 If everything is okay.
*/
//Local include(s)
#include "MinBiasTree.h"
#include "SClass.h"
//ROOT include(s)
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

void usage();
std::vector<std::string>* Get_ROOT_Files(std::string );
bool Is_ROOT_File(std::string );

Int_t m_eventToProc = -1;
Int_t m_nprint = 100;
bool  m_debug = false;

int main(int argc, char** argv) {

  int optchar;
  std::vector<string> root_files;
  std::string TreeName = "MinBiasTree";
  std::vector<string>* newfiles;


  while((optchar = getopt(argc, argv, "f:e:t:n:h")) != -1){
    switch(optchar){
    case 'f':
      newfiles = Get_ROOT_Files(optarg);
      root_files.insert(root_files.end(), newfiles->begin(), newfiles->end());
      delete newfiles; break;
    case 'e': m_eventToProc=atoi(optarg);     break;
    case 't': TreeName=(char*)strdup(optarg);  break;
    case 'n': m_nprint=atoi(optarg);  break;
    case 'h':
    default:
      usage();
      return 0;
    }
  }

  

  if(root_files.size() <= 0){
    cerr << "Must specify some number of files to process!  Aborting." << endl;
    return -1;
  }
  
  //Retrieve the TTree -  chain them all together
  TChain* fchain = new TChain(TreeName.c_str());
  
  printf("adding files:\n");
  for(std::vector<string>::iterator rf_iter = root_files.begin();
                                    rf_iter != root_files.end(); ++rf_iter){
    if(m_debug) printf("%s\n",(*rf_iter).c_str());
    fchain->Add((*rf_iter).c_str());
   }
  printf("\n");


  MinBiasTree  *evt = MinBiasTree::getInstance();
  evt->Init(fchain);

//////////////////////////////////////////////////////////////////////////////////////////

  SClass *F = SClass::getInstance();
 
/////////////////////////////////////////////////////////////////////////////////////////


  Long64_t nentries = evt->GetNEntries(); 
  printf("Entries in  ntuple  %10lli \n", nentries);
  if(m_eventToProc<1 ) m_eventToProc=nentries;

  //
  //Event loop ...
  // 
  for (Long64_t ientry = 0; ientry < m_eventToProc; ientry++) {
    if (evt->LoadTree(ientry) < 0) {
      cerr << "ERROR: minbias_d3pd  can't get entry == " << ientry << endl;
      exit(1);
    }
    Long64_t nb = evt->GetEntry(ientry);
    if(m_debug||(ientry%m_nprint) == 1)
      printf("minbias_d3pd: Analysing run %u,  event  %lli, already analysed %lli, to analyse %i \n",
             evt->RunNumber,evt->EventNumber,  ientry, (int)m_eventToProc );
	   F->Fill_hist();
 
  
  }
}


//-----------------------------------------------------------------------
void usage(){
//-----------------------------------------------------------------------
 
  std::cerr << endl
            << "Usage: minbias  <options>" << endl
            << "\t" << "-f <filename>"  << "\t" << "Run on ROOT file <filename> or list of file" << endl
            << "\t" << "-n <printevnt>" << "\t" << "Print message every n events" << endl
            << "\t" << "-e <numofevnt>" << "\t" << "Number of events to process  " << endl
            << "\t" << "-h            " << "\t" << "Print this message" << endl
            << endl;
 
  return;
}

//********************************************************************************************
//
// Some useful functions for parsing command line input into a list of ROOT files
//********************************************************************************************
//
//-----------------------------------------------------------------------
bool Is_ROOT_File(std::string filename){
//-----------------------------------------------------------------------
  
  if((filename.find("rfio") != std::string::npos) || (filename.find("root:") != std::string::npos)||
     (filename.find("dcap:") != std::string::npos)|| (filename.find("dcache:") != std::string::npos)){

    
  } else {
    

    std::string modified_file_name(filename);
    // replace all ".."'s with "__"'s.
    std::string::size_type found = modified_file_name.find("..");
    while(found != std::string::npos){
      modified_file_name.replace(found,2,"__");
      found = modified_file_name.find("..",found+1,2);
    }
 
    // replace all "/"'s with "_"'s.
    for(unsigned int i=0; i<modified_file_name.length(); i++){
      if(modified_file_name[i] == '/') modified_file_name[i] = '_';
    }

    
    pid_t pid = getpid();
 
    std::ostringstream test_file_name;
    //std::string::size_type lastslash = filename.find_last_of("/",filename.length());
    //test_file_name << "." << filename.substr(lastslash+1,(filename.length()-lastslash)) << "_type";
    test_file_name << "." << modified_file_name << "__type__" << pid;
 
    // std::cout << "dumping file status into " << test_file_name.str() << std::endl;
 
    std::ostringstream command;
    command << "file -L " << filename << " > " << test_file_name.str();
    system(command.str().c_str());
    //
    std::ifstream file_out(test_file_name.str().c_str());
    if(!file_out.is_open()){
      std::cerr << "ERROR: cannot open results of filetype check for file " << filename << "!" << std::endl;
 
      if(filename.find(".root") != std::string::npos && filename.find(",") == std::string::npos){
        std::cerr << "Assuming this is a root file, because it contains .root." << std::endl;
       return true;
      }
      else 
        return false; 
    }
    char charline[1000];
    file_out.getline(charline,1000);
    std::string lin(charline);
    system(("rm " + test_file_name.str()).c_str());

    found = lin.find("ROOT", 0);
    if(lin.find("ROOT", 0) != std::string::npos) {
      //if directory name contains word ROOT 
      if(lin.find("/", found) != std::string::npos) return false;

      return true ;
      
    }  else 
      return false; 
  }
  return true;
}
//-----------------------------------------------------------------------
std::vector<std::string>* Get_ROOT_Files(std::string filename){
//-----------------------------------------------------------------------
  std::vector<std::string>* root_files = new std::vector<std::string>();
  
  if(Is_ROOT_File(filename)) {
     if(filename.find(",") != std::string::npos){
      std::istringstream ss(filename);
      while(ss){
        std::string csv;
        getline(ss, csv, ',');
        if(csv.length()>0)
          if(Is_ROOT_File(csv)) root_files->push_back(csv);
      }
    } else root_files->push_back(filename);
  }
  else if(filename.find(",") != std::string::npos){
      
    std::istringstream ss(filename);
    while(ss){
      std::string csv;
      getline(ss, csv, ',');
      if(csv.length()>0)
        if(Is_ROOT_File(csv)) root_files->push_back(csv);
    }
  }
 
  else{
 
    std::ifstream filelist(filename.c_str());
    if(!filelist.is_open()){
      std::cerr << "ERROR: could not open file list " << filename << std::endl;
      return root_files;
    }
 
    //char charline[1000];
    std::string lin;
    while(filelist.good()){
      getline(filelist,lin);
      //filelist.getline(charline,1000);
      //std::string lin(charline);
      if(lin.length() == 0){}
      else if(lin.find(",")){
        std::istringstream ss(lin);
        while(ss){
          std::string csv;
          getline(ss, csv, ',');
          if(csv.length()>0)
            root_files->push_back(csv);
        }
      }
      else{
        if(lin.find(".root",0) == std::string::npos)
          std::cerr << "WARNING: is " << lin << " really a ROOT file?  Skipping." << std::endl;
        else if(lin.find("merged",0) != std::string::npos)
          std::cerr << "WARNING: " << lin << " is a merged file.  Skipping for now." << std::endl;
        else
          root_files->push_back(lin);
      }
    }
  }
  return root_files;
}
