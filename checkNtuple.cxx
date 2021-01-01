#include <iterator>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <dirent.h>

#include <sstream>
#include <fstream>

using namespace std;

static bool readConfigFile(const char * cfgfilepath, const string & key, string & value)
{   
    fstream cfgFile;
    cfgFile.open(cfgfilepath);
    if( ! cfgFile.is_open())
    {   
        cout<<"can not open cfg file!"<<endl;
        return false;
    }
    char tmp[1000];
    while(!cfgFile.eof())
    {   
        cfgFile.getline(tmp,1000);
        string line(tmp);
        size_t pos = line.find(':');
        if(pos==string::npos) continue;
        string tmpKey = line.substr(0,pos);
        if(key==tmpKey)
        {   
            value = line.substr(pos+1);
        }
    }
    return false;
}

int fileNameFilter(const struct dirent *cur) {
    std::string str(cur->d_name);
    if (str.find("..") != std::string::npos) {
        return 0;
    }
    return 1;
}

std::vector<std::string> getDirBinsSortedPath(std::string dirPath) {
    struct dirent **namelist;
    std::vector<std::string> ret;
    int n = scandir(dirPath.c_str(), &namelist, fileNameFilter, alphasort);
    if (n < 0) {
        return ret;
    }
    for (int i = 0; i < n; ++i) {
        std::string filePath(namelist[i]->d_name);
        ret.push_back(filePath);
        free(namelist[i]);
    };
    free(namelist);
    return ret;
}


void checkNtuple()
{
//  TFile *f_hist = new TFile("hist_myy.root", "recreate");

  int nBins = 11;
  string config = "config2Tau";

  map<TString,TString> samples_id;
  //samples_id["vbf"] = "346214";
  //samples_id["ZH"] = "345319";
  //samples_id["ggh"] = "343981";
  //samples_id["yy"] = "364352";
  samples_id["data"] = "data";
  samples_id["yy2L"] = "450698";

  samples_id["Sherpa2_diphoton_myy_90_175"                 ]="364352"; 
  samples_id["MGPy8_ttgammagamma_allhad"                   ]="345869"; 
  samples_id["MGPy8_ttgammagamma_noallhad"                 ]="345868"; 
  samples_id["PowhegPy8EG_NNPDF30_VBFH125"                 ]="346214"; 
  samples_id["PowhegPy8_NNLOPS_ggH125"                     ]="343981"; 
  samples_id["PowhegPy8_WmH125J"                           ]="345317"; 
  samples_id["PowhegPy8_WpH125J"                           ]="345318"; 
  samples_id["PowhegPy8_ZH125J"                            ]="345319"; 
  samples_id["PowhegPy8_bbH125"                            ]="345315"; 
  samples_id["PowhegPy8_ggZH125"                           ]="345061"; 
  samples_id["PowhegPy8_ttH125_fixweight"                  ]="346525"; 
  samples_id["Sherpa_224_eegammagamma_LO_pty_17_myy_80"    ]="364862"; 
  samples_id["Sherpa_224_enugammagamma_LO_pty_17_myy_80"   ]="364874"; 
  samples_id["Sherpa_224_mumugammagamma_LO_pty_17_myy_80"  ]="364865"; 
  samples_id["Sherpa_224_munugammagamma_LO_pty_17_myy_80"  ]="364877"; 
  samples_id["Sherpa_224_nunugammagamma_LO_pty_17_myy_80"  ]="364871"; 
  samples_id["Sherpa_224_taunugammagamma_LO_pty_17_myy_80" ]="364880"; 
  samples_id["Sherpa_224_tautaugammagamma_LO_pty_17_myy_80"]="364868"; 
  samples_id["aMCnloPy8_tHjb125_4fl_shw_fix"               ]="346188"; 
  samples_id["aMCnloPy8_tWH125"                            ]="346486";

  TString dirpath = "output/";
  std::string path_str = dirpath.Data();
  std::vector<std::string> sub_dirs = getDirBinsSortedPath(path_str);

  std::vector<std::string> files(0);

  for(auto d : sub_dirs){
    if(d==".") continue;
    if(d.find("fullrun2") == std::string::npos) continue; //cout<<"d: "<<path_str+d<<endl;
    std::vector<std::string> fs = getDirBinsSortedPath(path_str+d);
    for(auto f : fs){
      if(f==".") continue; //cout<<"f: "<<path_str+d+"/"+f<<endl;
      files.push_back(path_str+d+"/"+f);
      
    }
  }

  cout<<"\n"<<endl;

  string nomCut = "";
  string srCut = "";
  string crCut = "";
  string passCut = "";
  string failCut = "";
  string blindCut = "";

  readConfigFile(config.data(), "nomSel", nomCut);
  readConfigFile(config.data(), "srSel", srCut);
  readConfigFile(config.data(), "crSel", crCut);
  readConfigFile(config.data(), "passID", passCut);
  readConfigFile(config.data(), "failID", failCut);
  readConfigFile(config.data(), "blindSel", blindCut);

  map<TString, TH1F*> srpassHists;
  map<TString, TH1F*> srfailHists;
  map<TString, TH1F*> crpassHists;
  map<TString, TH1F*> crfailHists;

  for (auto it = samples_id.begin(); it != samples_id.end(); it++){
    TString sample = it->first;
    TString sample_id = it->second;
    cout<<"processing "<<sample<<".."<<endl;

    TChain ch("output", "output");

    for(auto f : files){
      TString filepath = f.data();
      if(f.find(it->second) == std::string::npos) continue;
      ch.Add(filepath);
    }

    //ROOT::RDataFrame df("output", filepath.Data(), {"m_yy"});
    ROOT::RDataFrame df(ch, {"m_yy"});

    TH1F h_sr_pass("h_sr_pass", "", nBins, 105, 160);

    auto df_sr_pass = df.Filter(nomCut).Filter(srCut).Filter(passCut);
    if(sample == "data") df_sr_pass = df_sr_pass.Filter(blindCut);
    df_sr_pass.Foreach([&h_sr_pass] (double m_yy, double w) { h_sr_pass.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
    srpassHists[sample] = (TH1F*) h_sr_pass.Clone(sample+"_SR_passID");

//    cout<<"count: "<<h.Integral()<<endl;
  }

  TH1F *crfailData = (TH1F*) srpassHists["data"]->Clone("data");
  cout<<"data : "<<crfailData->Integral()<<endl;

  THStack *crfailMC = new THStack("hs_CR_failID", "");
  gStyle->SetPalette(kOcean);
  for(auto hist : srpassHists){
    TString h_name = hist.first;
    if(h_name == "data") continue;
    TH1F *h_tmp = (TH1F*) hist.second->Clone(h_name);
    cout<<h_name<<" : "<<h_tmp->Integral()<<endl;
    crfailMC->Add(h_tmp);
  }
  //crfailMC->Draw("pfc");

//  f_hist->cd();
}
