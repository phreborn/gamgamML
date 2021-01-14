#include "fakeFactor.h"

void subTauFF_VR(){
  gStyle->SetErrorX(0.5);

  //string cfSuffix = "2taus";
  //string cfSuffix = "2taus_1binSB";
  //string cfSuffix = "2taus_LCutSR";
  //string cfSuffix = "2taus_CRtau0Fail";
  string cfSuffix = "2taus_invertGam1";
  string config = "config_";
  config.append(cfSuffix);
  //int nBins = 20;
//  int nBins = 1;
  int nBins = 55;

  bool isSR = false;

  map<TString,TString> samples_id;
  samples_id["data"] = "data";
  samples_id["yy2L"] = "450698";
  samples_id["Sherpa2_diphoton"                            ]="364352";
  samples_id["MGPy8_ttgamgam_allhad"                       ]="345869";
  samples_id["MGPy8_ttgamgam_noallhad"                     ]="345868";
  samples_id["PowhegPy8EG_NNPDF30_VBF"                     ]="346214";
  samples_id["PowhegPy8_NNLOPS_ggH"                        ]="343981";
  samples_id["PowhegPy8_WmH"                               ]="345317";
  samples_id["PowhegPy8_WpH"                               ]="345318";
  samples_id["PowhegPy8_ZH"                                ]="345319";
  samples_id["PowhegPy8_bbH"                               ]="345315";
  samples_id["PowhegPy8_ggZH"                              ]="345061";
  samples_id["PowhegPy8_ttH_fixweight"                     ]="346525";
  samples_id["Sherpa_eegamgam"                             ]="364862";
  samples_id["Sherpa_enugamgam"                            ]="364874";
  samples_id["Sherpa_mumugamgam"                           ]="364865";
  samples_id["Sherpa_munugamgam"                           ]="364877";
  samples_id["Sherpa_nunugamgam"                           ]="364871";
  samples_id["Sherpa_taunugamgam"                          ]="364880";
  samples_id["Sherpa_tautaugamgam"                         ]="364868";
  samples_id["aMCnloPy8_tHjb_4fl_shw_fix"                  ]="346188";
  samples_id["aMCnloPy8_tWH"                               ]="346486";

  //colors["Sherpa2_diphoton"            ] = (EColor) (kRed);
  //colors["MGPy8_ttgamgam_allhad"       ] = (EColor) (kOrange+14);
  //colors["MGPy8_ttgamgam_noallhad"     ] = (EColor) (kOrange+16);
  //colors["PowhegPy8EG_NNPDF30_VBF"     ] = (EColor) (kAzure+2);
  //colors["PowhegPy8_NNLOPS_ggH"        ] = (EColor) (kAzure);
  //colors["PowhegPy8_WmH"               ] = (EColor) (kAzure+4);
  //colors["PowhegPy8_WpH"               ] = (EColor) (kAzure+6);
  //colors["PowhegPy8_ZH"                ] = (EColor) (kAzure+8);
  //colors["PowhegPy8_bbH"               ] = (EColor) (kAzure+10);
  //colors["PowhegPy8_ggZH"              ] = (EColor) (kAzure+11);
  //colors["PowhegPy8_ttH_fixweight"     ] = (EColor) (kAzure+12);
  //colors["Sherpa_eegamgam"             ] = (EColor) (kOrange);
  //colors["Sherpa_enugamgam"            ] = (EColor) (kOrange+2);
  //colors["Sherpa_mumugamgam"           ] = (EColor) (kOrange+4);
  //colors["Sherpa_munugamgam"           ] = (EColor) (kOrange+6);
  //colors["Sherpa_nunugamgam"           ] = (EColor) (kOrange+8);
  //colors["Sherpa_taunugamgam"          ] = (EColor) (kOrange+10);
  //colors["Sherpa_tautaugamgam"         ] = (EColor) (kOrange+12);
  //colors["aMCnloPy8_tHjb_4fl_shw_fix"  ] = (EColor) (kAzure+14);
  //colors["aMCnloPy8_tWH"               ] = (EColor) (kAzure+16);

  colors["Sherpa2_diphoton"            ] = "#CC3333";
  colors["MGPy8_ttgamgam_allhad"       ] = "#";
  colors["MGPy8_ttgamgam_noallhad"     ] = "#";
  colors["PowhegPy8EG_NNPDF30_VBF"     ] = "#";
  colors["PowhegPy8_NNLOPS_ggH"        ] = "#";
  colors["PowhegPy8_WmH"               ] = "#";
  colors["PowhegPy8_WpH"               ] = "#";
  colors["PowhegPy8_ZH"                ] = "#CC3366";
  colors["PowhegPy8_bbH"               ] = "#";
  colors["PowhegPy8_ggZH"              ] = "#663366";
  colors["PowhegPy8_ttH_fixweight"     ] = "#";
  colors["Sherpa_eegamgam"             ] = "#";
  colors["Sherpa_enugamgam"            ] = "#";
  colors["Sherpa_mumugamgam"           ] = "#";
  colors["Sherpa_munugamgam"           ] = "#";
  colors["Sherpa_nunugamgam"           ] = "#";
  colors["Sherpa_taunugamgam"          ] = "#666666";
  colors["Sherpa_tautaugamgam"         ] = "#CCCCCC";
  colors["aMCnloPy8_tHjb_4fl_shw_fix"  ] = "#";
  colors["aMCnloPy8_tWH"               ] = "#";

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

  string vrCut = "";
  string vrpassCut = "";
  string vrfailCut = "";
  readConfigFile(config.data(), "vrSel", vrCut);
  readConfigFile(config.data(), "srpassID", vrpassCut);
  readConfigFile(config.data(), "srfailID", vrfailCut);
  string vrfailAllCut = Form("%s && %s && %s", nomCut.data(), vrCut.data(), vrfailCut.data()); cout<<endl<<"vr fail cuts: "<<vrfailAllCut<<endl<<endl;
  string vrpassAllCut = Form("%s && %s && %s", nomCut.data(), vrCut.data(), vrpassCut.data()); cout<<endl<<"vr pass cuts: "<<vrpassAllCut<<endl<<endl;

  string mainVar;
  readConfigFile(config.data(), "mainVar", mainVar);

  string obsVar;
  double varMin, varMax;
  getVarAndRange(mainVar, obsVar, varMin, varMax); cout<<"variable to draw: "<<obsVar<<" "<<varMin<<" "<<varMax<<endl;
double binning[3];
binning[0] = nBins;
binning[1] = varMin;
binning[2] = varMax;

  map<TString, TH1F*> srpassHists;
  map<TString, TH1F*> srfailHists;
  map<TString, TH1F*> srfailTimesFFHists;
  map<TString, TH1F*> vrpassHists;
  map<TString, TH1F*> vrfailHists;
  map<TString, TH1F*> vrfailTimesFFHists;

  TFile *f_FF_1p = new TFile(Form("forSR_pass_%s_1p.root", cfSuffix.data()), "read");
  TFile *f_FF_3p = new TFile(Form("forSR_pass_%s_3p.root", cfSuffix.data()), "read");
  TH1F *h_FF_1p = (TH1F*) f_FF_1p->Get("fakeFactor_rebin");
  TH1F *h_FF_3p = (TH1F*) f_FF_3p->Get("fakeFactor_rebin");

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

    if(ch.GetEntries()==0) { cout<<sample<<" 0 entry"<<endl; continue; }

    ROOT::RDataFrame df(ch, {"m_yy"});

    TH1F h_vr_fail("h_vr_fail", "", nBins, varMin, varMax); h_vr_fail.Sumw2();
    TH1F h_vr_pass("h_vr_pass", "", nBins, varMin, varMax); h_vr_pass.Sumw2();

    auto df_vr_fail = df.Filter(vrfailAllCut);
    df_vr_fail.Foreach([&h_vr_fail] (double var, double w) { h_vr_fail.Fill(var/1000, w); }, {obsVar.data(), "wt"}); cout<<sample<<"_VR_failID :"<<h_vr_fail.Integral()<<endl;
    vrfailHists[sample] = (TH1F*) h_vr_fail.Clone(sample+"_VR_failID");

    if(isSR && doBlind && sample=="data"){
      auto df_vr_pass = df.Filter(vrpassAllCut).Filter(blindCut);
      df_vr_pass.Foreach([&h_vr_pass] (double var, double w) { h_vr_pass.Fill(var/1000, w); }, {obsVar.data(), "wt"});
      vrpassHists[sample] = (TH1F*) h_vr_pass.Clone(sample+"_VR_passID");
    }else{
      auto df_vr_pass = df.Filter(vrpassAllCut);
      df_vr_pass.Foreach([&h_vr_pass] (double var, double w) { h_vr_pass.Fill(var/1000, w); }, {obsVar.data(), "wt"});
      vrpassHists[sample] = (TH1F*) h_vr_pass.Clone(sample+"_VR_passID");
    }

    TH1F h_vr_fail_x_FF_nom("h_vr_fail_x_FF_nom", "", nBins, varMin, varMax);
    h_vr_fail_x_FF_nom.Sumw2();
    TH1F h_vr_fail_x_FF_upFF("h_vr_fail_x_FF_upFF", "", nBins, varMin, varMax);

    if(sample!="yy2L"&&sample!="Sherpa2_diphoton"){
      auto df_vr_fail_x_FF = df.Filter(vrfailAllCut);

      df_vr_fail_x_FF.Foreach([&h_FF_1p, &h_FF_3p, &h_vr_fail_x_FF_nom](double var, double tau0_pt, double tau1_pt, int tau1_np, double w){
        //double ff0 = h_FF->GetBinContent(h_FF->FindBin(tau0_pt/1000));
        double ff1 = 0.;
        if(tau1_np == 1) ff1 = h_FF_1p->GetBinContent(h_FF_1p->FindBin(tau1_pt/1000));
        if(tau1_np == 3) ff1 = h_FF_3p->GetBinContent(h_FF_3p->FindBin(tau1_pt/1000));
        //h_vr_fail_x_FF_nom.Fill(var/1000, w*ff0*ff1);
        //h_vr_fail_x_FF_nom.Fill(var/1000, w*ff0); // leading tau FF
        h_vr_fail_x_FF_nom.Fill(var/1000, w*ff1); // subleading tau FF
      }, {obsVar.data(), "tau0_pt", "tau1_pt", "tau1_ntracks", "wt"});

      df_vr_fail_x_FF.Foreach([&h_FF_1p, &h_FF_3p, &h_vr_fail_x_FF_upFF](double var, double tau0_pt, double tau1_pt, int tau1_np, double w){
        //double ff0 = h_FF->GetBinError(h_FF->FindBin(tau0_pt/1000))+h_FF->GetBinContent(h_FF->FindBin(tau0_pt/1000));
        double ff1 = 0.;
        if(tau1_np == 1) ff1 = h_FF_1p->GetBinError(h_FF_1p->FindBin(tau1_pt/1000))+h_FF_1p->GetBinContent(h_FF_1p->FindBin(tau1_pt/1000));
        if(tau1_np == 3) ff1 = h_FF_3p->GetBinError(h_FF_3p->FindBin(tau1_pt/1000))+h_FF_3p->GetBinContent(h_FF_3p->FindBin(tau1_pt/1000));
        //h_vr_fail_x_FF_upFF.Fill(var/1000, w*ff0*ff1);
        //h_vr_fail_x_FF_upFF.Fill(var/1000, w*ff0);
        h_vr_fail_x_FF_upFF.Fill(var/1000, w*ff1);
      }, {obsVar.data(), "tau0_pt", "tau1_pt", "tau1_ntracks", "wt"});

      for(int i = 1; i <= nBins; i++){
        double sysFF = h_vr_fail_x_FF_upFF.GetBinContent(i)-h_vr_fail_x_FF_nom.GetBinContent(i);
        double stat = h_vr_fail_x_FF_nom.GetBinError(i);
        double err = std::sqrt(std::pow(sysFF, 2) + std::pow(stat, 2));
        h_vr_fail_x_FF_nom.SetBinError(i, err); //cout<<"bin "<<i<<" sysFF stat and total uncer: "<<sysFF<<" "<<stat<<" "<<err<<endl;
      } cout<<sample<<"_VR_failID_x_FF : "<<h_vr_fail_x_FF_nom.Integral()<<endl;
      vrfailTimesFFHists[sample] = (TH1F*)h_vr_fail_x_FF_nom.Clone(sample+"_VR_failID_x_FF");
    }
  }

  double vrpassData = vrpassHists["data"]->Integral(); cout<<endl<<"data in VR pass "<<vrpassData<<endl;
  for(auto h : vrpassHists){
    TString hname = h.first;
    if(hname=="data") continue;
    TH1F* htmp = h.second; cout<<hname<<" "<<htmp->Integral()<<endl;
    if(hname=="Sherpa2_diphoton"||hname=="yy2L") continue;
    vrpassData -= htmp->Integral();
  } cout<<"data in VR pass(MC subtracted) "<<vrpassData<<endl;

  double vrfailData = vrfailHists["data"]->Integral(); cout<<endl<<"data in VR fail "<<vrfailData<<endl;
  for(auto h : vrfailHists){
    TString hname = h.first;
    if(hname=="data") continue;
    TH1F* htmp = h.second; cout<<hname<<" "<<htmp->Integral()<<endl;
    if(hname=="Sherpa2_diphoton"||hname=="yy2L") continue;
    vrfailData -= htmp->Integral();
  } cout<<"data in VR fail(MC subtracted) "<<vrfailData<<endl;

  /****** FF*N_data - FF*N_mc ******/
  TH1F *h_VR_fail_x_FF = (TH1F*) vrfailTimesFFHists["data"]->Clone("VR_failID_x_FF"); cout<<endl<<endl<<"==> VR failID x FF:"<<h_VR_fail_x_FF->Integral()<<endl<<endl;
  h_VR_fail_x_FF->Sumw2();
  for(auto hist : vrfailTimesFFHists){
    if(hist.first == "data") continue;
    if(hist.first == "yy2L") continue;
    if(hist.first == "Sherpa2_diphoton") continue;
    h_VR_fail_x_FF->Add(hist.second, -1); cout<<hist.first<<" :"<<hist.second->Integral()<<endl;
  } //h_VR_fail_x_FF->Draw("e");
  cout<<endl<<"==> VR failID x FF (MC subtracted):"<<h_VR_fail_x_FF->Integral()<<endl<<endl;

  /****** ignore and merge bkgs ******/
  std::vector<TString> ignoreList;

  ignoreList.push_back("Sherpa2_diphoton");
  vrpassHists["yyjj_fake"] = (TH1F*)h_VR_fail_x_FF->Clone("yyjj_fake_VR_passID");
  //colors["yyjj_fake"] = (EColor) (kRed);
  colors["yyjj_fake"] = "#CC3333";

