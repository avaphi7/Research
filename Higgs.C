//This script analyzes the invariant mass distribution of the final-state particles produced in proton-proton collisions.

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "vector"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include <TChain.h>
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "Rtypes.h"
#include "TLegend.h"
#include "RooWorkspace.h"
#include "RooAbsData.h"
using namespace RooFit ;

void Higgs(){
	
	//Histograms for Signal and Background Data
	TH1D *h_Signal = new TH1D("h_Signal", "Signal Mass Distribution",1100, 105, 160);
	TH1D *h_Background = new TH1D("h_Background", "Background Mass Distribution",1100, 105, 160);
	
	//Histograms for Production Channels
	TH1D *h_ggF = new TH1D("h_ggF", "Gluon Fusion Production", 1100, 105, 160);
	TH1D *h_VBF = new TH1D("h_VBF", "Vector Boson Fusion Production", 1100, 105, 160);
	TH1D *h_VH = new TH1D("h_VH", "Associated Production with Vector Boson", 1100, 105, 160);
	TH1D *h_ttH = new TH1D("h_ttH", "Associated Production wtih Top Quarks", 1100, 105, 160);
	
	//Declare variables in tree
	float y1_pt, y1_eta, y1_phi = 0;
	float y2_pt, y2_eta, y2_phi = 0;
	float myy;
	Char_t passed = 0;
	float weight,c_s = 0;
	UInt_t channel_num = 0;
	vector <int> channels  = {364352,508784,601521,601482,601483,601484,601523,601522,601481};
	vector <double> sum_weights = {357737100,95504710000,123287100,7593420,115394,184347,419728,3590.61,279324};
	
	//Open files for Production Channels and total Background data 
	for (int i = 0; i<5; i++){
		TChain *tree = new TChain("output");
		if (i == 0){
			tree->Add("background/*.root"); //all background data
			}
		if (i == 1){
			tree->Add("signal/mc21a.PhPy8EG_PDF4LHC21_gg*");
		}
		if (i == 2){
			tree->Add("signal/mc21a.PhPy8EG_PDF4LHC21_VBF*");
		}
		if (i == 3){
			tree->Add("signal/mc21a.PhPy8EG_PDF4LHC21_W*");
			tree->Add("signal/mc21a.PhPy8EG_PDF4LHC21_ZH125J_Zincl_MINLO.MxAODDetailedNoSkim.e8472_s3873_r13829_p5441_h029.root");
		}
		if (i ==4){
			tree->Add("signal/mc21a.PhPy8EG_PDF4LHC21_ttH*");
		}

		//Set Branch Addresses
		tree->SetBranchAddress("y1_pt", &y1_pt);
		tree->SetBranchAddress("y2_pt", &y2_pt);
		tree->SetBranchAddress("y1_eta", &y1_eta);
		tree->SetBranchAddress("y2_eta", &y2_eta);
		tree->SetBranchAddress("y1_phi", &y1_phi);
		tree->SetBranchAddress("y2_phi", &y2_phi);
		tree->SetBranchAddress("HGamEventInfoAuxDyn.isPassed",&passed);
		tree->SetBranchAddress("HGamEventInfoAuxDyn.weight",&weight);
		tree->SetBranchAddress("HGamEventInfoAuxDyn.crossSectionBRfilterEff",&c_s);
		tree->SetBranchAddress("EventInfoAuxDyn.mcChannelNumber",&channel_num);
		TLorentzVector photon1,photon2;
		float normalized_weight;
		int index;
		int nentries = tree->GetEntries();

		for (int j = 0; j < nentries; j++){
			tree->GetEntry(j);
			//Passed cut requirements
			if (passed==0){
				continue;
			}
			//Invariant Mass
			photon1.SetPtEtaPhiM(y1_pt,y1_eta,y1_phi,0);
			photon2.SetPtEtaPhiM(y2_pt,y2_eta,y2_phi,0);
			myy = (photon1 + photon2).M()/1000;
			index = 0;
			//sum_weight value
			for (int k=1; k<channels.size()+1;k++){
				if (channel_num == channels[k]){
					index = k;
					break;
				}
			}
			if (index<=0){
				cout <<"Error Channel Index Invalid!!" << endl;
				continue;
			}
			normalized_weight = 31400*c_s*weight/sum_weights[index];

			//Filling Histograms
			if (i==0) h_Background->Fill(myy,normalized_weight);
			if (i==1) h_ggF->Fill(myy, normalized_weight);
			if (i==2) h_VBF->Fill(myy, normalized_weight);
			if (i==3) h_VH->Fill(myy, normalized_weight);
			if (i==4) h_ttH->Fill(myy, normalized_weight);
			if (i>0) h_Signal->Fill(myy, normalized_weight);

		}
	}
	//delete tree;

	//Making separate files for background, signal, and production channels
	TFile *file = new TFile("Background.root", "RECREATE");
	h_Background->Write();
	file->Close();

	TFile *file1 = new TFile("Signal.root", "RECREATE");
	h_Signal->Write();
	file1->Close();

	TFile *file2 = new TFile("ProductionChannels.root","RECREATE");
	h_ggF->Write();
	h_VBF->Write();
	h_VH->Write();
	h_ttH->Write();
	file2->Close();
}

