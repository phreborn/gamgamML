#include <iterator>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <dirent.h>

#include <sstream>
#include <fstream>

using namespace std;


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


void slim_yy()
{
//  TFile *f_hist = new TFile("hist_myy.root", "recreate");
//  TH1F h("h", "", 1, 105, 160);

  map<TString,TString> samples_id;
  //samples_id["vbf"] = "346214";
  //samples_id["ZH"] = "345319";
  //samples_id["ggh"] = "343981";
  //samples_id["yy"] = "364352";
  //samples_id["yy2L"] = "450698";

  samples_id["Sherpa2_diphoton_myy_90_175"                 ]="364352"; 
  //samples_id["MGPy8_ttgammagamma_allhad"                   ]="345869"; 
  //samples_id["MGPy8_ttgammagamma_noallhad"                 ]="345868"; 
  //samples_id["PowhegPy8EG_NNPDF30_VBFH125"                 ]="346214"; 
  //samples_id["PowhegPy8_NNLOPS_ggH125"                     ]="343981"; 
  //samples_id["PowhegPy8_WmH125J"                           ]="345317"; 
  //samples_id["PowhegPy8_WpH125J"                           ]="345318"; 
  //samples_id["PowhegPy8_ZH125J"                            ]="345319"; 
  //samples_id["PowhegPy8_bbH125"                            ]="345315"; 
  //samples_id["PowhegPy8_ggZH125"                           ]="345061"; 
  //samples_id["PowhegPy8_ttH125_fixweight"                  ]="346525"; 
  //samples_id["Sherpa_224_eegammagamma_LO_pty_17_myy_80"    ]="364862"; 
  //samples_id["Sherpa_224_enugammagamma_LO_pty_17_myy_80"   ]="364874"; 
  //samples_id["Sherpa_224_mumugammagamma_LO_pty_17_myy_80"  ]="364865"; 
  //samples_id["Sherpa_224_munugammagamma_LO_pty_17_myy_80"  ]="364877"; 
  //samples_id["Sherpa_224_nunugammagamma_LO_pty_17_myy_80"  ]="364871"; 
  //samples_id["Sherpa_224_taunugammagamma_LO_pty_17_myy_80" ]="364880"; 
  //samples_id["Sherpa_224_tautaugammagamma_LO_pty_17_myy_80"]="364868"; 
  //samples_id["aMCnloPy8_tHjb125_4fl_shw_fix"               ]="346188"; 
  //samples_id["aMCnloPy8_tWH125"                            ]="346486";

  map<TString,double> lumi;
  lumi["mc16a"] = 36207.66;
  lumi["mc16d"] = 44307.4;
  lumi["mc16e"] = 58450.1;

  TString dirpath = "/eos/user/h/huirun/gamgamML/";
  std::string path_str = dirpath.Data();
  std::vector<std::string> sub_dirs = getDirBinsSortedPath(path_str);

  std::vector<std::string> files(0);

  for(auto d : sub_dirs){
    if(d==".") continue;
    if(d.find("mc") == std::string::npos) continue; //cout<<"d: "<<path_str+d<<endl;
    std::vector<std::string> fs = getDirBinsSortedPath(path_str+d);
    for(auto f : fs){
      if(f==".") continue; //cout<<"f: "<<path_str+d+"/"+f<<endl;
      files.push_back(path_str+d+"/"+f);
      
    }
  }

  cout<<"\n"<<endl;

  for (auto it = samples_id.begin(); it != samples_id.end(); it++){
    TString sample = it->first;
    TString sample_id = it->second;
    cout<<"processing "<<sample<<".."<<endl;
    double f_sig = sample == "yy2L"? 0.00000962/0.0275 : 1.;

    for(auto camp = lumi.begin(); camp != lumi.end(); camp++){
      cout<<camp->first<<endl;

      TString mcCamp = camp->first;
      double luminosity = camp->second;

      double sumOfWeights = 0.;
      int nFiles = 0;

      TChain ch("output", "output");

      for(auto f : files){
        TString filepath = f.data();
        if(f.find(it->second) != std::string::npos && f.find(camp->first) != std::string::npos){
          nFiles++;
          ch.Add(filepath);
          TFile *f_w = new TFile(filepath,"read");
          TTree *tree_w = (TTree*) f_w->Get("output");
          double total_weights;
          tree_w->SetBranchAddress("total_weights", &total_weights);
          tree_w->GetEntry(0);
          sumOfWeights += total_weights;
        }
      }

      cout<<"number of files : "<<nFiles<<", "<<"sum of weights: "<<sumOfWeights<<endl;

      //ROOT::RDataFrame df("output", filepath.Data(), {"m_yy"});
      ROOT::RDataFrame df(ch, {"m_yy"});
      auto df_save = df.Filter("cutflow>14 && n_bjet==0 && n_lep+n_tau>=1")
      //auto df_save = df.Filter("cutflow>14 && n_bjet==0")
        .Define("addup_sumWeight", [&sumOfWeights] () { return sumOfWeights; })
        .Define("wt", [&luminosity, &sumOfWeights, &f_sig] (double weight, double xsec_kF_ge) { return luminosity*weight*xsec_kF_ge*f_sig/sumOfWeights; }, {"weight", "xsec_kF_ge"});
      df_save.Snapshot("output", ("output/"+mcCamp+"/"+sample_id+"_"+sample+".root").Data());
//      df_save.Foreach([&h] (double m_yy, double w) { h.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
    }
  }

//  f_hist->cd();
//  h.Write();
}
