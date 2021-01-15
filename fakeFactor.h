#include "/scratchfs/bes/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.h"
#include "/scratchfs/bes/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasStyle.h"
#include "/scratchfs/bes/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.h"

#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "/scratchfs/bes/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.C"
#include "/scratchfs/bes/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.C"
#endif

#include <iterator>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <dirent.h>

#include <sstream>
#include <fstream>

#include <string.h>

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

void getVarAndRange(std::string cfstring, string &name, double &min, double &max){
  TString ts_tmp;
  std::vector<TString> ts_vec;
  char *p;
  char *buff = (char*)cfstring.data();
  char *sep = (char*)",";
  p = strsep(&buff, sep);
  while(p!=NULL){
    ts_tmp = p;
    ts_tmp.ReplaceAll(" ", "");
    if(ts_tmp!="") ts_vec.push_back(ts_tmp); //cout<<ts_tmp<<endl;
    p = strsep(&buff, sep);
  }
  name = ts_vec.at(0).Data();
  min = std::atof(ts_vec.at(1).Data());
  max = std::atof(ts_vec.at(2).Data());
}

void fillHists(std::map<TString, TH1F*> &histVec, ROOT::RDataFrame &df, std::string cuts, std::string varname, TString sample, TString region, double *bins){
  TH1F hist("h_"+region, "", (int)bins[0], bins[1]/1000, bins[2]/1000);
  hist.Sumw2();
  auto df_cut = df.Filter(cuts);
  df_cut.Foreach([&hist] (double var, double w) { hist.Fill(var/1000, w); }, {varname.data(), "wt"});
  histVec[sample] = (TH1F*) hist.Clone(sample+"_"+region); std::cout<<sample<<" in "<<region<<": "<<histVec[sample]->Integral()<<endl;
}

void cloneHistMap(std::map<TString, TH1F*> &h_ori, std::map<TString, TH1F*> &h_cl){
  for(auto h : h_ori) h_cl[h.first] = (TH1F*) h.second->Clone();
}

//map<TString, EColor> colors;
map<TString, TString> colors;

void ignoreAndMerge(std::map<TString, TH1F*> &hists, std::vector<TString> &ignoreList, TString region, map<TString, TString> &colors, double *binning, int length = 0, double *x_rebin = nullptr){
  double sumBkgYield = 0.;

  TH1F *h_minorBkgs_ori = new TH1F("minor_bkgs", "", binning[0], binning[1], binning[2]); h_minorBkgs_ori->Sumw2();

  TH1F *h_minorBkgs = nullptr;

  if(x_rebin!=nullptr){
    h_minorBkgs = (TH1F*)h_minorBkgs_ori->Rebin(length-1, "rebin", x_rebin);

    for(auto h : hists){
      hists[h.first] = (TH1F*) h.second->Rebin(length-1, "rebin", x_rebin);
    }

  }else{
    h_minorBkgs = h_minorBkgs_ori;
  }

  for(int i = 1; i<=h_minorBkgs->GetNbinsX(); i++) h_minorBkgs->SetBinContent(i, 0.);
  for(auto h : hists) { if(h.first=="data"||h.first=="Sherpa2_diphoton") continue; sumBkgYield += h.second->Integral(); } cout<<endl<<"sumBkgYield(exclude yy+jets): "<<sumBkgYield<<endl<<endl;
  for(auto h : hists){
    TString h_name = h.first;
    if(h_name=="yy2L") continue; // needed!!
    TH1F *h_tmp = (TH1F*) h.second->Clone("hist_tmp_"+h_name);
    double h_int = h_tmp->Integral(); cout<<h_name<<": "<<h_int/sumBkgYield<<endl;
    if(h_tmp->Integral() > 0.05*sumBkgYield) continue;
    ignoreList.push_back(h_name);
    h_minorBkgs->Add(h_tmp);
  }
  hists["others"] = (TH1F*)h_minorBkgs->Clone("others_"+region);
  colors["others"] = "#FF6600";

  delete h_minorBkgs_ori;
}

