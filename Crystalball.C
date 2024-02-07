//This script fits the Higgs boson signal production channels with a double-sided crystalball fit and saves the parameters.

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "vector"
#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "Rtypes.h"
#include "TLegend.h"
#ifndef RooFit_RooFit_RooCrystalBall_h
#define RooFit_RooFit_RooCrystalBall_h
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCrystalBall.h"
#endif
using namespace RooFit;

void Crystalball(){
	TFile *file = new TFile("ProductionChannels.root", "READ");
	TFile *file1 = new TFile("Signal.root", "READ");

	//Histograms from .root Files for Signal and individual Production Channels
	TH1D *h_ggF = (TH1D*)file->Get("h_ggF");
	TH1D *h_VBF = (TH1D*)file->Get("h_VBF");
	TH1D *h_VH = (TH1D*)file->Get("h_VH");
	TH1D *h_ttH = (TH1D*)file->Get("h_ttH");
	TH1D *h_Signal = (TH1D*)file1->Get("h_Signal");

	//Defining Parameters for the PDF and double-sided crystal ball fit
	RooRealVar mgg("mgg","mgg", 105, 160); //representing x
	RooRealVar avg_mgg("avg_mgg","avg_mgg", 125, 120, 130); //representing x0
        RooRealVar sigmaL("sigmaL", "sigmaL",1.5, 0.01, 5);
        RooRealVar sigmaR("sigmaR", "sigmaR",1.5, 0.01, 5);
        RooRealVar alphaL("alphaL", "alphaL",10, 0.01, 20);
        RooRealVar alphaR("alphaR", "alphaR",10, 0.01, 20);
        RooRealVar nL("nL","nL",5, 0.01, 15);
        RooRealVar nR("nR","nR",5, 0.01, 25);
	RooCrystalBall model("CrystalBall", "Crystal Ball Fit", mgg, avg_mgg, sigmaL, sigmaR, alphaL, nL, alphaR, nR);

	//Creating TCanvas for histograms
	TCanvas *c = new TCanvas("c", "CrystallBallFit",800,600);
	TCanvas *c1 = nullptr;
	c->Divide(2, 2,0.05,0);

	//Creating .root file to save CB parameters
	TFile *file2 = new TFile("CrystalBallParameters.root","RECREATE");
	for(int a = 0; a<5; a++){
		//Depending on iteration, histogram points to different channels
		TH1D* h_production = nullptr;
		if(a==0) h_production = h_ggF;
		if(a==1) h_production = h_VBF;
		if(a==2) h_production = h_VH;
		if(a==3) h_production = h_ttH;
		if(a==4) h_production = h_Signal;
		if (h_production == nullptr) cerr << "Error in h_production!" << endl;
		std::string production_channel = h_production->GetName();
		std::string title = h_production->GetTitle();

		//Create DataHist from histogram, perform fit
		RooDataHist data("data","data",mgg, Import(*h_production));
		model.fitTo(data,RooFit::Save());

		//Saving parameters to a RooWorkspace and into TFile
		RooWorkspace *workspace = new RooWorkspace(production_channel.substr(2).c_str());
                workspace->import(avg_mgg);
                workspace->import(sigmaL);
                workspace->import(sigmaR);
                workspace->import(alphaL);
                workspace->import(alphaR);
                workspace->import(nL);
                workspace->import(nR);
                workspace->Write();
		workspace->Print();
		cout << avg_mgg.getVal() << " " << sigmaL.getVal() << " " << sigmaR.getVal() << endl;
                cout << alphaL.getVal() << " " << alphaR.getVal() << " " << nL.getVal() << " " << nR.getVal() << endl;
		delete workspace;

		//Save plot of the different production channels fit
		if(a==4){
			c->Update();
			c->SaveAs("CrystalBallFit.png");
			c1 = new TCanvas("c1","SignalFit",800,600);
		}
		
		//Plot in quadrant a+1 for each production channel
		else c->cd(a+1);
                RooPlot* frame = mgg.frame();
                data.plotOn(frame, RooFit::Name(title.c_str()));
                model.plotOn(frame);
                frame->SetTitle(title.c_str());
		if(a==1 or a==3) frame->GetYaxis()->SetLabelOffset(-0.05);
		frame->Draw();
		//Saving the Signal Plot to a seperate png
		if(a==4) c1->SaveAs("CrystalBallFitSignal.png");
	}
	file->Close();
        file1->Close();
        file2->Close();
}

