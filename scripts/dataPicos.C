#include "iostream.h"

class     StChain;
class     St_db_Maker;


StChain  *chain=0;

void dataPicos( const Char_t *fileList = "small.lis",
				const Char_t *name = ".root",
				int maxEvents = 50000,
				int maxFiles = 10 )
{
  	

  	// Load libraries
	gROOT->Macro("loadMuDst.C");
	gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
	loadSharedLibraries();
	
	gSystem ->Load("StChain");
    gSystem ->Load("StMiniMcEvent");
	gSystem->Load("StRefMultCorr");
	gSystem->Load("StRooBarb");
	

	gSystem->Load("StSpectraSkimmer");
	gSystem->Load("StSpectraPicoMaker");
	gSystem->Load("StSpectraQAMaker");

	
	// Create Chain
	chain  = new StChain("StChain");

	// create the StMuDstMaker
	StMuDstMaker *muDstMaker = new StMuDstMaker(  0, 0, "", fileList, "MuDst.root", maxFiles );

	StSpectraPicoMaker *rcpPico = new StSpectraPicoMaker("spectraPico", ("pico_" + string(name) ).c_str() );

	// Initialize chain
	Int_t iInit = chain->Init();
	if (iInit) 
		chain->Fatal(iInit,"on init");
	
	chain->PrintInfo();


	bool good = true;
	int iEvent = 0;
	while ( good ){

		if ( iEvent >= maxEvents )
			break;

		chain->Clear();
		int istat = 0;
		istat = chain->Make(iEvent);
		if (istat == 2) {
			cout << "Last  event processed. Status = " << istat << endl;
			break;
		}
		if (istat == 3) {
			cout << "Error event processed. Status = " << istat << endl;
		}

		if ( iEvent % 1000 == 0 ) {
			cout << " Event " << iEvent << " Complete" << endl;
		}

		iEvent++;
	}

	// Clean up
	if (iEvent >= 1) {
		chain->Finish();
		delete chain;
	}

}



