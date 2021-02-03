/// \file
/// \ingroup tutorial_tmva
/// \notebook -nodraw
/// This macro provides examples for the training and testing of the
/// TMVA classifiers.
///
/// As input data is used a toy-MC sample consisting of four Gaussian-distributed
/// and linearly correlated input variables.
/// The methods to be used can be switched on and off by means of booleans, or
/// via the prompt command, for example:
///
///     root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)
///
/// (note that the backslashes are mandatory)
/// If no method given, a default set of classifiers is used.
/// The output file "TMVA.root" can be analysed with the use of dedicated
/// macros (simply say: root -l <macro.C>), which can be conveniently
/// invoked through a GUI that will appear at the end of the run of this macro.
/// Launch the GUI via the command:
///
///     root -l ./TMVAGui.C
///
/// You can also compile and run the example with the following commands
///
///     make
///     ./TMVAClassification <Methods>
///
/// where: `<Methods> = "method1 method2"` are the TMVA classifier names
/// example:
///
///     ./TMVAClassification Fisher LikelihoodPCA BDT
///
/// If no method given, a default set is of classifiers is used
///
/// - Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis
/// - Package   : TMVA
/// - Root Macro: TMVAClassification
///
/// \macro_output
/// \macro_code
/// \author Andreas Hoecker

#include "/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/fakeFactor/fakeFactor.h"
#include "headTMVA.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

int myTMVA( TString myMethodList = "" )
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // Methods to be processed can be given as an argument; use format:
   //
   //     mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost
   Use["BDTG"]            = 0; // uses Gradient Boost
   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return 1;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // Here the preparation phase begins

   // Read training and test data
   // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
   //TFile *input(0);
   //TString fname = "./tmva_class_example.root";
   //if (!gSystem->AccessPathName( fname )) {
   //   input = TFile::Open( fname ); // check if file in local directory exists
   //}
   //else {
   //   TFile::SetCacheFileDir(".");
   //   input = TFile::Open("http://root.cern.ch/files/tmva_class_example.root", "CACHEREAD");
   //}
   //if (!input) {
   //   std::cout << "ERROR: could not open data file" << std::endl;
   //   exit(1);
   //}
   //std::cout << "--- TMVAClassification       : Using input file: " << input->GetName() << std::endl;

   // Register the training and test trees

   TFile *inputs = new TFile("/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/fakeFactor/output/h025_Jan_7/fullrun2/for_mva/450698_yy2L.root", "read");
   TFile *inputb = new TFile("/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/fakeFactor/output/h025_Jan_7/fullrun2/for_mva/MVAbkgs.root", "read");
   TFile *inputb_yy = new TFile("/scratchfs/atlas/chenhr/atlaswork/multilepton/gamgamML/fakeFactor/output/h025_Jan_7/fullrun2/for_mva/364352_Sherpa2_diphoton_myy_90_175.root", "read");

   TTree *signalTree     = (TTree*)inputs->Get("output");
   TTree *background     = (TTree*)inputb->Get("output");
   TTree *background_yy     = (TTree*)inputb_yy->Get("output");

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "outTMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory is
   // the only TMVA object you have to interact with
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");
   // If you wish to modify default settings
   // (please check "src/Config.h" to see all available global options)
   //
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  string config = "configTMVA";
  string allInputVars;
  readConfigFile(config.data(), "addVariable", allInputVars);

  std::vector<TString> tsAllVars;
  getTStringVector(allInputVars, tsAllVars, ",");

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
   //dataloader->AddVariable( "myvar1 := var1+var2", 'F' );
   //dataloader->AddVariable( "myvar2 := var1-var2", "Expression 2", "", 'F' );
   //dataloader->AddVariable( "var3",                "Variable 3", "units", 'F' );
   //dataloader->AddVariable( "var4",                "Variable 4", "units", 'F' );

  for(auto var : tsAllVars){
    dataloader->AddVariable(var, 'D');
  }

  dataloader->AddSpectator("eventNumber", 'I');

   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables

   //dataloader->AddSpectator( "spec1 := var1*2",  "Spectator 1", "units", 'F' );
   //dataloader->AddSpectator( "spec2 := var1*3",  "Spectator 2", "units", 'F' );


   // global event weights per tree (see below for setting event-wise weights)
   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;

   //double FFyield = 2.33991; // MC not subtracted
   double FFyield = 2.14684;
   double MCyield = 6.8135;
   Double_t backgroundWeight_yy = FFyield/MCyield;

   // You can add an arbitrary number of signal or background trees
   dataloader->AddSignalTree    ( signalTree,     signalWeight );
   dataloader->AddBackgroundTree( background, backgroundWeight );
   dataloader->AddBackgroundTree( background_yy, backgroundWeight_yy );

   // To give different trees for training and testing, do as follows:
   //
   //     dataloader->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
   //     dataloader->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );

   // Use the following code instead of the above two or four lines to add signal and background
   // training and test events "by hand"
   // NOTE that in this case one should not give expressions (such as "var1+var2") in the input
   //      variable definition, but simply compute the expression before adding the event
   // ```cpp
   // // --- begin ----------------------------------------------------------
   // std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
   // Float_t  treevars[4], weight;
   //
   // // Signal
   // for (UInt_t ivar=0; ivar<4; ivar++) signalTree->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   // for (UInt_t i=0; i<signalTree->GetEntries(); i++) {
   //    signalTree->GetEntry(i);
   //    for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //    // add training and test events; here: first half is training, second is testing
   //    // note that the weight can also be event-wise
   //    if (i < signalTree->GetEntries()/2.0) dataloader->AddSignalTrainingEvent( vars, signalWeight );
   //    else                              dataloader->AddSignalTestEvent    ( vars, signalWeight );
   // }
   //
   // // Background (has event weights)
   // background->SetBranchAddress( "weight", &weight );
   // for (UInt_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   // for (UInt_t i=0; i<background->GetEntries(); i++) {
   //    background->GetEntry(i);
   //    for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //    // add training and test events; here: first half is training, second is testing
   //    // note that the weight can also be event-wise
   //    if (i < background->GetEntries()/2) dataloader->AddBackgroundTrainingEvent( vars, backgroundWeight*weight );
   //    else                                dataloader->AddBackgroundTestEvent    ( vars, backgroundWeight*weight );
   // }
   // // --- end ------------------------------------------------------------
   // ```
   // End of tree registration

   // Set individual event weights (the variables must exist in the original TTree)
   // -  for signal    : `dataloader->SetSignalWeightExpression    ("weight1*weight2");`
   // -  for background: `dataloader->SetBackgroundWeightExpression("weight1*weight2");`
  dataloader->SetSignalWeightExpression("wt");
  dataloader->SetBackgroundWeightExpression( "wt" );

   // Apply additional cuts on the signal and background samples (can be different)
  string cutSR = "";
  //readConfigFile(config.data(), "cutSR", cutSR);

   //TCut mycuts = (char*)cutSR.data(); // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   //TCut mycutb = (char*)cutSR.data(); // for example: TCut mycutb = "abs(var1)<0.5";
  TCut mycut = (char*)cutSR.data(); cout<<mycut<<endl;

   // Tell the dataloader how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used
   // for training, and the other half for testing:
   //
       dataloader->PrepareTrainingAndTestTree( mycut, "nTest_Signal=2:nTest_Background=2:SplitMode=random:!V" );
   //
   // To also specify the number of testing events, use:
   //
   //    dataloader->PrepareTrainingAndTestTree( mycut,
   //         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
   //dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,
   //                                     "nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=NumEvents:!V" );

   // ### Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   UInt_t numFolds = 2;
   TString analysisType = "Classification";
   TString splitType = "Deterministic";
   TString splitExpr = "int(fabs([eventNumber]/1000))%(int([NumFolds]))";

   TString cvOptions = Form("!V"
                            ":!Silent"
                            ":ModelPersistence"
                            ":FoldFileOutput=True"
                            ":AnalysisType=%s"
                            ":SplitType=%s"
                            ":NumFolds=%i"
                            ":SplitExpr=%s",
                            analysisType.Data(), splitType.Data(), numFolds,
                            splitExpr.Data());

   TMVA::CrossValidation cv{"TMVACrossValidation", dataloader, outputFile, cvOptions};

   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
