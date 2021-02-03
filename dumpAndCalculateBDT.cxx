#include "/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/fakeFactor/fakeFactor.h"

using namespace TMVA::Experimental;

void dumpAndCalculateBDT(){

  string config = "configTMVA";

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

  string cutSR;
  readConfigFile(config.data(), "noIDSel", cutSR);

  RReader model("dataset/weights/TMVAClassification_BDT.weights.xml");
  auto variables = model.GetVariableNames();
  const std::size_t nVars = variables.size();

  for (auto it = samples_id.begin(); it != samples_id.end(); it++){
    TString sample = it->first;
    TString sample_id = it->second;
    cout<<endl<<endl<<"processing "<<sample<<".."<<endl;

    TChain ch("output", "output");

    TString outPath;
    for(auto f : files){
      TString filepath = f.data();
      if(f.find(it->second) == std::string::npos) continue;
      outPath = f.data();
      ch.Add(filepath); cout<<filepath<<endl;
    }

    ROOT::RDataFrame df(ch, {"m_yy"});

    auto df_srCut = df.Filter(cutSR);

    auto df_forceFloat = df_srCut.Define("f_met_TST", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"met_TST"})
                                 .Define("f_phi_TST", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"phi_TST"})
                                 .Define("f_sumet_TST", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"sumet_TST"})
                                 .Define("f_m_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"m_tautau"})
                                 .Define("f_m_jj", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"m_jj"})
                                 .Define("f_pt_yy", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"pt_yy"})
                                 .Define("f_pt_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"pt_tautau"})
                                 .Define("f_eta_yy", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"eta_yy"})
                                 .Define("f_eta_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"eta_tautau"})
                                 .Define("f_phi_yy", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"phi_yy"})
                                 .Define("f_phi_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"phi_tautau"})
                                 .Define("f_dR_yy_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"dR_yy_tautau"})
                                 .Define("f_dR_yy_jj", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"dR_yy_jj"})
                                 .Define("f_dR_y_y", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"dR_y_y"})
                                 .Define("f_dR_tau_tau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"dR_tau_tau"});

    std::vector<std::string> f_variables;
    for(auto var : variables){
      string f_prefix = "f_";
      f_variables.push_back(f_prefix.append(var));
    }

    //auto df_srCut2 = df_forceFloat.Range(3); // Read only a small subset of the dataset
    //auto x = AsTensor<float>(df_srCut2, f_variables);
    //auto y = model.Compute(x);
    //std::cout << "RTensor input for inference on data of multiple events:\n" << x << "\n\n";
    //std::cout << "Prediction performed on multiple events: " << y << "\n\n";

    outPath = outPath.ReplaceAll(dirpath, "/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/mva/"); //cout<<outPath<<endl;
    df_forceFloat.Define("BDT", Compute<15, float>(model), f_variables)
                 .Define("BDT0", [](vector<float> BDT){ return (double)(1000.0*BDT.at(0)); }, {"BDT"})
                 .Snapshot("output", outPath.Data());
  }
}
