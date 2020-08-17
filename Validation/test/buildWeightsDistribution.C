#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
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
using namespace std;

// generic method to make a efficiency table


void buildWeightsDistribution(string Hgg, string DY)
{ 
  
  gROOT->Reset();  
  gROOT->SetStyle("Plain");  
  gStyle->SetPalette(52); 
  gStyle->SetOptStat(0);
  
  TFile* fileHgg = new TFile(Hgg.c_str());
  TFile* fileDY = new TFile(DY.c_str());
  
  
  TH2* hmc_Hgg = (TH2*) fileHgg->Get("diphotonDumper/histograms/ggh_125_13TeV_AllR9Eta_R9VsEtaLead"); //for STD Analysis
  //TH2* hmc_Hgg = (TH2*) fileHgg->Get("diphotonDumper/histograms/ggh_90_13TeV_AllR9Eta_R9VsEtaLead"); //for Low-Mass Analysis
  TH2* hmc_DY = (TH2*)fileDY->Get("diphotonDumper/histograms/dy_13TeV_AllR9Eta_R9VsEtaLead");
  TH2* hmc_Hgg_subl = (TH2*)fileHgg->Get("diphotonDumper/histograms/ggh_125_13TeV_AllR9Eta_R9VsEtaSubLead"); //for STD Analysis
  //TH2* hmc_Hgg_subl = (TH2*)fileHgg->Get("diphotonDumper/histograms/ggh_90_13TeV_AllR9Eta_R9VsEtaSubLead");//for Low-Mass Analysis
  TH2* hmc_DY_subl = (TH2*)fileDY->Get("diphotonDumper/histograms/dy_13TeV_AllR9Eta_R9VsEtaSubLead");
  TH2* hmc_Hgg_cl= (TH2*) hmc_Hgg->Clone("hmc_Hgg");
  TH2* hmc_DY_cl= (TH2*) hmc_DY->Clone("hmc_DY");
  TH2* hmc_Hgg_subl_cl= (TH2*) hmc_Hgg_subl->Clone("hmc_Hgg_subl"); 
  TH2* hmc_DY_subl_cl= (TH2*) hmc_DY_subl->Clone("hmc_DY_subl");

  hmc_Hgg->Sumw2();
  hmc_DY->Sumw2();
  hmc_Hgg_subl->Sumw2();
  hmc_DY_subl->Sumw2();
  
  hmc_Hgg_cl->Sumw2();
  hmc_DY_cl->Sumw2();
  hmc_Hgg_subl_cl->Sumw2();
  hmc_DY_subl_cl->Sumw2();
  
  //Dealing with negative bin contents, if exist
  for(int k=0; k<=hmc_Hgg->ProjectionY("hmc_eta",0,-1,"")->GetNbinsX();k++){
    
    for(int j=0;j<=hmc_Hgg->ProjectionX("hmc_eta",0,-1,"")->GetNbinsX();j++){
      
      if(hmc_Hgg->GetBinContent(j,k) <0){
	cout << "hmc_Hgg " << hmc_Hgg->GetBinContent(j,k) << " r9 = " << hmc_Hgg->ProjectionX("hmc_eta",0,-1,"")->GetBinCenter(j)  << " eta = "<<hmc_Hgg->ProjectionY("hmc_eta",0,-1,"")->GetBinCenter(k) <<" " <<endl;
	cout <<"setting the content of this bin to 0" << endl;
	hmc_Hgg->SetBinContent(j,k,0.);	
      }
      if(hmc_DY->GetBinContent(j,k) <0) cout << "hmc_DY " << hmc_DY->GetBinContent(j,k) << " "<<endl;
      if(hmc_Hgg_subl->GetBinContent(j,k) <0){
	cout << "hmc_Hgg_subl " << hmc_Hgg_subl->GetBinContent(j,k) <<  " r9 = " <<hmc_Hgg_subl->ProjectionX("hmc_eta",0,-1,"")->GetBinCenter(j)  << " eta = "<<hmc_Hgg_subl->ProjectionY("hmc_eta",0,-1,"")->GetBinCenter(k) <<" " <<endl;
	cout <<"setting the content of this bin to 0" << endl;
	hmc_Hgg_subl->SetBinContent(j,k,0.);	
      }
      if(hmc_DY_subl->GetBinContent(j,k) <0) cout << "hmc_DY_subl " << hmc_DY_subl->GetBinContent(j,k) << " "<<endl;
    }
  }
    
  for(int k=0; k<=hmc_Hgg->ProjectionY("hmc_eta",0,-1,"")->GetNbinsX();k++){
    
    for(int j=0;j<=hmc_Hgg->ProjectionX("hmc_eta",0,-1,"")->GetNbinsX();j++){
      if(hmc_Hgg->GetBinContent(j,k) <0) cout << "hmc_Hgg " << hmc_Hgg->GetBinContent(j,k) << " r9 bin " << j << " eta bin "<<k;
      if(hmc_DY->GetBinContent(j,k) <0) cout << "hmc_DY " << hmc_DY->GetBinContent(j,k) << " ";
    }
  }
  
  hmc_Hgg->Scale(1/hmc_Hgg->Integral());//(float(hmc_Hgg->GetNbinsX()))*(float(hmc_Hgg->GetNbinsY()))/hmc_Hgg->Integral());
  hmc_DY->Scale(1/hmc_DY->Integral());//(float(hmc_DY->GetNbinsX()))*(float(hmc_DY->GetNbinsY()))/hmc_DY->Integral());
  
  hmc_Hgg->Divide(hmc_DY);
  cout << " num bin eta: " <<hmc_Hgg->ProjectionY("hmc_eta",0,-1,"")->GetNbinsX() << " num bin r9: " <<hmc_Hgg->ProjectionX("hmc_eta",0,-1,"")->GetNbinsX() << endl;
  for(int k=0; k<=hmc_Hgg->ProjectionY("hmc_eta",0,-1,"")->GetNbinsX();k++){
    
    for(int j=0;j<=hmc_Hgg->ProjectionX("hmc_eta",0,-1,"")->GetNbinsX();j++){
      if(hmc_Hgg->GetBinContent(j,k) <0) cout << hmc_Hgg->GetBinContent(j,k) << " "<< "r9 " <<hmc_Hgg->ProjectionX("hmc_eta",0,-1,"")->GetBinCenter(j) << " bin "<<j<<" "<< "eta " <<hmc_Hgg->ProjectionY("hmc_eta",0,-1,"")->GetBinCenter(k) << " bin "<<k<<" " <<endl;
    }
  }
  
  int binEta_min= 1;
  int binEta_max = 7;
  TCanvas *cWeights_r9eta = new TCanvas("cWeights_r9eta","cWeights_r9eta");
  gStyle->SetPalette(55);
  hmc_Hgg->GetYaxis()->SetRange( binEta_min,binEta_max);
  hmc_Hgg->Draw("COLZ");
  hmc_Hgg->SetMinimum(-1.);

  //1D-comparison plots
  
  //Get 1D distributions from 2D histograms
  TH1* hmc_Hgg_r9;
  TH1* hmc_Hgg_eta;
  TH1* hmc_DY_r9;
  TH1* hmc_DY_eta;
  TH1* hmc_Hgg_subl_r9;
  TH1* hmc_Hgg_subl_eta;
  TH1* hmc_DY_subl_r9;
  TH1* hmc_DY_subl_eta;

  hmc_Hgg_r9 = hmc_Hgg_cl->ProjectionX("hmc_Hgg_r9",0,-1,"");
  hmc_Hgg_eta = hmc_Hgg_cl->ProjectionY("hmc_Hgg_eta",0,-1,"");
  hmc_DY_r9 = hmc_DY_cl->ProjectionX("hmc_DY_r9",0,-1,"");
  hmc_DY_eta = hmc_DY_cl->ProjectionY("hmc_DY_eta",0,-1,"");
  hmc_Hgg_subl_r9 = hmc_Hgg_subl_cl->ProjectionX("hmc_Hgg_subl_r9",0,-1,"");
  hmc_Hgg_subl_eta = hmc_Hgg_subl_cl->ProjectionY("hmc_Hgg_subl_eta",0,-1,"");
  hmc_DY_subl_r9 = hmc_DY_subl_cl->ProjectionX("hmc_DY_subl_r9",0,-1,"");
  hmc_DY_subl_eta = hmc_DY_subl_cl->ProjectionY("hmc_DY_subl_eta",0,-1,"");

  //Normalization to 1 
  hmc_Hgg_r9->Scale(1/hmc_Hgg_r9->Integral());
  hmc_DY_r9->Scale(1/hmc_DY_r9->Integral());
  hmc_Hgg_subl_r9->Scale(1/hmc_Hgg_subl_r9->Integral());
  hmc_DY_subl_r9->Scale(1/hmc_DY_subl_r9->Integral());
  hmc_Hgg_eta->Scale(1/hmc_Hgg_eta->Integral());
  hmc_DY_eta->Scale(1/hmc_DY_eta->Integral());
  hmc_Hgg_subl_eta->Scale(1/hmc_Hgg_subl_eta->Integral());
  hmc_DY_subl_eta->Scale(1/hmc_DY_subl_eta->Integral());


  TLegend *leg_r9 = new TLegend(.65,.75,.75,.85);
  leg_r9->SetBorderSize(0);
  leg_r9->SetTextSize(0.03 );
  TLegend *leg_eta = new TLegend(.65,.75,.75,.85);  
  leg_eta->SetBorderSize(0);
  leg_eta->SetTextSize(0.03 );
  TLegend *leg_subl_r9 = new TLegend(.65,.75,.75,.85);
  leg_subl_r9->SetBorderSize(0);
  leg_subl_r9->SetTextSize(0.03 );
  TLegend *leg_subl_eta = new TLegend(.65,.75,.75,.85);
  leg_subl_eta->SetBorderSize(0);
  leg_subl_eta->SetTextSize(0.03 );

  //R9 leading Hgg-DY normalized
  TCanvas *cR9_lead = new TCanvas("cR9_lead","cR9_lead"); cout <<"0" << endl;
  hmc_Hgg_r9->SetMarkerColor(kBlue); 
  hmc_DY_r9->SetMarkerColor(kRed);
  hmc_Hgg_r9->SetLineColor(kBlack); 
  hmc_DY_r9->SetLineColor(kBlack);
  hmc_Hgg_r9->SetMarkerStyle(8); 
  hmc_DY_r9->SetMarkerStyle(8); 
  hmc_Hgg_r9->SetMarkerSize(0.5); 
  hmc_DY_r9->SetMarkerSize(0.5);
  hmc_Hgg_r9->SetTitle("R9 distribution (leading photon)"); 
  hmc_Hgg_r9->Draw("P");
  hmc_DY_r9->Draw("SAME P");
  leg_r9->AddEntry(hmc_Hgg_r9,"H #rightarrow #gamma#gamma","pe"); 
  leg_r9->AddEntry(hmc_DY_r9,"Z #rightarrow ee","pe"); 
  leg_r9->Draw("SAME");
 
  // R9 subleading Hgg-DY normalized
  TCanvas *cR9_sublead = new TCanvas("cR9_sublead","cR9_sublead");
  hmc_Hgg_subl_r9->SetMarkerColor(kBlue); 
  hmc_DY_subl_r9->SetMarkerColor(kRed);
  hmc_Hgg_subl_r9->SetLineColor(kBlack); 
  hmc_DY_subl_r9->SetLineColor(kBlack);
  hmc_Hgg_subl_r9->SetMarkerSize(0.5); 
  hmc_DY_subl_r9->SetMarkerSize(0.5); 
  hmc_Hgg_subl_r9->SetMarkerStyle(8); 
  hmc_DY_subl_r9->SetMarkerStyle(8); 
  hmc_Hgg_subl_r9->SetTitle("R9 distribution (sub-leading photon)");  
  hmc_Hgg_subl_r9->Draw("P");
  hmc_DY_subl_r9->Draw("SAME P");
  leg_subl_r9->AddEntry(hmc_Hgg_subl_r9,"H#rightarrow #gamma#gamma","pe"); 
  leg_subl_r9->AddEntry(hmc_DY_subl_r9,"Z#rightarrow ee","pe"); 
  leg_subl_r9->Draw("SAME");
  //cR9_sublead->Draw();

  //eta leading Hgg-DY normalized 
  TCanvas *cEta_lead = new TCanvas("cEta_lead","cEta_lead");
  hmc_Hgg_eta->SetMarkerColor(kBlue);  
  hmc_DY_eta->SetMarkerColor(kRed);   
  hmc_Hgg_eta->SetLineColor(kBlack);   
  hmc_DY_eta->SetLineColor(kBlack);  
  hmc_Hgg_eta->SetMarkerSize(0.5);  
  hmc_DY_eta->SetMarkerSize(0.5);  
  hmc_Hgg_eta->SetMarkerStyle(8); 
  hmc_DY_eta->SetMarkerStyle(8); 
  hmc_Hgg_eta->SetTitle("|Eta| distribution (leading photon)"); 
  hmc_Hgg_eta->GetXaxis()->SetRange(binEta_min,binEta_max);
  hmc_Hgg_eta->Draw("P"); 
  hmc_Hgg_eta->SetMaximum(0.55);  
  hmc_DY_eta->Draw("SAME P");
  leg_eta->AddEntry(hmc_Hgg_eta,"H#rightarrow #gamma#gamma","pe"); 
  leg_eta->AddEntry(hmc_DY_eta,"Z#rightarrow ee","pe"); 
  leg_eta->Draw("SAME"); 
 

  //eta subleading Hgg-DY normalized
  TCanvas *cEta_sublead = new TCanvas("cEta_sublead","cEta_sublead");
  hmc_Hgg_subl_eta->SetMarkerColor(kBlue); 
  hmc_DY_subl_eta->SetMarkerColor(kRed); 
  hmc_Hgg_subl_eta->SetLineColor(kBlack); 
  hmc_DY_subl_eta->SetLineColor(kBlack);
  hmc_Hgg_subl_eta->SetMarkerSize(0.5); 
  hmc_DY_subl_eta->SetMarkerSize(0.5); 
  hmc_Hgg_subl_eta->SetMarkerStyle(8); 
  hmc_DY_subl_eta->SetMarkerStyle(8);  
  hmc_Hgg_subl_eta->SetTitle("|Eta| distribution (sub-leading photon)");  
  hmc_Hgg_subl_eta->GetXaxis()->SetRange(binEta_min,binEta_max);
  hmc_Hgg_subl_eta->Draw("P");
  hmc_Hgg_subl_eta->SetMaximum(0.55);  
  hmc_DY_subl_eta->Draw("SAME P");
  leg_subl_eta->AddEntry(hmc_Hgg_subl_eta,"H#rightarrow #gamma#gamma","pe"); 
  leg_subl_eta->AddEntry(hmc_DY_subl_eta,"Z#rightarrow ee","pe"); 
  leg_subl_eta->Draw("SAME");


  //summing up lead and sublead distributions
  TH1* hmc_Hgg_cl_r9= (TH1*) hmc_Hgg_r9->Clone("hmc_Hgg");
  TH1* hmc_DY_cl_r9= (TH1*) hmc_DY_r9->Clone("hmc_DY");
  TH1* hmc_Hgg_cl_eta= (TH1*) hmc_Hgg_eta->Clone("hmc_Hgg");
  TH1* hmc_DY_cl_eta= (TH1*) hmc_DY_eta->Clone("hmc_DY");
   
  hmc_Hgg_cl_r9->Add(hmc_Hgg_subl_r9,1); 
  hmc_DY_cl_r9->Add(hmc_DY_subl_r9,1);
  hmc_Hgg_cl_r9->Scale(1/hmc_Hgg_cl_r9->Integral()); 
  hmc_DY_cl_r9->Scale(1/hmc_DY_cl_r9->Integral());

  //added 11/03/2020
  hmc_Hgg_cl_eta->Add(hmc_Hgg_subl_eta,1); 
  hmc_DY_cl_eta->Add(hmc_DY_subl_eta,1);
  hmc_Hgg_cl_eta->Scale(1/hmc_Hgg_cl_eta->Integral()); 
  hmc_DY_cl_eta->Scale(1/hmc_DY_cl_eta->Integral()); 
 
  TH1* hmc_Hgg_cl2_r9= (TH1*) hmc_Hgg_cl_r9->Clone("hmc_Hgg");
  TH1* hmc_Hgg_cl2_eta= (TH1*) hmc_Hgg_cl_eta->Clone("hmc_Hgg");
 
  TCanvas *cR9_tot = new TCanvas("cR9_tot","cR9_tot");
  hmc_Hgg_cl_r9->SetMarkerColor(kBlue); 
  hmc_DY_cl_r9->SetMarkerColor(kRed);
  hmc_Hgg_cl_r9->SetLineColor(kBlack); 
  hmc_DY_cl_r9->SetLineColor(kBlack);
  hmc_Hgg_cl_r9->SetMarkerStyle(8); 
  hmc_DY_cl_r9->SetMarkerStyle(8); 
  hmc_Hgg_cl_r9->SetMarkerSize(0.5); 
  hmc_DY_cl_r9->SetMarkerSize(0.5);
  hmc_Hgg_cl_r9->SetTitle("R9 distribution");  
  hmc_Hgg_cl_r9->Draw("P");
  hmc_DY_cl_r9->Draw("SAME P");
  leg_r9->Draw("SAME");


  TCanvas *cEta_tot = new TCanvas("cEta_tot","cEta_tot");
  hmc_Hgg_cl_eta->SetMarkerColor(kBlue); 
  hmc_DY_cl_eta->SetMarkerColor(kRed);
  hmc_Hgg_cl_eta->SetLineColor(kBlack); 
  hmc_DY_cl_eta->SetLineColor(kBlack);
  hmc_Hgg_cl_eta->SetMarkerStyle(8); 
  hmc_DY_cl_eta->SetMarkerStyle(8); 
  hmc_Hgg_cl_eta->SetMarkerSize(0.5); 
  hmc_DY_cl_eta->SetMarkerSize(0.5);
  hmc_Hgg_cl_eta->SetTitle("|Eta| distribution");  
  hmc_Hgg_cl_eta->GetXaxis()->SetRange(binEta_min,binEta_max);
  hmc_Hgg_cl_eta->Draw("P");
  hmc_DY_cl_eta->Draw("SAME P");
  leg_r9->Draw("SAME");


  TCanvas *cR9_ratio = new TCanvas("cR9_ratio","cR9_tot");
  hmc_Hgg_cl2_r9->Divide(hmc_DY_cl_r9);
  hmc_Hgg_cl2_r9->SetTitle("Hgg/DY ratio (R9 distribution)");
  //hmc_Hgg_cl2_r9->SetMaximum(15.);
  hmc_Hgg_cl2_r9->Draw("P");
  

  TCanvas *cEta_ratio = new TCanvas("cEta_ratio","cEta_tot");
  hmc_Hgg_cl2_eta->Divide(hmc_DY_cl_eta);
  hmc_Hgg_cl2_eta->SetTitle("Hgg/DY ratio (|eta| distribution)"); 
  hmc_Hgg_cl2_eta->SetMaximum(2.);
  hmc_Hgg_cl2_eta->GetXaxis()->SetRange(binEta_min,binEta_max);
  hmc_Hgg_cl2_eta->Draw("P");
  

  cR9_lead->Print("R9_leadPhotons_Hgg-DY.png");  
  cR9_lead->Print("R9_leadPhotons_Hgg-DY.pdf");  
  cEta_lead->Print("Eta_leadPhotons_Hgg-DY.png");  
  cEta_lead->Print("Eta_leadPhotons_Hgg-DY.pdf"); 
  cR9_sublead->Print("R9_subleadPhotons_Hgg-DY.png");  
  cR9_sublead->Print("R9_subleadPhotons_Hgg-DY.pdf");  
  cEta_sublead->Print("Eta_subleadPhotons_Hgg-DY.png");  
  cEta_sublead->Print("Eta_subleadPhotons_Hgg-DY.pdf"); 
  cR9_tot->Print("R9_AllPhotons_Hgg-DY.png");  
  cR9_tot->Print("R9_AllPhotons_Hgg-DY.pdf");  
  cEta_tot->Print("Eta_AllPhotons_Hgg-DY.png"); 
  cEta_tot->Print("Eta_AllPhotons_Hgg-DY.pdf"); 
  cWeights_r9eta->Print("R9-eta_weights.png");
  cWeights_r9eta->Print("R9-eta_weights.pdf");
  cR9_ratio->Print("R9_weights.png");
  cR9_ratio->Print("R9_weights.pdf");
  cEta_ratio->Print("Eta_weights.png");
  cEta_ratio->Print("Eta_weights.pdf");

  TFile* fout = new TFile("r9_eta_weights2017UL.root","UPDATE");
  fout->cd();
  hmc_Hgg->Write();
  fout->Close();
  
}

