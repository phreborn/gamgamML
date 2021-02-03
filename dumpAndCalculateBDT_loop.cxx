#include "/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/fakeFactor/fakeFactor.h"

using namespace TMVA::Experimental;

void dumpAndCalculateBDT_loop(){

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

  double m_yy, BDTG, tau1_pt, tau0_rnn_flat, tau1_rnn_flat, wt;
  int n_bjet, n_tau, tau0_ntracks, tau1_ntracks, tau0_charge, tau1_charge;
  bool y1_passPID, y1_passIso, y0_passPID, y0_passIso;

  TMVA::Reader *reader = new TMVA::Reader("!Silent");
      //TMVA::Reader *reader1 = new TMVA::Reader("!Silent");
      //TMVA::Reader *reader2 = new TMVA::Reader("!Silent");

  int eventNumber;
  float met_TST,sumet_TST,m_tautau,pt_yy,pt_tautau,eta_yy,dR_y_y,dR_tau_tau,tau0_pt;
  double Dmet_TST,Dsumet_TST,Dm_tautau,Dpt_yy,Dpt_tautau,Deta_yy,DdR_y_y,DdR_tau_tau,Dtau0_pt;

  reader->AddSpectator("eventNumber", &eventNumber);
  reader->AddVariable("met_TST", &met_TST);
  reader->AddVariable("sumet_TST", &sumet_TST);
  reader->AddVariable("m_tautau", &m_tautau);
  reader->AddVariable("pt_yy", &pt_yy);
  reader->AddVariable("pt_tautau", &pt_tautau);
  reader->AddVariable("eta_yy", &eta_yy);
  reader->AddVariable("dR_y_y", &dR_y_y);
  reader->AddVariable("dR_tau_tau", &dR_tau_tau);
  reader->AddVariable("tau0_pt", &tau0_pt);

      //reader1->AddSpectator("eventNumber", &eventNumber);
      //reader1->AddVariable("met_TST", &met_TST);
      //reader1->AddVariable("sumet_TST", &sumet_TST);
      //reader1->AddVariable("m_tautau", &m_tautau);
      //reader1->AddVariable("pt_yy", &pt_yy);
      //reader1->AddVariable("pt_tautau", &pt_tautau);
      //reader1->AddVariable("eta_yy", &eta_yy);
      //reader1->AddVariable("dR_y_y", &dR_y_y);
      //reader1->AddVariable("dR_tau_tau", &dR_tau_tau);
      //reader1->AddVariable("tau0_pt", &tau0_pt);

      //reader2->AddSpectator("eventNumber", &eventNumber);
      //reader2->AddVariable("met_TST", &met_TST);
      //reader2->AddVariable("sumet_TST", &sumet_TST);
      //reader2->AddVariable("m_tautau", &m_tautau);
      //reader2->AddVariable("pt_yy", &pt_yy);
      //reader2->AddVariable("pt_tautau", &pt_tautau);
      //reader2->AddVariable("eta_yy", &eta_yy);
      //reader2->AddVariable("dR_y_y", &dR_y_y);
      //reader2->AddVariable("dR_tau_tau", &dR_tau_tau);
      //reader2->AddVariable("tau0_pt", &tau0_pt);

  reader->BookMVA("BDTG", "dataset/weights/TMVACrossValidation_BDTG.weights.xml");
      //reader1->BookMVA("BDTG", "dataset/weights/TMVACrossValidation_BDTG_fold1.weights.xml");
      //reader2->BookMVA("BDTG", "dataset/weights/TMVACrossValidation_BDTG_fold2.weights.xml");

//  RReader model("dataset/weights/TMVAClassification_BDT.weights.xml");
//  RReader model1("dataset/weights/TMVACrossValidation_BDTG_fold1.weights.xml");
//  RReader model2("dataset/weights/TMVACrossValidation_BDTG_fold2.weights.xml");
//  auto variables = model.GetVariableNames();
//  const std::size_t nVars = variables.size();

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

    TFile *fIn = new TFile(outPath, "read");

    TTree *tIn = (TTree*)fIn->Get("output");

    tIn->SetBranchAddress("m_yy"         ,&m_yy);
    //tIn->SetBranchAddress("BDTG"         ,&BDTG);
    tIn->SetBranchAddress("tau1_pt"      ,&tau1_pt);
    tIn->SetBranchAddress("tau0_rnn_flat",&tau0_rnn_flat);
    tIn->SetBranchAddress("tau1_rnn_flat",&tau1_rnn_flat);
    tIn->SetBranchAddress("wt"           ,&wt);
    tIn->SetBranchAddress("n_bjet"       ,&n_bjet);
    tIn->SetBranchAddress("n_tau"        ,&n_tau);
    tIn->SetBranchAddress("tau0_ntracks" ,&tau0_ntracks);
    tIn->SetBranchAddress("tau1_ntracks" ,&tau1_ntracks);
    tIn->SetBranchAddress("tau0_charge"  ,&tau0_charge);
    tIn->SetBranchAddress("tau1_charge"  ,&tau1_charge);
    tIn->SetBranchAddress("y1_passPID"   ,&y1_passPID);
    tIn->SetBranchAddress("y1_passIso"   ,&y1_passIso);
    tIn->SetBranchAddress("y0_passPID"   ,&y0_passPID);
    tIn->SetBranchAddress("y0_passIso"   ,&y0_passIso);

    tIn->SetBranchAddress("eventNumber"  ,&eventNumber);
    tIn->SetBranchAddress("met_TST"      ,&Dmet_TST);
    tIn->SetBranchAddress("sumet_TST"    ,&Dsumet_TST);
    tIn->SetBranchAddress("m_tautau"     ,&Dm_tautau);
    tIn->SetBranchAddress("pt_yy"        ,&Dpt_yy);
    tIn->SetBranchAddress("pt_tautau"    ,&Dpt_tautau);
    tIn->SetBranchAddress("eta_yy"       ,&Deta_yy);
    tIn->SetBranchAddress("dR_y_y"       ,&DdR_y_y);
    tIn->SetBranchAddress("dR_tau_tau"   ,&DdR_tau_tau);
    tIn->SetBranchAddress("tau0_pt"      ,&Dtau0_pt);

    outPath = outPath.ReplaceAll(dirpath, "/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/mva/"); cout<<"creating "<<outPath<<endl;

    TFile *fOut = new TFile(outPath, "recreate");

    TTree *tOut = new TTree("output","" );

    tOut->Branch("m_yy"         , &m_yy         , "m_yy/D");
    tOut->Branch("BDTG"         , &BDTG         , "BDTG/D");
    tOut->Branch("tau1_pt"      , &tau1_pt      , "tau1_pt/D");
    tOut->Branch("tau0_rnn_flat", &tau0_rnn_flat, "tau0_rnn_flat/D");
    tOut->Branch("tau1_rnn_flat", &tau1_rnn_flat, "tau1_rnn_flat/D");
    tOut->Branch("wt"           , &wt           , "wt/D");
    tOut->Branch("n_bjet"       , &n_bjet       , "n_bjet/I");
    tOut->Branch("n_tau"        , &n_tau        , "n_tau/I");
    tOut->Branch("tau0_ntracks" , &tau0_ntracks , "tau0_ntracks/I");
    tOut->Branch("tau1_ntracks" , &tau1_ntracks , "tau1_ntracks/I");
    tOut->Branch("tau0_charge"  , &tau0_charge  , "tau0_charge/I");
    tOut->Branch("tau1_charge"  , &tau1_charge  , "tau1_charge/I");
    tOut->Branch("y1_passPID"   , &y1_passPID   , "y1_passPID/O");
    tOut->Branch("y1_passIso"   , &y1_passIso   , "y1_passIso/O");
    tOut->Branch("y0_passPID"   , &y0_passPID   , "y0_passPID/O");
    tOut->Branch("y0_passIso"   , &y0_passIso   , "y0_passIso/O");
    tOut->Branch("tau0_pt"      , &Dtau0_pt      , "tau0_pt/D"); //

    Long64_t endentry = tIn->GetEntries();

    for(Long64_t i = 0; i < endentry; i++){
      tIn->GetEntry(i);

      met_TST    = Dmet_TST;
      sumet_TST  = Dsumet_TST;
      m_tautau   = Dm_tautau;
      pt_yy      = Dpt_yy;
      pt_tautau  = Dpt_tautau;
      eta_yy     = Deta_yy;
      dR_y_y     = DdR_y_y;
      dR_tau_tau = DdR_tau_tau;
      tau0_pt = Dtau0_pt; //

          //if((std::abs(eventNumber)/1000)%2==0) BDTG = 1000*reader1->EvaluateMVA("BDTG");
          //else if((std::abs(eventNumber)/1000)%2==1) BDTG = 1000*reader2->EvaluateMVA("BDTG");

      BDTG = 1000*reader->EvaluateMVA("BDTG");

      tOut->Fill();
    }

    fOut->cd();
    tOut->Write();

    delete fIn;
    delete tOut;

    //ROOT::RDataFrame df(ch, {"m_yy"});

    //auto df_srCut = df.Filter(cutSR);

    //auto df_forceFloat = df_srCut.Define("f_met_TST", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"met_TST"})
    //                             .Define("f_sumet_TST", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"sumet_TST"})
    //                             .Define("f_tau0_pt", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"tau0_pt"})
    //                             .Define("f_m_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"m_tautau"})
    //                             .Define("f_pt_yy", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"pt_yy"})
    //                             .Define("f_pt_tautau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"pt_tautau"})
    //                             .Define("f_eta_yy", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"eta_yy"})
    //                             .Define("f_dR_y_y", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"dR_y_y"})
    //                             .Define("f_dR_tau_tau", [] (double dvar) { float fvar; fvar = (float)dvar; return fvar; }, {"dR_tau_tau"});

    //std::vector<std::string> f_variables;
    //for(auto var : variables){
    //  string f_prefix = "f_";
    //  f_variables.push_back(f_prefix.append(var));
    //}

    ////auto df_srCut2 = df_forceFloat.Range(3); // Read only a small subset of the dataset
    ////auto x = AsTensor<float>(df_srCut2, f_variables);
    ////auto y = model.Compute(x);
    ////std::cout << "RTensor input for inference on data of multiple events:\n" << x << "\n\n";
    ////std::cout << "Prediction performed on multiple events: " << y << "\n\n";

    //outPath = outPath.ReplaceAll(dirpath, "/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/mva/"); //cout<<outPath<<endl;
    //df_forceFloat.Define("vBDTG_fold1", Compute<9, float>(model1), f_variables)
    //             .Define("vBDTG_fold2", Compute<9, float>(model2), f_variables)
    //             .Define("BDTG", [](vector<float> vBDTG_fold1, vector<float> vBDTG_fold2, int evtNum){
    //               return std::abs(evtNum)/1000%2==0? (double)(1000.0*vBDTG_fold1.at(0)) : (double)(1000.0*vBDTG_fold2.at(0));
    //             }, {"vBDTG_fold1", "vBDTG_fold2", "eventNumber"})
    //             .Snapshot("output", outPath.Data());
  }
}
