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

//map<TString, EColor> colors;
map<TString, TString> colors;

void stackHist(map<TString, TH1F*> &crfailHists, string varName, TString rg, string cfSuffix, bool logy, std::vector<TString> igList = {""}){
  cout<<endl<<endl<<"drawing stack hist.."<<endl<<endl;

  gStyle->SetErrorX(0.5);

  TCanvas *c = new TCanvas("c", "canvas", 800, 600);

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

  double y_min = yieldMin/10.0;
  double yScale = 1.8*(logy? 100.0 : 1.0);

  Bkg->GetXaxis()->SetTitle(varName.data());
  Bkg->GetYaxis()->SetRangeUser(y_min, y_max*yScale);
  Data->GetYaxis()->SetRangeUser(y_min, y_max*yScale);
  Sig->GetYaxis()->SetRangeUser(y_min, y_max*yScale);

  Bkg->SetMaximum(y_max*yScale);

  ATLASLabel(0.19,0.88,"Internal");
  myText(0.19, 0.81, 1, "#sqrt{s}= 13 TeV, 139 fb^{-1}");
  myText(0.19, 0.76, 1, "DiHiggs, #it{#gamma#gamma}+#it{#tau_{h}#tau_{h}}");

  if(logy) c->SetLogy();

  c->Update();
  TString pngName = varName.data();
  pngName += "_"+rg+"_";
  pngName += cfSuffix.data();
  c->SaveAs(pngName+".png");
}