void stackHist(map<TString, TH1F*> &crfailHists, string varName, TString rg, string cfSuffix, bool logy, std::vector<TString> igList = {""}){
  cout<<endl<<endl<<"drawing stack hist.."<<endl<<endl;

  gStyle->SetErrorX(0.5);

  TCanvas *c = new TCanvas("c", "canvas", 800, 800);

  TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
  pad1->SetBottomMargin(0); // Upper and lower plot are joined
  //pad1->SetGridx();         // Vertical grid
  pad1->Draw();             // Draw the upper pad: pad1
  pad1->cd();               // pad1 becomes the current pad

  TLegend* lg = new TLegend(0.59, 0.6, 0.93, 0.9);

  TH1F *Data = (TH1F*) crfailHists["data"]->Clone("data");
  TH1F *Sig = (TH1F*) crfailHists["yy2L"]->Clone("yy2L"); cout<<"signal yields: "<<Sig->Integral()<<endl;

  int sig_scale = 1;

  lg->AddEntry(Data, "Data", "lp");

  double yieldMin = std::pow(10, 15);
  double sumYields = 0.;

  THStack *Bkg = new THStack("hs_CR_failID", "");
  //gStyle->SetPalette(kOcean);
  cout<<endl<<"BKGs' yields:"<<endl;
  for(auto hist : crfailHists){
    TString h_name = hist.first;
    if(h_name == "data") continue;
    if(h_name == "yy2L") continue;
    if(std::find(igList.begin(), igList.end(), h_name) != igList.end()) continue;
    auto h_tmp = (TH1F*) hist.second->Clone(h_name); cout<<h_name<<" "<<h_tmp->Integral()<<endl;
    //h_tmp->SetFillColor(colors[h_name]);
    h_tmp->SetFillColor(TColor::GetColor(colors[h_name].Data()));
    h_tmp->SetLineWidth(0);
    Bkg->Add(h_tmp);
    TString name_tmp = h_name.ReplaceAll("_", " ");
    lg->AddEntry(h_tmp, name_tmp, "f");
    if(h_tmp->Integral()<yieldMin) yieldMin = h_tmp->Integral();
    sumYields += h_tmp->Integral();
  } cout<<endl<<"total bkg yields: "<<sumYields<<endl; cout<<endl<<"data yields: "<<Data->Integral()<<endl<<endl;

  if(sumYields > Sig->Integral()) sig_scale = (int)std::round(0.5*sumYields/Sig->Integral());
  Sig->Scale(sig_scale);

  lg->AddEntry(Sig, Form("signal #times %i",sig_scale), "l");

  double y_max = 0.;
  double data_max = Data->GetMaximum(); cout<<"data y axis maximum: "<<data_max<<endl;
  double bkg_max = Bkg->GetMaximum(); cout<<"bkg y axis maximum: "<<bkg_max<<endl;
  y_max = data_max;
  if(bkg_max>y_max) y_max = bkg_max; //cout<<y_max<<endl;

  TH1F *Bkg_err = (TH1F*)Bkg->GetStack()->Last()->Clone("Bkg_error");
  Bkg_err->SetMarkerSize(0);
  Bkg_err->SetFillStyle(3254);
  Bkg_err->SetFillColor(kBlack);

  //Bkg->Draw("hist pfc");
  Bkg->Draw("hist");
  Bkg_err->Draw("e2 same");
  Data->SetMarkerSize(1);
  Data->Draw("same e");
  Sig->SetLineColor(kGreen);
  Sig->SetLineWidth(1);
  Sig->Draw("same hist");

  lg->SetBorderSize(0);
  lg->Draw("same");

  double y_min = logy? yieldMin/10.0 : 0.; cout<<"minimum bkg yield: "<<yieldMin<<endl;
  double yScale = 1.8*(logy? 100.0 : 1.0);

  Bkg->GetXaxis()->SetTitle(varName.data());
  Bkg->GetYaxis()->SetRangeUser(y_min, y_max*yScale);
  Data->GetYaxis()->SetRangeUser(y_min, y_max*yScale);
  Sig->GetYaxis()->SetRangeUser(y_min, y_max*yScale);

  Bkg->SetMinimum(y_min);
  Bkg->SetMaximum(y_max*yScale);

  ATLASLabel(0.19,0.88,"Internal");
  myText(0.19, 0.81, 1, "#sqrt{s}= 13 TeV, 139 fb^{-1}");
  myText(0.19, 0.76, 1, "DiHiggs, #it{#gamma#gamma}+#it{#tau_{h}#tau_{h}}");

  if(logy) c->SetLogy();

  Bkg->GetYaxis()->SetLabelSize(.04);
  TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
  axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  axis->SetLabelSize(15);
  axis->Draw();

  c->cd();          // Go back to the main canvas before defining pad2
  TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0.2);
  //pad2->SetGridx(); // vertical grid
  pad2->Draw();
  pad2->cd();

  TH1F *Bkg_noErr = (TH1F*)Bkg->GetStack()->Last()->Clone("Bkg_noError");
  for(int i=1; i<=Bkg_noErr->GetNbinsX(); i++){
    Bkg_noErr->SetBinError(i, 0.);
  }

  TH1F *rData_err = (TH1F*)Data->Clone("ratio_data_err");
  rData_err->SetLineColor(kBlack);
  rData_err->SetMinimum(0.2);
  rData_err->SetMaximum(1.8);
  rData_err->Sumw2();
  rData_err->SetStats(0);
  rData_err->Divide(Bkg_noErr);
  rData_err->SetMarkerSize(1);
  rData_err->SetMarkerStyle(20);
  rData_err->Draw("ep");

  TH1F *rBkg_err = (TH1F*)Bkg_err->Clone("ratio_Bkg_err");
  rBkg_err->SetLineColor(kBlack);
  rBkg_err->Divide(Bkg_noErr);
  rBkg_err->SetMarkerStyle(20);
  rBkg_err->SetMarkerSize(0);
  rBkg_err->SetFillStyle(3001);
  rBkg_err->SetFillColor(kBlack);
  rBkg_err->Draw("same e2");

  Bkg->GetYaxis()->SetTitleSize(20);
  Bkg->GetYaxis()->SetTitleFont(43);
  Bkg->GetYaxis()->SetTitleOffset(1.55);

  rData_err->SetTitle(""); // Remove the ratio title

  rData_err->GetYaxis()->SetTitle("Data/MC");
  //rData_err->GetYaxis()->SetNdivisions(505);
  rData_err->GetYaxis()->SetTitleSize(20);
  rData_err->GetYaxis()->SetTitleFont(43);
  rData_err->GetYaxis()->SetTitleOffset(1.55);
  rData_err->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  rData_err->GetYaxis()->SetLabelSize(15);

  rData_err->GetXaxis()->SetTitle(Form("%s/GeV", varName.data()));
  rData_err->GetXaxis()->SetTitleSize(20);
  rData_err->GetXaxis()->SetTitleFont(43);
  rData_err->GetXaxis()->SetTitleOffset(4.);
  rData_err->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  rData_err->GetXaxis()->SetLabelSize(15);

  c->Update();
  TString pngName = varName.data();
  pngName += "_"+rg+"_";
  pngName += cfSuffix.data();
  c->SaveAs(pngName+".png");

  //delete c;
}
