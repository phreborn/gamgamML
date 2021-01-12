#include "fakeFactor.h"

void getFakeFactor()
{
  gStyle->SetErrorX(0.5);
  gStyle->SetPalette(kOcean);

  //string config = "config_2taus";
  string config = "config_2taus_CRtau0Fail";
  //string config = "configLepTau";
  double maxFFVar = 120000;
  int nBins = 20;
  bool deriveFF = true;

  colors["Sherpa2_diphoton"            ] = "#CC3333";
  colors["MGPy8_ttgamgam_allhad"       ] = "#";
  colors["MGPy8_ttgamgam_noallhad"     ] = "#";
  colors["PowhegPy8EG_NNPDF30_VBF"     ] = "#CCCC99";
  colors["PowhegPy8_NNLOPS_ggH"        ] = "#CC3399";
  colors["PowhegPy8_WmH"               ] = "#";
  colors["PowhegPy8_WpH"               ] = "#";
  colors["PowhegPy8_ZH"                ] = "#CC3366";
  colors["PowhegPy8_bbH"               ] = "#";
  colors["PowhegPy8_ggZH"              ] = "#663366";
  colors["PowhegPy8_ttH_fixweight"     ] = "#";
  colors["Sherpa_eegamgam"             ] = "#CCCC99";
  colors["Sherpa_enugamgam"            ] = "#FFFF66";
  colors["Sherpa_mumugamgam"           ] = "#";
  colors["Sherpa_munugamgam"           ] = "#99CC00";
  colors["Sherpa_nunugamgam"           ] = "#";
  //colors["Sherpa_taunugamgam"          ] = "#666666";
  colors["Sherpa_taunugamgam"          ] = "#009966";
  colors["Sherpa_tautaugamgam"         ] = "#CCCCCC";
  colors["aMCnloPy8_tHjb_4fl_shw_fix"  ] = "#";
  colors["aMCnloPy8_tWH"               ] = "#";

  map<TString,TString> samples_id;
  //samples_id["vbf"] = "346214";
  //samples_id["ZH"] = "345319";
  //samples_id["ggh"] = "343981";
  //samples_id["yy"] = "364352";
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
  string crCut = "";
  string passCut = "";
  string failCut = "";
  string crpassCut = "";
  string crfailCut = "";
  string srpassCut = "";
  string srfailCut = "";
  string blindCut = "";

  readConfigFile(config.data(), "nomSel", nomCut);
  readConfigFile(config.data(), "srSel", srCut);
  readConfigFile(config.data(), "crSel", crCut);
  readConfigFile(config.data(), "passID", passCut);
  readConfigFile(config.data(), "failID", failCut);
  readConfigFile(config.data(), "crpassID", crpassCut);
  readConfigFile(config.data(), "crfailID", crfailCut);
  readConfigFile(config.data(), "srpassID", srpassCut);
  readConfigFile(config.data(), "srfailID", srfailCut);
  readConfigFile(config.data(), "blindSel", blindCut);

  string mainVar;
  readConfigFile(config.data(), "FFDependVar", mainVar);

  string obsVar;
  double varMin, varMax;
  getVarAndRange(mainVar, obsVar, varMin, varMax); cout<<"variable to draw: "<<obsVar<<" "<<varMin<<" "<<varMax<<endl;
  double binning[3];
  binning[0] = nBins;
  binning[1] = varMin;
  binning[2] = varMax;

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

    //TH1F h_cr_fail("h_cr_fail", "", nBins, 105, 160);
    //TH1F h_cr_pass("h_cr_pass", "", nBins, 105, 160);
    TH1F h_cr_fail("h_cr_fail", "", nBins, 20, maxFFVar/1000);
    TH1F h_cr_pass("h_cr_pass", "", nBins, 20, maxFFVar/1000);

    TH1F h_sr_fail("h_sr_fail", "", nBins, 20, maxFFVar/1000);
    TH1F h_sr_pass("h_sr_pass", "", nBins, 20, maxFFVar/1000);

    auto df_cr_fail = df.Filter(nomCut).Filter(crCut).Filter(crfailCut);
    //auto df_cr_fail = df.Filter(nomCut).Filter(crCut).Filter(passCut);
    //auto df_cr_fail = df.Filter(nomCut).Filter(srCut).Filter(failCut);
    //auto df_cr_fail = df.Filter(nomCut).Filter(srCut).Filter(passCut);
    //df_cr_fail.Foreach([&h_cr_fail] (double m_yy, double w) { h_cr_fail.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
    df_cr_fail.Foreach([&h_cr_fail] (double tau0_pt, double w) { h_cr_fail.Fill(tau0_pt/1000, w); }, {"tau0_pt", "wt"});
    crfailHists[sample] = (TH1F*) h_cr_fail.Clone(sample+"_CR_failID");

    auto df_cr_pass = df.Filter(nomCut).Filter(crCut).Filter(crpassCut);
    //df_cr_pass.Foreach([&h_cr_pass] (double m_yy, double w) { h_cr_pass.Fill(m_yy/1000, w); }, {"m_yy", "wt"});
    df_cr_pass.Foreach([&h_cr_pass] (double tau0_pt, double w) { h_cr_pass.Fill(tau0_pt/1000, w); }, {"tau0_pt", "wt"});
    crpassHists[sample] = (TH1F*) h_cr_pass.Clone(sample+"_CR_passID");

    auto df_sr_fail = df.Filter(nomCut).Filter(srCut).Filter(crfailCut); // using crfailCut for MC comparison between SR and VR
    df_sr_fail.Foreach([&h_sr_fail] (double tau0_pt, double w) { h_sr_fail.Fill(tau0_pt/1000, w); }, {"tau0_pt", "wt"});
    srfailHists[sample] = (TH1F*) h_sr_fail.Clone(sample+"_SR_failID");

    if(doBlind && sample=="data"){
      auto df_sr_pass = df.Filter(nomCut).Filter(srCut).Filter(crpassCut).Filter(blindCut);
      df_sr_pass.Foreach([&h_sr_pass] (double tau0_pt, double w) { h_sr_pass.Fill(tau0_pt/1000, w); }, {"tau0_pt", "wt"});
      srpassHists[sample] = (TH1F*) h_sr_pass.Clone(sample+"_SR_passID");
    }else{
      auto df_sr_pass = df.Filter(nomCut).Filter(srCut).Filter(crpassCut);
      df_sr_pass.Foreach([&h_sr_pass] (double tau0_pt, double w) { h_sr_pass.Fill(tau0_pt/1000, w); }, {"tau0_pt", "wt"});
      srpassHists[sample] = (TH1F*) h_sr_pass.Clone(sample+"_SR_passID");
    }

//    cout<<"count: "<<h.Integral()<<endl;
  }

  double x_rebin[5] = {20, 30, 40, 60, maxFFVar/1000};
  int length = sizeof(x_rebin)/sizeof(x_rebin[0]);

  /****** ignore and merge bkgs ******/
  if(!deriveFF)
  {
    std::vector<TString> ignoreList;

    // CR failID
    ignoreList.push_back("Sherpa2_diphoton");

    //TH1F h_minorBkgs("minor_bkgs", "", nBins, varMin, varMax); h_minorBkgs.Sumw2(); // a)
    //double sumBkgYield = 0.;
    //for(int i = 1; i<=h_minorBkgs.GetNbinsX(); i++) h_minorBkgs.SetBinContent(i, 0.);
    //for(auto h : crfailHists) { if(h.first=="data"||h.first=="Sherpa2_diphoton") continue; sumBkgYield += h.second->Integral(); } // data yy+jets included
    //for(auto h : crfailHists){
    //  TString h_name = h.first;
    //  if(h_name=="yy2L") continue; // needed!!
    //  TH1F *h_tmp = (TH1F*) h.second->Clone("hist_tmp_"+h_name);
    //  double h_int = h_tmp->Integral(); cout<<h_name<<"/sumBkgYield: "<<h_int<<" "<<h_int/sumBkgYield<<endl;
    //  if(h_tmp->Integral() > 0.05*sumBkgYield) continue;
    //  ignoreList.push_back(h_name);
    //  h_minorBkgs.Add(h_tmp);
    //}
    //crfailHists["others"] = (TH1F*)h_minorBkgs.Clone("others_CR_failID");
    //colors["others"] = "#FF6600";

    //stackHist(crfailHists, "tau0_pt", "CRfail", "2taus", true, ignoreList);

    //std::map<TString, TH1F*> crfailHists_clone; // b) rebin
    //cloneHistMap(crfailHists, crfailHists_clone);
    //ignoreAndMerge(crfailHists_clone, ignoreList, "CR_failID", colors, binning, length, x_rebin);
    //stackHist(crfailHists_clone, "tau0_pt", "CRfail", "2taus", true, ignoreList);


    // CR passID
    ignoreList.clear();
    ignoreList.push_back("Sherpa2_diphoton");

    std::map<TString, TH1F*> crpassHists_clone;
    cloneHistMap(crpassHists, crpassHists_clone);

    //ignoreAndMerge(crpassHists, ignoreList, "CR_passID", colors, binning); // a)
    //stackHist(crpassHists, "tau0_pt", "CRpass", "2taus", true, ignoreList);
    ignoreAndMerge(crpassHists_clone, ignoreList, "CR_passID", colors, binning);
    stackHist(crpassHists_clone, "tau0_pt", "CRpass", "2taus", true, ignoreList);

    //ignoreAndMerge(crpassHists_clone, ignoreList, "CR_passID", colors, binning, length, x_rebin); // b) rebin
    //stackHist(crpassHists_clone, "tau0_pt", "CRpass", "2taus", true, ignoreList);


    //// SR failID
    //ignoreList.clear();
    //ignoreList.push_back("Sherpa2_diphoton");
    //ignoreAndMerge(srfailHists, ignoreList, "SR_failID", colors, binning);

    //stackHist(srfailHists, "tau0_pt", "SRfail", "2taus", true, ignoreList);
  }


  /* ====== Data Driven FF ====== */
  TH1F *h_CR_fail = (TH1F*) crfailHists["data"]->Clone("CR_failID"); cout<<endl<<"data in CR_failID : "<<h_CR_fail->Integral()<<endl;
  //h_CR_fail->Draw();
  for(auto hist : crfailHists){
    if(hist.first == "data") continue;
    if(hist.first == "yy2L") continue;
    if(hist.first == "Sherpa2_diphoton") continue;
    h_CR_fail->Add(hist.second, -1);
    cout<<hist.first<<" : "<<hist.second->Integral()<<endl;
  }
  cout<<"CR failID (MC subtracted): "<<h_CR_fail->Integral()<<endl;

  TH1F *h_CR_pass = (TH1F*) crpassHists["data"]->Clone("CR_passID"); cout<<endl<<"data in CR_passID : "<<h_CR_pass->Integral()<<endl;
  for(auto hist : crpassHists){
    if(hist.first == "data") continue;
    if(hist.first == "yy2L") continue;
    if(hist.first == "Sherpa2_diphoton") continue;
    h_CR_pass->Add(hist.second, -1);
    cout<<hist.first<<" : "<<hist.second->Integral()<<endl;
  }
  cout<<"CR passID (MC subtracted): "<<h_CR_pass->Integral()<<endl;

  h_CR_fail->Sumw2();
  h_CR_pass->Sumw2();

  TH1F *h_FF = (TH1F*) h_CR_pass->Clone("fakeFactor");
  h_FF->Divide(h_CR_fail);

  //cout<<"bin1: "<<h_CR_pass->Integral(1, 2)/h_CR_fail->Integral(1, 2)<<endl;
  //cout<<"bin2: "<<h_CR_pass->Integral(3, 4)/h_CR_fail->Integral(3, 4)<<endl;
  //cout<<"bin3: "<<h_CR_pass->Integral(5, 8)/h_CR_fail->Integral(5, 8)<<endl;
  //cout<<"bin2: "<<h_CR_pass->Integral(9, 20)/h_CR_fail->Integral(9, 20)<<endl;

  //h_CR_fail->Draw();
  //h_CR_pass->Draw("same");

  //h_FF->SetMinimum(0.);
  //h_FF->Draw("hist e");

  //double x_rebin[2] = {20, maxFFVar/1000};
  //int length = sizeof(x_rebin)/sizeof(x_rebin[0]);
  //auto h_CR_fail_1bin = h_CR_fail->Rebin(length-1, "rebin", x_rebin);cout<<h_CR_fail_1bin->GetBinError(1)<<endl;
  //auto h_CR_pass_1bin = h_CR_pass->Rebin(length-1, "rebin", x_rebin);cout<<h_CR_pass_1bin->GetBinError(1)<<endl;
  //h_CR_fail_1bin->SetMinimum(0.);
  //h_CR_fail_1bin->Draw("hist e");
  //h_CR_pass_1bin->Draw("same e");
  //h_CR_pass_1bin->Sumw2(); // needed
  //h_CR_fail_1bin->Sumw2();
  //TH1F *h_FF_1bin = (TH1F*)h_CR_pass_1bin->Clone("FF_1bin");
  //h_FF_1bin->Divide(h_CR_fail_1bin);
  //h_FF_1bin->Draw("e");
  //cout<<h_FF_1bin->GetBinError(1)<<endl;

  auto h_CR_fail_rb = h_CR_fail->Rebin(length-1, "rebin", x_rebin);
  auto h_CR_pass_rb = h_CR_pass->Rebin(length-1, "rebin", x_rebin);
  h_CR_fail_rb->Sumw2();
  h_CR_pass_rb->Sumw2();
  auto h_FF_rb = (TH1F*) h_CR_pass_rb->Clone("fakeFactor_rebin");
  h_FF_rb->Divide(h_CR_fail_rb);
  int binNum = h_FF_rb->GetNbinsX(); cout<<endl<<binNum<<" bins"<<endl;
  for(int i = 1; i <= binNum; i++){
    cout<<i<<" bin pass:"<<h_CR_pass_rb->GetBinContent(i)<<" "<<h_CR_pass_rb->GetBinError(i)<<endl;
    cout<<i<<" bin fail:"<<h_CR_fail_rb->GetBinContent(i)<<" "<<h_CR_fail_rb->GetBinError(i)<<endl;
    cout<<i<<" bin ratio:"<<h_FF_rb->GetBinContent(i)<<" "<<h_FF_rb->GetBinError(i)<<endl;
  }

  h_FF_rb->SetMinimum(0.);
  h_FF_rb->SetMaximum(0.3);
  h_FF_rb->SetMarkerSize(0);
  h_FF_rb->SetLineColor(kBlue);
  h_FF_rb->Draw("e1");

  //TH1F *h_FF_rb_tt = (TH1F*)h_FF_rb->Clone("FF_rb_tautau");
  //h_FF_rb_tt->Sumw2();
  //h_FF_rb_tt->Multiply(h_FF_rb);
  //h_FF_rb_tt->SetMinimum(0.);
  //h_FF_rb_tt->Draw("e");

  /* ====== FF from MC yy comparison between SR and VR ====== */
  TH1F *h_yy_cr_pass = (TH1F*)crpassHists["Sherpa2_diphoton"]->Clone("yy_cr_pass");
  TH1F *h_yy_cr_fail = (TH1F*)crfailHists["Sherpa2_diphoton"]->Clone("yy_cr_fail");
  h_yy_cr_pass->Sumw2();
  h_yy_cr_fail->Sumw2();
  auto h_yy_cr_pass_rb = h_yy_cr_pass->Rebin(length-1, "rebin", x_rebin);
  auto h_yy_cr_fail_rb = h_yy_cr_fail->Rebin(length-1, "rebin", x_rebin);
  TH1F *h_FF_yy_cr_rb = (TH1F*) h_yy_cr_pass_rb->Clone("FF_yy_cr_rb");
  h_FF_yy_cr_rb->Divide(h_yy_cr_fail_rb);
  h_FF_yy_cr_rb->SetMinimum(0.);
  h_FF_yy_cr_rb->SetLineColor(kBlue);
  h_FF_yy_cr_rb->SetMarkerSize(0);

  TH1F *h_yy_sr_pass = (TH1F*)srpassHists["Sherpa2_diphoton"]->Clone("yy_sr_pass");
  TH1F *h_yy_sr_fail = (TH1F*)srfailHists["Sherpa2_diphoton"]->Clone("yy_sr_fail");
  h_yy_sr_pass->Sumw2();
  h_yy_sr_fail->Sumw2();
  auto h_yy_sr_pass_rb = h_yy_sr_pass->Rebin(length-1, "rebin", x_rebin);
  auto h_yy_sr_fail_rb = h_yy_sr_fail->Rebin(length-1, "rebin", x_rebin);
  TH1F *h_FF_yy_sr_rb = (TH1F*) h_yy_sr_pass_rb->Clone("FF_yy_sr_rb");
  h_FF_yy_sr_rb->Divide(h_yy_sr_fail_rb);
  h_FF_yy_sr_rb->SetMinimum(0.);
  h_FF_yy_sr_rb->SetMaximum(0.3);
  h_FF_yy_sr_rb->SetLineColor(kRed);
  h_FF_yy_sr_rb->SetMarkerSize(0);

  //h_FF_yy_sr_rb->Draw("e");
  //h_FF_yy_cr_rb->Draw("same e");


  TH1F *h_SR_fail = (TH1F*) srfailHists["data"]->Clone("SR_failID"); cout<<endl<<"SR failID:"<<h_SR_fail->Integral()<<endl;
  for(auto hist : srfailHists){
    if(hist.first == "data") continue;
    if(hist.first == "yy2L") continue;
    if(hist.first == "Sherpa2_diphoton") continue;
    h_SR_fail->Add(hist.second, -1);
    cout<<hist.first<<" : "<<hist.second->Integral()<<endl;
  } cout<<"SR failID (MC subtracted):"<<h_SR_fail->Integral()<<endl;
  h_SR_fail->Sumw2();
  //h_SR_fail->Draw("e");

  TFile *f_out = new TFile("forSR_pass.root", "recreate");

  f_out->cd();
  h_FF_rb->Write();
  h_FF_yy_sr_rb->Write();
  h_FF_yy_cr_rb->Write();
//  h.Write();
}
