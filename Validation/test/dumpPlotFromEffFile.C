///////////////////////////////////////////////////////////////////////////////////
////////// Example usages:

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TKey.h>
#include <TClass.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream> 
#include <TGraphAsymmErrors.h>
#include "tdrstyle.C"
#include "CMS_lumi.C"
#include <RooHist.h>

using namespace std;
//r9Bin = 0 ==> no R9 restriction
//r9Bin = 1 ==> R9 Turn-on bin
//r9Bin = 2 ==> R9 plateau bin

void EffCategories( string efficiencyFolderName = "", string inputFileName = "HLT",
			   TString dir="PhotonToRECO", 
		    bool isCnt = false, int r9Bin = 0, bool isUnseeded = false ,bool isLM = true,string var = "pt", string outFolder = "test") {

  std::cout << "   ##################################################   " << std::endl;
  if(isCnt) {
    std::cout << "Plotting efficiency from cut & count. No background subtraction performed !" << std::endl;
    std::cout << "If you want to plot MC truth efficiency, please set: isMCTruth = true." << std::endl;
  }
  else std::cout << "Plotting efficiency from simultaneous fit." << std::endl;
  std::cout << "   ##################################################   " << std::endl;


  //cout << efficiencyFolderName<< " "<< inputFileName << " " << dir << " " << isCnt << " " << path<< " " << leg << " " << var << " " << outFolder << endl;
  gROOT->Reset();  
  gROOT->SetStyle("Plain");  
  gStyle->SetPalette(52); 
  gStyle->SetOptStat(0);
    
  setTDRStyle(); 
    
  writeExtraText = 1;
  extraText = " Preliminary";
  lumi_sqrtS = "41.5 fb^{-1} 2017 (13 TeV)"; //standard trigger
  //  lumi_sqrtS = "57.2 fb^{-1} 2018 (13 TeV)"; //standard trigger
  //  lumi_sqrtS = "54.4 fb^{-1} 2018 (13 TeV)"; //low mass trigger

  
  int iPos = 11;

  int iPeriod = 0;  
  lumiTextSize     = 0.80;//0.7;
  cmsTextSize      = 1;//0.7;
  extraOverCmsTextSize  = 0.80;//0.80;//0.63; 


  int icat = 1;
  if(isLM == 1)icat = 3;
  else icat = 4; 
  
  TFile* fEff[icat];
  TGraphAsymmErrors *aeEff[icat];
  TGraphAsymmErrors *aeNewEff[icat];
  
  TKey *keyEff[icat];
  TCanvas* cEff[icat]; 
  const char* nameEff[icat];
  TObject *objEff[icat]; 
  TString dirNameEff[icat];
  TString canvasname;

  if(TString(var) =="pt") canvasname ="ph_et_PLOT";
  else if(TString(var) =="npv") canvasname ="event_nPV_PLOT";
  else if(TString(var) =="r9") canvasname ="ph_full5x5x_r9_PLOT";

  string tempName;
  string tempname[4]= {"_cat0","_cat1","_cat2","_cat3"};

  for(int i =0;i<icat;i++){
  //for(int i =2;i<3;i++){
    
    tempName = efficiencyFolderName + "efficiency-data-passing"+inputFileName+tempname[i]+".root";
    
    fEff[i] = new TFile(tempName.c_str());
    fEff[i]->cd(dir);
  
    TIter nextEff(gDirectory->GetListOfKeys()); 
 
    while ((keyEff[i] = (TKey*) nextEff())) {
      keyEff[i]->Print();
      objEff[i] = keyEff[i]->ReadObj();
      objEff[i]->Print();
      nameEff[i] = objEff[i]->GetName();
      cout << nameEff[i] <<endl;//"passingHLTl1"
      std::cout << "   ==================================================   " << std::endl;
      
      
      if(isCnt == true)  dirNameEff[i] = Form("%s/cnt_eff_plots/", nameEff[i]);
      else  dirNameEff[i] = Form("%s/fit_syst_eff_plots/", nameEff[i]);
      //else  dirNameEff[i] = Form("%s/fit_eff_plots/", nameEff[i]); //this is without altbkg systematics
      gDirectory->cd(dirNameEff[i]);
      
      cEff[i] = (TCanvas*) gDirectory->Get(canvasname);
      if(cEff[i]==0)  cout <<canvasname <<" doesn't exist" <<endl;
      else cout << canvasname <<" exists" << endl;
      // cEff[i]->Draw("same");
      //get back to the initial directory
      if( !isCnt ) gDirectory->cd("../");
      else gDirectory->cd("../../");
      
      std::cout << "   ==================================================   " << std::endl;
    }
    
    aeEff[i] = (TGraphAsymmErrors*)(cEff[i]->GetPrimitive(canvasname)->Clone("aeEff[i]")); 

    //aeEff[i]->Draw();

  }

  
  //PLOTTING
  
  double xmin = 0, xmax = 0, ymin = 0, ymax = 0;

  if(var == "pt"){
    xmin =15;
    xmax = 135; 
    ymin = 0.5;//6*pvEff[1];
    ymax = 1.05;
  }
  else if(var == "abseta"){
    xmin =0.;
    xmax = 3.; 
    ymin = 0.7; 
    ymax = 1.02;
  }
  else if(var == "eta"){
    xmin =-3.;
    xmax = 3.; 
    ymin = 0.7; 
    ymax = 1.02;
  }

 
   TCanvas *c = new TCanvas("c", "c");
   TH1*h = c->DrawFrame(xmin,ymin,xmax,ymax,"");
   if(var == "pt") h->GetXaxis()->SetTitle("probe electron E_{T} (GeV)");
   if(isUnseeded) h->GetYaxis()->SetTitle("unseeded leg efficiency");
   else h->GetYaxis()->SetTitle("seeded leg efficiency");
   h->GetYaxis()->SetTitleOffset(1.4);
   h->SetTitle("");
   c->Modified();
   c->cd();
   cout << "cout2" << endl;
   TLegend *legend = new TLegend(.45,.25,.80,.45);
   legend->SetBorderSize(1);
   legend->SetTextSize(0.03 );
   
   aeEff[0]->SetMarkerColor(kBlack);
   legend->AddEntry(aeEff[0],"EB R_{9} > 0.85","pe");
   aeEff[0]->Draw("same PE");
   
   aeEff[1]->SetMarkerColor(kMagenta);
   /* for(int k =0; k <aeEff[1]->GetN(); k++){ */
   /*   cout << "point X "<< aeEff[2]->GetX()[k] << endl; */
   /*   cout << "point Y "<< aeEff[2]->GetY()[k] << endl; */
   /* } */
   if(r9Bin == 0 || ((r9Bin == 1 || r9Bin == 2) && isLM == false)) legend->AddEntry(aeEff[1],"EE R_{9} > 0.90","pe");
   else if(r9Bin == 1 && isLM == true) legend->AddEntry(aeEff[1],"EE 0.90 < R_{9}< 0.93","pe");
   else if(r9Bin == 2 && isLM == true) legend->AddEntry(aeEff[1],"EE R_{9} > 0.93","pe");
   else cout << "wrong R9 bin for efficiencies" << endl;
   aeEff[1]->Draw("same pe");
   //aeEff[1]->Draw();
   
   aeEff[2]->SetMarkerColor(kRed);
   if(r9Bin == 0) legend->AddEntry(aeEff[2],"EB 0.50 < R_{9} < 0.85","pe");
   else if(r9Bin == 1) legend->AddEntry(aeEff[2],"EB 0.50 < R_{9} < 0.56","pe");
   else if(r9Bin == 2) legend->AddEntry(aeEff[2],"EB 0.56 < R_{9} < 0.85","pe");
   else cout << "wrong R9 bin for efficiencies" << endl;
  
   aeEff[2]->Draw("same PE");
  
   if(isLM==false){
     aeEff[3]->SetMarkerColor(kBlue);
     if(r9Bin == 0) legend->AddEntry(aeEff[3],"EE 0.80 < R_{9} < 0.90","pe");
     else if(r9Bin == 1) legend->AddEntry(aeEff[3],"EE 0.80 < R_{9} < 0.85","pe");
     else if(r9Bin == 2) legend->AddEntry(aeEff[3],"EE 0.85 < R_{9} < 0.90","pe");
     else cout << "wrong R9 bin for efficiencies" << endl;
     aeEff[3]->Draw("same pe");
   }
  
   legend->Draw("same");

   CMS_lumi(c,iPeriod,0);


   TString turnon;
   if(r9Bin == 0) turnon = "_AllR9";
   else if(r9Bin == 1)  turnon = "_TurnOnBin";
   else if(r9Bin == 2)  turnon = "_PlateauBin";
   else cout << "wrong R9 bin for efficiencies" << endl;
   
   TString cnt;
   if(isCnt == true) cnt = "_cnt";
   else cnt = "_fit";
   
   TString outpath = "/afs/cern.ch/user/l/lfinco/www/HGG/HLT/"+outFolder+"/";
   system("mkdir "+outpath);
   system("cp /afs/cern.ch/user/l/lfinco/www/HGG/HLT/index.php "+outpath);
   
   
   if(isUnseeded==true){
     c->Print(outpath+"/efficiency-data-passing"+inputFileName+"_unseeded_"+var+turnon+cnt+".png");
     c->Print(outpath+"/efficiency-data-passing"+inputFileName+"_unseeded_"+var+turnon+cnt+".pdf");
   }
   
   else{
     c->Print(outpath+"/efficiency-data-passing"+inputFileName+"_seeded_"+var+turnon+cnt+".png");
     c->Print(outpath+"/efficiency-data-passing"+inputFileName+"_seeded_"+var+turnon+cnt+".pdf");
   }

}

void plotAllEfficiencies(string efficiencyFolderName = "", string var = "pt",  bool isUnseeded = false, bool isLM = true, string outFolder = "test"){
     
  EffCategories(efficiencyFolderName + "/allR9/", "HLT", "PhotonToRECO", true, 0, isUnseeded, isLM,var, outFolder);
  EffCategories(efficiencyFolderName + "/R9TurnOnBin/", "HLT", "PhotonToRECO", true, 1, isUnseeded, isLM,var, outFolder);
  EffCategories(efficiencyFolderName + "/R9PlateauBin/", "HLT", "PhotonToRECO", true, 2, isUnseeded, isLM,var, outFolder);
  
  //fit
  EffCategories(efficiencyFolderName + "/allR9/", "HLT", "PhotonToRECO", false, 0, isUnseeded, isLM,var, outFolder);
  EffCategories(efficiencyFolderName + "/R9TurnOnBin/", "HLT", "PhotonToRECO", false, 1, isUnseeded, isLM,var, outFolder);
  EffCategories(efficiencyFolderName + "/R9PlateauBin/", "HLT", "PhotonToRECO", false, 2, isUnseeded, isLM,var, outFolder);

}



