#include "fakeFactor.h"

void SR_pass(){
  gStyle->SetErrorX(0.5);

  string config = "config2Tau";
  int nBins = 20;

  map<TString,TString> samples_id;
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

  colors["Sherpa2_diphoton_myy_90_175"] = (EColor) (kRed);
  colors["MGPy8_ttgammagamma_allhad"] = (EColor) (kOrange+14);
  colors["MGPy8_ttgammagamma_noallhad"] = (EColor) (kOrange+16);
  colors["PowhegPy8EG_NNPDF30_VBFH125"] = (EColor) (kAzure+2);
  colors["PowhegPy8_NNLOPS_ggH125"] = (EColor) (kAzure);
  colors["PowhegPy8_WmH125J"] = (EColor) (kAzure+4);
  colors["PowhegPy8_WpH125J"] = (EColor) (kAzure+6);
  colors["PowhegPy8_ZH125J"] = (EColor) (kAzure+8);
  colors["PowhegPy8_bbH125"] = (EColor) (kAzure+10);
  colors["PowhegPy8_ggZH125"] = (EColor) (kAzure+11);
  colors["PowhegPy8_ttH125_fixweight"] = (EColor) (kAzure+12);
  colors["Sherpa_224_eegammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange);
  colors["Sherpa_224_enugammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange+2);
  colors["Sherpa_224_mumugammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange+4);
  colors["Sherpa_224_munugammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange+6);
  colors["Sherpa_224_nunugammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange+8);
  colors["Sherpa_224_taunugammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange+10);
  colors["Sherpa_224_tautaugammagamma_LO_pty_17_myy_80"] = (EColor) (kOrange+12);
  colors["aMCnloPy8_tHjb125_4fl_shw_fix"] = (EColor) (kAzure+14);
  colors["aMCnloPy8_tWH125"] = (EColor) (kAzure+16);

  string sDir;
  readConfigFile(config.data(), "inputPath", sDir);

  TString dirpath = sDir.data();
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

  bool doBlind = true;
  string blind_tmp = "";
  readConfigFile(config.data(), "doBlind", blind_tmp);
  if(blind_tmp.find("TRUE") != std::string::npos) doBlind = true;
  else doBlind = false;

  string nomCut = "";
  string srCut = "";
  string srpassCut = "";
  string srfailCut = "";
  string blindCut = "";

  readConfigFile(config.data(), "nomSel", nomCut);
  readConfigFile(config.data(), "srSel", srCut);
  readConfigFile(config.data(), "srpassID", srpassCut);
  readConfigFile(config.data(), "srfailID", srfailCut);
  readConfigFile(config.data(), "blindSel", blindCut);

  string srfailAllCut = Form("%s && %s && %s", nomCut.data(), srCut.data(), srfailCut.data()); cout<<endl<<"sr fail cuts: "<<srfailAllCut<<endl<<endl;
  string srpassAllCut = Form("%s && %s && %s", nomCut.data(), srCut.data(), srpassCut.data()); cout<<endl<<"sr pass cuts: "<<srpassAllCut<<endl<<endl;

  map<TString, TH1F*> srpassHists;
  map<TString, TH1F*> srfailHists;
  map<TString, TH1F*> srfailTimesFFHists;

  TFile *f_FF = new TFile("forSR_pass.root", "read");
  TH1F *h_FF = (TH1F*) f_FF->Get("fakeFactor_rebin");

  for (auto it = samples_id.begin(); it != samples_id.end(); it++){
    TString sample = it->first;
    TString sample_id = it->second;
    cout<<endl<<endl<<"processing "<<sample<<".."<<endl;

    TChain ch("output", "output");

    for(auto f : files){
      TString filepath = f.data();
      if(f.find(it->second) == std::string::npos) continue;
      ch.Add(filepath);
    }

    ROOT::RDataFrame df(ch, {"m_yy"});

    TH1F h_sr_fail("h_sr_fail", "", nBins, 105, 160); h_sr_fail.Sumw2();
    TH1F h_sr_pass("h_sr_pass", "", nBins, 105, 160); h_sr_pass.Sumw2();

    auto df_sr_fail = df.Filter(srfailAllCut);
    df_sr_fail.Foreach([&h_sr_fail] (double m_yy, double w) { h_sr_fail.Fill(m_yy/1000, w); }, {"m_yy", "wt"}); cout<<sample<<"_SR_failID :"<<h_sr_fail.Integral()<<endl;
    srfailHists[sample] = (TH1F*) h_sr_fail.Clone(sample+"_SR_failID");

    if(doBlind && sample=="data"){
      auto df_sr_pass = df.Filter(srpassAllCut).Filter(blindCut);
      df_sr_pass.Foreach([&h_sr_pass] (double m_yy, double w) { h_sr_pass.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
      srpassHists[sample] = (TH1F*) h_sr_pass.Clone(sample+"_SR_passID");
    }else{
      auto df_sr_pass = df.Filter(srpassAllCut);
      df_sr_pass.Foreach([&h_sr_pass] (double m_yy, double w) { h_sr_pass.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
      srpassHists[sample] = (TH1F*) h_sr_pass.Clone(sample+"_SR_passID");
    }

    TH1F h_sr_fail_x_FF_nom("h_sr_fail_x_FF_nom", "", nBins, 105, 160);
    h_sr_fail_x_FF_nom.Sumw2();
    TH1F h_sr_fail_x_FF_upFF("h_sr_fail_x_FF_upFF", "", nBins, 105, 160);

    if(sample!="yy2L"&&sample!="Sherpa2_diphoton_myy_90_175"){
      auto df_sr_pass = df.Filter(srfailAllCut);

      df_sr_pass.Foreach([&h_FF, &h_sr_fail_x_FF_nom](double m_yy, double tau0_pt, double tau1_pt, double w){
        double ff0 = h_FF->GetBinContent(h_FF->FindBin(tau0_pt/1000));
        double ff1 = h_FF->GetBinContent(h_FF->FindBin(tau1_pt/1000));
        h_sr_fail_x_FF_nom.Fill(m_yy/1000, w*ff0*ff1);
      },{"m_yy", "tau0_pt", "tau1_pt", "wt"});

      df_sr_pass.Foreach([&h_FF, &h_sr_fail_x_FF_upFF](double m_yy, double tau0_pt, double tau1_pt, double w){
        double ff0 = h_FF->GetBinError(h_FF->FindBin(tau0_pt/1000))+h_FF->GetBinContent(h_FF->FindBin(tau0_pt/1000));
        double ff1 = h_FF->GetBinError(h_FF->FindBin(tau1_pt/1000))+h_FF->GetBinContent(h_FF->FindBin(tau1_pt/1000));
        h_sr_fail_x_FF_upFF.Fill(m_yy/1000, w*ff0*ff1);
      }, {"m_yy", "tau0_pt", "tau1_pt", "wt"});

      for(int i = 1; i <= nBins; i++){
        double sysFF = h_sr_fail_x_FF_upFF.GetBinContent(i)-h_sr_fail_x_FF_nom.GetBinContent(i);
        double stat = h_sr_fail_x_FF_nom.GetBinError(i);
        double err = std::sqrt(std::pow(sysFF, 2) + std::pow(stat, 2));
        h_sr_fail_x_FF_nom.SetBinError(i, err); //cout<<"bin "<<i<<" sysFF stat and total uncer: "<<sysFF<<" "<<stat<<" "<<err<<endl;
      } cout<<sample<<"_SR_failID_x_FF : "<<h_sr_fail_x_FF_nom.Integral()<<endl;
      srfailTimesFFHists[sample] = (TH1F*)h_sr_fail_x_FF_nom.Clone(sample+"_SR_failID_x_FF");
    }
  }

  /****** FF*N_data - FF*N_mc ******/
  TH1F *h_SR_fail_x_FF = (TH1F*) srfailTimesFFHists["data"]->Clone("SR_failID_x_FF"); cout<<endl<<endl<<"==> SR failID x FF:"<<h_SR_fail_x_FF->Integral()<<endl<<endl;
  h_SR_fail_x_FF->Sumw2();
  for(auto hist : srfailTimesFFHists){
    if(hist.first == "data") continue;
    if(hist.first == "yy2L") continue;
    if(hist.first == "Sherpa2_diphoton_myy_90_175") continue;
    h_SR_fail_x_FF->Add(hist.second, -1); cout<<hist.first<<" :"<<hist.second->Integral()<<endl;
  } //h_SR_fail_x_FF->Draw("e");
  cout<<endl<<"==> SR failID x FF (MC subtracted):"<<h_SR_fail_x_FF->Integral()<<endl<<endl;

  /****** ignore and merge bkgs ******/
  std::vector<TString> ignoreList;

  ignoreList.push_back("Sherpa2_diphoton_myy_90_175");
  srpassHists["yyjj_fake"] = (TH1F*)h_SR_fail_x_FF->Clone("yyjj_fake_SR_passID");
  colors["yyjj_fake"] = (EColor) (kRed);

  TH1F h_minorBkgs("minor_bkgs", "", nBins, 105, 160); h_minorBkgs.Sumw2();
  for(int i = 1; i<=h_minorBkgs.GetNbinsX(); i++) h_minorBkgs.SetBinContent(i, 0.);
  for(auto h : srpassHists){
    TString h_name = h.first;
    TH1F *h_tmp = (TH1F*) h.second->Clone("hist_tmp_"+h_name);
    if(h_tmp->Integral() > 0.02*srpassHists["data"]->Integral()) continue;
    ignoreList.push_back(h_name);
    h_minorBkgs.Add(h_tmp);
  }
  srpassHists["others"] = (TH1F*)h_minorBkgs.Clone("others_SR_passID");
  colors["others"] = (EColor) (kOrange);

  /****** draw hist stack plot ******/
  stackHist(srpassHists, "VR", ignoreList);
}
