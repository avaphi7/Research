//This script takes the saved parameters from Crystalball.C and fits the signal data with the double-sided crystalball fit. The background data is fit with a Bernstein polynomial fit.

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
#include "RooCrystalBall.h"
#include "RooFitResult.h"
using namespace RooFit ;

void HiggsFit(){
	TFile *file = new TFile("Signal.root", "READ");
	TFile *file1 = new TFile("Background.root", "READ");
	
	RooRealVar mgg("mgg", "mgg", 105, 160);
	//Histograms from Binned Data 
	TH1D *h_Background = (TH1D*)file1->Get("h_Background");
	TH1D *h_Signal = (TH1D*)file->Get("h_Signal");

	//Creating Invariant Mass Histogram 
	TH1D *h_mass = new TH1D("h_mass", "h_mass", 1100, 105, 160);
	h_mass->Add(h_Background);
	h_mass->Add(h_Signal);

	RooDataHist data("data", "data", mgg, Import(*h_mass));
	TH1D *h_mRebin = (TH1D*)h_mass->Rebin(11,"Histogram Rebinned");
	RooDataHist dataRebin("data", "data", mgg, Import(*h_mRebin));

	//Using Bernstein Polynomial as Background PDF
	RooRealVar coef0("coef0", "Coefficient 0", 0.5, -100, 100);
	RooRealVar coef1("coef1", "Coefficient 1", 0.5, -100, 100);
	RooRealVar coef2("coef2", "Coefficient 2", 0.2, -100, 100);
	RooRealVar coef3("coef3", "Coefficient 3", 0.1, -100, 100);
	RooRealVar coef4("coef4", "Coefficient 4", 0.05, -100, 100);
	//RooRealVar coef5("coef5", "Coefficient 5", 0.01, -100, 100);
	RooArgList coefList(coef0,coef1, coef2, coef3, coef4);
	RooBernstein background_pdf("bernstein", "Bernstein polynomial", mgg, coefList);	
	
	mgg.setRange("total",105,160);

	//CrystalBall Fit Parameters for gluon gluon fusion production
	TFile *params_file = new TFile("CrystalBallParameters.root","READ");
	RooWorkspace *params_workspace = (RooWorkspace*)params_file->Get("ggF");
	RooArgSet parameters = params_workspace->allVars();

	RooRealVar* avg_mgg = dynamic_cast<RooRealVar*>(parameters.find("avg_mgg"));
	RooRealVar* sigmaL = dynamic_cast<RooRealVar*>(parameters.find("sigmaL"));
	RooRealVar* sigmaR = dynamic_cast<RooRealVar*>(parameters.find("sigmaR"));
	RooRealVar* alphaL = dynamic_cast<RooRealVar*>(parameters.find("alphaL"));
	RooRealVar* alphaR = dynamic_cast<RooRealVar*>(parameters.find("alphaR"));
	RooRealVar* nL = dynamic_cast<RooRealVar*>(parameters.find("nL")); 
	RooRealVar* nR = dynamic_cast<RooRealVar*>(parameters.find("nR"));	
	RooCrystalBall signal_pdf("CrystalBall", "Crystal Ball Fit", mgg,*avg_mgg, *sigmaL, *sigmaR, *alphaL, *nL, *alphaR, *nR);

	avg_mgg->setConstant(true);
	sigmaL->setConstant(true);
	sigmaR->setConstant(true);
	alphaL->setConstant(true);
	alphaR->setConstant(true);
	nL->setConstant(true);
	nR->setConstant(true);
	
	//Fitting to an extended PDF with background + signal Gaussian
	float upperlim =  h_mass->Integral();
	RooRealVar numBackground("numBackground", "Number of background events", upperlim/100, 0, upperlim);
	RooRealVar numSignal("numSignal", "Number of signal events", upperlim/100, 0, upperlim);
	RooAddPdf model("model", "Extended PDF", RooArgList(signal_pdf, background_pdf), RooArgList(numSignal, numBackground));
	RooFitResult *result = model.fitTo(data,RooFit::Range("total"),RooFit::Save());
	RooExtendPdf extended_pdf("extendedPdf", "Extended PDF with normalization", model, numSignal);

	cout << "numSignal Value: " << numSignal.getVal() << " +- " << numSignal.getError() << endl;
	cout << "numBackground Value: " << numBackground.getVal() << " +- " << numBackground.getError() << endl;

	double dataValue, fittedValue, residual, fittedValue1,residual1;
	TH1D *h_residual = new TH1D("h_residuals","h_residuals",1100,105,160);
	double scaleFactor = numBackground.getVal() / background_pdf.createIntegral(mgg,RooFit::Range(""))->getVal();
	float width = h_mass->GetBinWidth(1);
	float center;
	for (int i = 0; i < h_mass->GetNbinsX(); ++i) {
        	center = h_mass->GetBinCenter(i+1);
	        mgg.setRange("bin",center-width/2,center+width/2);
        	dataValue = h_mass->GetBinContent(i+1);
        	fittedValue = background_pdf.createIntegral(mgg,RooFit::Range("bin"))->getVal()*scaleFactor;
        	residual = dataValue - fittedValue;

		h_residual->SetBinContent(i+1, residual);
        	h_residual->SetBinError(i+1,h_mass->GetBinError(i+1));

	}
	TH1D *h_residualRebin =  (TH1D*)h_residual->Rebin(11,"residualshistogram");
	RooDataHist data_residual("data", "data", mgg, Import(*h_residualRebin));

	//Plot invariant mass histogram and fit for background data
	double signalScale = numSignal.getVal();
	TCanvas *c = new TCanvas("c","Invariant Mass Distribution",800,600);
	c->Divide(1, 2,0,0);
	c->cd(1);
	RooPlot* frame = mgg.frame();
	signal_pdf.plotOn(frame,RooFit::Name("Signal PDF"),RooFit::Normalization(signalScale,RooAbsReal::NumEvent),RooFit::LineColor(kAzure+6));
	dataRebin.plotOn(frame,RooFit::Name("data"));
	extended_pdf.plotOn(frame,RooFit::Name("Total PDF"));
	background_pdf.plotOn(frame,RooFit::Name("Background PDF"),RooFit::LineStyle(kDotted), RooFit::LineColor(kBlack));
	frame->GetXaxis()->SetTitle("");
	frame->GetYaxis()->SetTitle("Events / GeV");
	frame->GetYaxis()->SetTitleSize(0.05);
	frame->SetTitle("Diphoton Invariant Mass Distribution");
	frame->SetTitleSize(0.1, "t");
	frame->Draw();
	TLegend* legend = new TLegend(0.65, 0.75, 0.85, 0.85);
	legend->AddEntry("data", "Data", "P");
	legend->AddEntry("Total PDF", "Total PDF", "l");
	legend->AddEntry("Signal PDF", "Signal PDF", "l");
	legend->AddEntry("Background PDF", "Background PDF", "l");
	legend->SetBorderSize(0);
	legend->SetFillStyle(0);
	legend->SetX1(0.65);
	legend->SetY1(0.6);
	legend->SetX2(0.95);
	legend->SetY2(0.9);
	legend->Draw();
	
	c->cd(2);
	RooPlot* frame1 = mgg.frame();
	
	data_residual.plotOn(frame1,RooFit::Name("Residuals"));
mgg.setRange("signal1",120,130);
	signal_pdf.plotOn(frame1,RooFit::Range("total"),RooFit::Normalization(signalScale,RooAbsReal::NumEvent),RooFit::Name("Signal PDF"),RooFit::LineColor(kAzure+6));
	frame1->GetXaxis()->SetTitle("m_{#gamma#gamma} (GeV)");
	frame1->GetXaxis()->SetTitleSize(0.05);
	frame1->GetYaxis()->SetTitle("Data - Background");
	frame1->GetYaxis()->SetTitleSize(0.05);
	frame1->SetTitle("");
	frame1->Draw();
	TLegend* legend1 = new TLegend(0.65, 0.75, 0.85, 0.85);
	legend1->AddEntry("Residuals", "Residuals", "P");
	legend1->AddEntry("Signal PDF", "Signal PDF", "l");
	legend1->SetBorderSize(0);
	legend1->SetFillStyle(0);
	legend1->SetX1(0.65);
	legend1->SetY1(0.6);
	legend1->SetX2(0.95);
	legend1->SetY2(0.9);
	legend1->Draw();
	
	c->Update();
	c->SaveAs("InvariantMassFit.png");

	//Fit for null hypothesis
	numSignal.setVal(0);
	numSignal.setConstant(true);
	RooFitResult *resultNull = model.fitTo(data,RooFit::Range("total"),RooFit::Save());
	double sig = sqrt(2*(resultNull->minNll()-result->minNll()));
	cout << "Significance: " << sig << endl;

	file->Close();
	file1->Close();

}

