#include "StSpectraPicoMaker/StSpectraPicoMaker.h"


#include "StMuDSTMaker/COMMON/StMuUtilities.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"

ClassImp(StSpectraPicoMaker);


void StSpectraPicoMaker::postEventCuts(){
	mData.runId 		= runId;
	mData.corrRefMult 	= corrRefMult;
	mData.weight 		= eventWeight;
	mData.bin9 			= cent9;
	mData.bin16 		= cent16;
}

void StSpectraPicoMaker::analyzeTrack( Int_t iNode, Int_t iGoodTrack ){
	//LOG_INFO << "StSpectraPicoMaker::analyzeTrack( " << iNode << ", " << iGoodTrack << ") " << endm;

	StMuTrack*	tPrimary 			= (StMuTrack*)muDst->primaryTracks(iNode);	
	StMuTrack*	tGlobal 			= (StMuTrack*)tPrimary->globalTrack();
	StMuBTofPidTraits tofpid 		= tGlobal->btofPidTraits();

	// Momentum
	StThreeVectorF pMom 			= tPrimary->momentum();
	StThreeVectorF gMom 			= tGlobal->momentum();

	mData.pP[ iGoodTrack ] 			= pMom.magnitude();
	mData.pPt[ iGoodTrack ] 		= pMom.perp() * tPrimary->charge();
	mData.gPt[ iGoodTrack ] 		= gMom.perp();
	mData.pEta[ iGoodTrack ] 		= pMom.pseudoRapidity();

	mData.nHitsFit[ iGoodTrack ] 	= tPrimary->nHitsFit(kTpcId); 
	mData.nHitsDedx[ iGoodTrack ] 	= tPrimary->nHitsDedx();
	mData.nHitsPossible[ iGoodTrack ] = tGlobal->nHitsPoss(kTpcId);

	mData.dca[ iGoodTrack ]			= (Float_t)(tPrimary->dcaGlobal().magnitude() );

	// get the tof pid
	StMuBTofPidTraits tofPid = tGlobal->btofPidTraits();
	StMuBTofPidTraits tofPid2 = tPrimary->btofPidTraits();

	if ( tofPid.beta() != tofPid2.beta() ){
		LOG_INFO << "tof1 = " << tofPid.beta() << ", tof2 = " << tofPid2.beta() << endm;
	}


	mData.matchFlag[ iGoodTrack ]	= tofPid.matchFlag();
	// defaults
	mData.beta[ iGoodTrack ] 		= 0;
	mData.yLocal[ iGoodTrack ]		= 0;
	mData.zLocal[ iGoodTrack ]		= 0;

	if ( tofPid.matchFlag() >= 1 ){
		// PID variables
		if ( tofpid.beta() > -800 )
			mData.beta[ iGoodTrack ] 		= (UShort_t)(tofpid.beta() * 20000);
		else
			mData.beta[ iGoodTrack ] 		= (UShort_t)0;

		
		mData.yLocal[ iGoodTrack ]			= tofPid.yLocal();
		mData.zLocal[ iGoodTrack ]			= tofPid.zLocal();
		
			
	}

	
	mData.dedx[ iGoodTrack ] 			= (UShort_t) ((tGlobal->dEdx()*1e6) * 1000);
}

void StSpectraPicoMaker::postTrackLoop(Int_t nPrimaryGood){
	//LOG_INFO << "StSpectraPicoMaker::postTrackLoop( " << nPrimaryGood << ") " << endm;
	mData.nTracks = nPrimaryGood;
	mTree->Fill();
}

/**
 * Initialize Tree Structure
 */
void StSpectraPicoMaker::bookNtuples(){


	mTupleFile = new TFile(mTupleFileName.c_str(), "RECREATE");

	mTree = new TTree("SpectraPicoDst","Pid Spectra Data");
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
 * Creates the Maker
 */
StSpectraPicoMaker::StSpectraPicoMaker( const Char_t *name="SpectraPicoMaker", const Char_t *outname="SpectraPico.root") : StSpectraSkimmer(name, outname) {
	
}

/**
 * Destructor
 */
StSpectraPicoMaker::~StSpectraPicoMaker( ){ 

}

/**
 * Initializes the Maker
 */
Int_t StSpectraPicoMaker::Init( ){

	StSpectraSkimmer::Init();

	if ( (string)"" != mTupleFileName ) 
		bookNtuples();

	return kStOK;
}