//      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTG",
//                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2" );
   cv.BookMethod(TMVA::Types::kBDT, "BDTG",
                 "!H:!V:NTrees=500:MinNodeSize=2.5%:BoostType=Grad"
                 ":Shrinkage=0.10:nCuts=20"
                 ":UseBaggedBoost:BaggedSampleFraction=0.5"
                 ":MaxDepth=2");

   if (Use["BDT"])  // Adaptive Boost
//      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",
//                           "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
   cv.BookMethod(TMVA::Types::kBDT, "BDT",
                 "!H:!V:NTrees=450:MinNodeSize=2.5%:BoostType=AdaBoost:AdaBoostBeta=0.5"
                 ":UseBaggedBoost:BaggedSampleFraction=0.5"
                 ":SeparationType=GiniIndex:nCuts=20"
                 ":MaxDepth=3");


   // For an example of the category classifier usage, see: TMVAClassificationCategory
   //
   // --------------------------------------------------------------------------------------------------
   //  Now you can optimize the setting (configuration) of the MVAs using the set of training events
   // STILL EXPERIMENTAL and only implemented for BDT's !
   //
   //     factory->OptimizeAllMethods("SigEffAtBkg0.01","Scan");
   //     factory->OptimizeAllMethods("ROCIntegral","FitGA");
   //
   // --------------------------------------------------------------------------------------------------

   // Now you can tell the factory to train, test, and evaluate the MVAs
   //
   // Train MVAs using the set of training events
//   factory->TrainAllMethods();

   // Evaluate all MVAs using the set of test events
//   factory->TestAllMethods();

   // Evaluate and compare performance of all configured MVAs
//   factory->EvaluateAllMethods();
   cv.Evaluate();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;
   delete dataloader;
   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );

   return 0;
}

int main( int argc, char** argv )
{
   // Select methods (don't look at this code - not of interest)
   TString methodList;
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(",");
      methodList += regMethod;
   }
   return myTMVA(methodList);
}
