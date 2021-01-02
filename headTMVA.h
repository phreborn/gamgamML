#include <stdio.h>
#include <string.h>

void getTStringVector(string allInputVars, std::vector<TString>& tsVector, string sSep){
  TString tsVar;
  char *p;
  char *buff = (char*)allInputVars.data();
  char *sep = (char*)sSep.data();
  p = strsep(&buff, sep);
  while(p!=NULL){
    tsVar = p;
    tsVar.ReplaceAll(" ", "");
    if(tsVar!="") tsVector.push_back(tsVar); cout<<tsVar<<endl;
    p = strsep(&buff, sep);
  }
}