//  TH1F h_minorBkgs("minor_bkgs", "", nBins, varMin, varMax); h_minorBkgs.Sumw2();
//  for(int i = 1; i<=h_minorBkgs.GetNbinsX(); i++) h_minorBkgs.SetBinContent(i, 0.);
//  double sumBkgYield = 0.;
//  for(auto h : vrpassHists) { if(h.first=="data" || h.first=="Sherpa2_diphoton") continue; sumBkgYield += h.second->Integral(); }
//  for(auto h : vrpassHists){
//    TString h_name = h.first;
//    if(h_name=="yy2L") continue; // needed!!
//    TH1F *h_tmp = (TH1F*) h.second->Clone("hist_tmp_"+h_name);
//    if(h_tmp->Integral() > 0.05*sumBkgYield) continue;
//    ignoreList.push_back(h_name);
//    h_minorBkgs.Add(h_tmp);
//  }
//  vrpassHists["others"] = (TH1F*)h_minorBkgs.Clone("others_VR_passID");
//  //colors["others"] = (EColor) (kOrange);
//  colors["others"] = "#FF6600";

double x_rebin[4] = {105, 120, 130, 160};
int length = sizeof(x_rebin)/sizeof(x_rebin[0]);

std::map<TString, TH1F*> vrpassHists_clone; // b) rebin
cloneHistMap(vrpassHists, vrpassHists_clone);
ignoreAndMerge(vrpassHists_clone, ignoreList, "VR_passID", colors, binning, length, x_rebin);

  /****** draw hist stack plot ******/
//  stackHist(vrpassHists, obsVar, "VR", cfSuffix, false, ignoreList);
stackHist(vrpassHists_clone, obsVar, "VR", cfSuffix, false, ignoreList);


  /****** save histograms ******/
  TString outputDir = "WSInputs/";
  for(auto h : vrpassHists){
    TString hname = h.first;
    if(hname == "others" || hname == "Sherpa2_diphoton") continue;

    TH1F *hist = h.second;
    TH1F *htmp = (TH1F*)hist->Clone(obsVar.data());

    TFile *f;
    f = new TFile(outputDir+hname+".root", "recreate");

    f->cd();
    htmp->Write();

    delete f;
  }
}
