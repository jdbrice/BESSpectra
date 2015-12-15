#include "StSpectraMiniMcPicoMaker.h"

ClassImp(StSpectraMiniMcPicoMaker);


StSpectraMiniMcPicoMaker::StSpectraMiniMcPicoMaker(int _gid, string _inFile, string _suffix, string _trackType, int _maxFiles)
	: StSpectraMiniMcSkimmer( _gid, _inFile, _suffix, _trackType, _maxFiles ) {


	bookNtuples();
}


void StSpectraMiniMcPicoMaker::postEventCuts(){
	mData.runId 		= runId;
	mData.corrRefMult 	= corrRefMult;
	mData.weight 		= eventWeight;
	mData.bin9 			= cent9;
	mData.bin16 		= cent16;
}

void StSpectraMiniMcPicoMaker::analyzeTrackPair( StMiniMcPair * track, Int_t iGoodTrack ){
	//LOG_INFO << "StSpectraMiniMcPicoMaker::analyzeTrack( " << iNode << ", " << iGoodTrack << ") " << endm;


	mData.pP[ iGoodTrack ] 			= track->pPr();
	mData.pPt[ iGoodTrack ] 		= track->ptPr() * track->charge();
	mData.gPt[ iGoodTrack ] 		= track->ptGl();
	mData.pEta[ iGoodTrack ] 		= track->etaPr();

	mData.nHitsFit[ iGoodTrack ] 	= track->fitPts(); // use kTpcId ??
	mData.nHitsDedx[ iGoodTrack ] 	= track->dedxPts();
	mData.nHitsPossible[ iGoodTrack ] = track->nPossiblePts(); // use kTpcId ??

	mData.dca[ iGoodTrack ]			= (Float_t)track->dcaGl();

	
	mData.matchFlag[ iGoodTrack ]		= 1;
	
	mData.beta[ iGoodTrack ] 			= (UShort_t)(0.75 * 20000);
		
	mData.yLocal[ iGoodTrack ]			= track->ptMc();
	mData.zLocal[ iGoodTrack ]			= 0;

	
	mData.dedx[ iGoodTrack ] 			= (UShort_t) ((track->dedx()*1e6) * 1000);
}


void StSpectraMiniMcPicoMaker::analyzeMcTrack( StTinyMcTrack * track, Int_t iGoodTrack ){
	//cout << "StSpectraMiniMcPicoMaker::analyzeTrack( " << track << ", " << iGoodTrack << ") " << endl;


	mData.pP[ iGoodTrack ] 			= track->pMc();
	mData.pPt[ iGoodTrack ] 		= track->ptMc() * track->chargeMc();
	mData.gPt[ iGoodTrack ] 		= track->ptMc();
	mData.pEta[ iGoodTrack ] 		= track->etaMc();

	mData.nHitsFit[ iGoodTrack ] 	= 20; // use kTpcId ??
	mData.nHitsDedx[ iGoodTrack ] 	= 20;
	mData.nHitsPossible[ iGoodTrack ] = 21; // use kTpcId ??

	mData.dca[ iGoodTrack ]			= (Float_t)0;

	
	mData.matchFlag[ iGoodTrack ]	= 1;
	
	mData.beta[ iGoodTrack ] 		= (UShort_t)(0.75 * 20000);
		
	mData.yLocal[ iGoodTrack ]		= 0;
	mData.zLocal[ iGoodTrack ]		= 0;

	
	mData.dedx[ iGoodTrack ] 		= (UShort_t)10;
}

void StSpectraMiniMcPicoMaker::postTrackLoop(Int_t nPrimaryGood){
	//cout << "StSpectraMiniMcPicoMaker::postTrackLoop( " << nPrimaryGood << ") " << endl;
	
	mData.nTracks = nPrimaryGood;
	mTree->Fill();
}

/**
 * Initialize Tree Structure
 */
void StSpectraMiniMcPicoMaker::bookNtuples(){


	mTree = new TTree("rcpPicoDst","Rcp Pid Spectra Data");
	mTree->SetAutoSave(10000);
	mTree->Branch("runId",				&mData.runId,"run/I");
	mTree->Branch("bin16",				&mData.bin16,"bin16/s");
	mTree->Branch("bin9",				&mData.bin9,"bin9/s");
	mTree->Branch("weight",				&mData.weight,"weight/F");
	mTree->Branch("corrRefMult",		&mData.corrRefMult,"corrRefMult/F");
	mTree->Branch("nTracks",			&mData.nTracks,"nTracks/I");

	mTree->Branch("ppT",				mData.pPt,	"ppT[nTracks]/F");
	mTree->Branch("gpT",				mData.gPt,	"gpT[nTracks]/F");
	mTree->Branch("pP",					mData.pP,	"pP[nTracks]/F");
	mTree->Branch("pEta",				mData.pEta,	"pEta[nTracks]/F");

	mTree->Branch("dca", 				mData.dca, "dca[nTracks]/F");

	mTree->Branch("nHitsFit", 			mData.nHitsFit, "nHitsFit[nTracks]/b");
	mTree->Branch("nHitsDedx", 			mData.nHitsDedx, "nHitsDedx[nTracks]/b");
	mTree->Branch("nHitsPossible", 		mData.nHitsPossible, "nHitsPossible[nTracks]/b");

	mTree->Branch("dedx",				mData.dedx,	"dedx[nTracks]/s");
	mTree->Branch("beta",				mData.beta,	"beta[nTracks]/s");
	
	mTree->Branch("matchFlag",			mData.matchFlag, "matchFlag[nTracks]/b");
	mTree->Branch("yLocal",				mData.yLocal,	"yLocal[nTracks]/F");
	mTree->Branch("zLocal",				mData.zLocal,	"zLocal[nTracks]/F");

	return;
}


/**
 * Destructor
 */
StSpectraMiniMcPicoMaker::~StSpectraMiniMcPicoMaker( ){ 

}
