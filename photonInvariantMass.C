//Exercise creating histograms.
void photonInvariantMass() {
    // Open the ROOT file containing the data
    TFile *file = TFile::Open("test.root");
    
    // Access the TTree named "output" from the file
    TTree *tree = (TTree*)file->Get("output");

    // Create a histogram to store the invariant mass of the two photons
    TH1F *h_mgg = new TH1F("h_mgg", "Invariant Mass of Two Photons; m_{#gamma#gamma} [GeV]; Events", 100, 0, 500);

    //1.answer: 
    TH1F *h_pT = new TH1F("h_pT", "Transverse momentum of the diphoton system", 100, 0, 500);

    //2. answer:
    TH1F *h_m_met = new TH1F("h_m_met", "Missing Transverse Energy", 20, 0, 3);

    //3. answer:
    TH1F *h_ph_pt3 = new TH1F("h_ph_pt3", "Transverse momentum of the third photon", 100, 0, 250);
    // Declare variables to hold the values from the tree branches
    Float_t ph_pt1, ph_eta1, ph_phi1;
    Float_t ph_pt2, ph_eta2, ph_phi2;
    Float_t mmet;
    Float_t ph_pt3;

    // 1.  Draw a distribution of the transverse momentum of the diphoton system
    // 2.  Draw the distribution of m_met
    // 3. Draw the pT distribution of the third photon ph_*3
    
    // Link the above variables to the branches in the tree


    tree->SetBranchAddress("ph_pt1", &ph_pt1);
    tree->SetBranchAddress("ph_eta1", &ph_eta1);
    tree->SetBranchAddress("ph_phi1", &ph_phi1);
    tree->SetBranchAddress("ph_pt2", &ph_pt2);
    tree->SetBranchAddress("ph_eta2", &ph_eta2);
    tree->SetBranchAddress("ph_phi2", &ph_phi2);
    tree->SetBranchAddress("m_met", &mmet);
    tree->SetBranchAddress("ph_pt3", &ph_pt3);

    // Get the number of entries (events) in the tree
    Long64_t nentries = tree->GetEntries();

    // Loop over each event in the tree
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);  // Load the data for the current event
	h_m_met->Fill(mmet);
	h_ph_pt3->Fill(ph_pt3);

        // Create Lorentz vectors to store the 4-momenta of the photons
        TLorentzVector photon1, photon2;

        // Set the 4-momenta for each photon (using SetPtEtaPhiM method as we have pT, eta, and phi)
        photon1.SetPtEtaPhiM(ph_pt1, ph_eta1, ph_phi1, 0);  // Photons are massless, so m = 0
        photon2.SetPtEtaPhiM(ph_pt2, ph_eta2, ph_phi2, 0);

	// pT of photons needs to be greater than 30 GeV

	if(photon1.Pt() < 30 || photon2.Pt() < 30)
	  continue;
        // Calculate the invariant mass of the two photons
        float mgg = (photon1 + photon2).M();
	
	//        cout << mgg << endl;
        // Fill the histogram with the calculated invariant mass
        h_mgg->Fill(mgg);
	//Fill histogram with pT of the two photons
	float pT = (photon1 + photon2).Pt();
	h_pT->Fill(pT);
    }


    // Create a canvas to draw the histogram
    TCanvas *c = new TCanvas("c", "Photon Invariant Mass", 800, 600);
    
    // Draw the histogram on the canvas
    h_mgg->Draw();

    // Save the canvas as an image
    c->SaveAs("PhotonInvariantMass.png");

    //Canvas for pT
    TCanvas *c1 = new TCanvas("c1", "Transverse Momentum of Diphoton System", 800, 600);
    h_pT->Draw();

    c1->SaveAs("TransverseMomentumDiphotonSystem.png");
    //Canvas for m_met
    TCanvas *c2 = new TCanvas("c2", "Missing Transverse Energy", 800, 600);
    h_m_met->Draw();

    c2->SaveAs("MissingTransverseEnergy.png");
    //Canvas for ph_pt3
    TCanvas *c3 = new TCanvas("c3", "Third Photon Transverse Momentum", 800, 600);
    h_ph_pt3->Draw();

    c3->SaveAs("ThirdPhotonTransverseMomentum.png");

    // Close the file
    file->Close();
}
