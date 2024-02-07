//Cut flow exercise
//Runs a proton-proton collision through a series of criteria and fills histogram with the data.

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>

void cutflow() {
	TFile *file = new TFile("photon.root");
	TString rootFileName = "photon.root";
	if (!file || file->IsZombie()) {
        cerr << "Error: Cannot open ROOT file " << rootFileName << endl;
        return;
    }
	TTree *tree = (TTree*)file->Get("output");

	vector<double> *ph_pt = nullptr;
	vector<double> *ph_eta = nullptr;
	vector<double> *ph_phi = nullptr;

	tree->SetBranchAddress("ph_pt", &ph_pt);
	tree->SetBranchAddress("ph_eta", &ph_eta);
	tree->SetBranchAddress("ph_phi", &ph_phi);

	TH1D *h_cutflow = new TH1D("h_cutflow", "Cutflow Histogram", 6, 0, 6);
	int nentries = tree->GetEntries();
	int pt1 = 0;
	int pt2 = 0;
	int index1 = 0;
	int index2 = 0;
	double eta = 0;
	int data = 0;
	TLorentzVector photon1, photon2;
	//Looping over all events and filling histogram
	for (Long64_t i=0; i < nentries; i++) {
		tree->GetEntry(i);
		h_cutflow->Fill(0.5);

		//First cut

		//at least 2 photons per event
		if (ph_pt->size()<2) continue;
		//leading and subleading
		int numph_pt = ph_pt->size();
		for (Long64_t j=0;j<numph_pt; j++){
		if ((*ph_pt)[j] > pt1){
			pt2 = pt1;
			index2 = index1;
			pt1 = (*ph_pt)[j];
			index1 = j;
		}
		else if ((*ph_pt)[j] > pt2){
			pt2 = (*ph_pt)[j];
			index2 = j;
		}
		//pT requirements
		photon1.SetPtEtaPhiM((*ph_pt)[index1],(*ph_eta)[index1],(*ph_phi)[index1],0);
		photon2.SetPtEtaPhiM((*ph_pt)[index2],(*ph_eta)[index2],(*ph_phi)[index2],0);
		
		if (photon1.Pt()<25 or photon2.Pt()<25){
			continue;
		}
		//eta requirements 
		eta = abs((*ph_eta)[j]);
		if (eta < 1.37 or (1.52 < eta and eta < 2.37)) {
		data = data + 1;
		}
		}
		h_cutflow->Fill(1.5);

		//Second Cut
		
		if (photon1.Pt()<40){
			continue;
		}
		h_cutflow->Fill(2.5);

		//Third Cut

		if (photon2.Pt()<30){
			continue;
		}
		h_cutflow->Fill(3.5);

		//Fourth Cut

		TLorentzVector diphoton = photon1 + photon2;
		double myy = diphoton.M();
		if (photon1.Pt() / myy < 0.4 && photon2.Pt() / myy < 0.3) {
			continue;
		}
		h_cutflow->Fill(4.5);

		//Fifth Cut 
		
		if (myy < 105 or myy > 160){
			continue;
		}
		h_cutflow->Fill(5.5);

		// Retrieve and print bin content and errors
		for (int bin = 1; bin <= h_cutflow->GetNbinsX(); ++bin) {
			double binContent = h_cutflow->GetBinContent(bin);
			double binError = h_cutflow->GetBinError(bin);
			if (bin == 1) std::cout<< "Initial Sample Bin";
			else std::cout << "Cut:" << bin ;
			std::cout << "Cut " << bin << ": Events = " << binContent << ", Error = " << binError << std::endl;
    }
	}
}





	
