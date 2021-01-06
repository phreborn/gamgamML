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


void slim_data()
{
//  TFile *f_hist = new TFile("hist_myy.root", "recreate");
//  TH1F h("h", "", 1, 105, 160);

  map<TString,TString> samples_id;
  samples_id["data"] = "data";

  map<TString,double> lumi;
  lumi["data1516"] = 36207.66;
  lumi["data17"] = 44307.4;
  lumi["data18"] = 58450.1;

  TString dirpath = "/eos/user/h/huirun/gamgamML/";
  std::string path_str = dirpath.Data();
  std::vector<std::string> sub_dirs = getDirBinsSortedPath(path_str);

  std::vector<std::string> files(0);

  for(auto d : sub_dirs){
    if(d==".") continue;
    if(d.find("data") == std::string::npos) continue; cout<<"d: "<<path_str+d<<endl;
    std::vector<std::string> fs = getDirBinsSortedPath(path_str+d);
    for(auto f : fs){
      if(f==".") continue; cout<<"f: "<<path_str+d+"/"+f<<endl;
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

      int nFiles = 0;

      TChain ch("output", "output");

      for(auto f : files){
        TString filepath = f.data();
        if(f.find(it->second) != std::string::npos && f.find(camp->first) != std::string::npos){
          nFiles++;
          ch.Add(filepath);
        }
      }

      cout<<"number of files : "<<nFiles<<endl;

      //ROOT::RDataFrame df("output", filepath.Data(), {"m_yy"});
      ROOT::RDataFrame df(ch, {"m_yy"});
      auto df_save = df.Filter("cutflow>14 && n_bjet==0 && n_lep+n_tau>=1")
      //auto df_save = df.Filter("cutflow>14 && n_bjet==0")
                       .Define("addup_sumWeight", [] () { return -1.; })
                       .Define("wt", [] () { return 1.; });
      df_save.Snapshot("output", ("output/"+mcCamp+"/"+sample_id+".root").Data());
//      df_save.Foreach([&h] (double m_yy, double w) { h.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
    }
  }

//  f_hist->cd();
//  h.Write();
}
