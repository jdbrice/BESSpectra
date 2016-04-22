#include "StSpectraSimPicoMaker/StSpectraSimPicoMaker.h"


#include "StMuDSTMaker/COMMON/StMuUtilities.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"

ClassImp(StSpectraSimPicoMaker);


void StSpectraSimPicoMaker::postEventCuts(){
	mData.runId 			= 15055000;
	mData.corrRefMult 		= 5.0;
	mData.weight 			= getVar( "eventWeight" );
	mData.bin9 				= (int)getVar( "cent9" );
	mData.bin16 			= -1;
}

void StSpectraSimPicoMaker::analyzeTrack( Int_t iGoodTrack ){
	
	
	mData.pPt[ iGoodTrack ] 			= getVar( "pPt" );
	mData.gPt[ iGoodTrack ] 			= mData.pPt[ iGoodTrack ] + getVar( "gPt" );
	mData.pEta[ iGoodTrack ] 			= getVar( "eta" );

	mData.pP[ iGoodTrack ] 				= mData.pPt[ iGoodTrack ] * cosh( mData.pEta[ iGoodTrack ] );

	mData.nHitsFit[ iGoodTrack ] 		= getVar( "nHitsFit" );
	mData.nHitsDedx[ iGoodTrack ] 		= getVar( "nHitsDedx" );
	mData.nHitsPossible[ iGoodTrack ] 	= getVar( "nHitsPossible" );

	mData.dca[ iGoodTrack ]			= getVar( "dca" );


	double pidMass = getVar( "pidMass" );
	map<int, double> masses;
	masses[0] = 0.1395702;
	masses[1] = 0.493667;
	masses[2] = 0.9382721;

	pidMass = masses[ (int)pidMass ];

	mData.matchFlag[ iGoodTrack ]	= getVar( "matchFlag" );
	// // defaults
	mData.beta[ iGoodTrack ] 		= (UShort_t)((1.0/zbGen->random( mData.pP[ iGoodTrack ], pidMass )) * 20000);
	mData.yLocal[ iGoodTrack ]		= getVar( "yLocal" );
	mData.zLocal[ iGoodTrack ]		= getVar( "yLocal" );
	mData.dedx[ iGoodTrack ] 		= (UShort_t) ((zdGen->random(mData.pP[ iGoodTrack ], pidMass) *1e3) * 1000);
}

void StSpectraSimPicoMaker::postTrackLoop(Int_t nPrimaryGood){
	//LOG_INFO << "StSpectraSimPicoMaker::postTrackLoop( " << nPrimaryGood << ") " << endm;
	mData.nTracks = nPrimaryGood;
	mTree->Fill();
}

/**
 * Initialize Tree Structure
 */
void StSpectraSimPicoMaker::bookNtuples(){


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
StSpectraSimPicoMaker::StSpectraSimPicoMaker( const Char_t *name="SpectraPicoMaker", const Char_t *outname="SpectraPico.root") {
	mTupleFileName = outname;
}

/**
 * Destructor
 */
StSpectraSimPicoMaker::~StSpectraSimPicoMaker( ){ 

}



Int_t StSpectraSimPicoMaker::Init( ){
	LOG_INFO << "StSpectraSimPicoMaker::Init" << endl << endm;
	
	LOG_INFO << "filename " << mTupleFileName << endm;
	if ( (string)"" != mTupleFileName ) {
		LOG_INFO << "Booking NTuples in " << mTupleFileName << endm;
		bookNtuples();
	}


	XmlConfig *cfg = new XmlConfig( "distros.xml" );
	vector<string> kids = cfg->childrenOf( "Vars" );

	LOG_INFO << "n distros " << kids.size() << endl << endm;
	for ( int i = 0; i < kids.size(); i++ ){
		LOG_INFO << cfg->tagName( kids[i] ) << " ===> " << cfg->getString( kids[i] + ":formula" ) << endm;
		XmlFunction * xmld = new XmlFunction( cfg, kids[ i ] );
		vars[ cfg->tagName( kids[i] ) ] = xmld;


	}

	delete cfg;


	zbGen = new tofGenerator(  );
	zdGen = new dedxGenerator( );

	return kStOK;
}


Int_t StSpectraSimPicoMaker::InitRun( int runId ) {
	LOG_INFO << "Run # " << runId << endm;

	
	return kStOK;
}

Int_t StSpectraSimPicoMaker::FinishRun( int runnumber ) {
  	return kStOK;
}

/// write and close the ntuple file
Int_t StSpectraSimPicoMaker::Finish() {

	if ( (string)"" != mTupleFileName && mTupleFile ){
		mTupleFile->Write();
		mTupleFile->Close();

		LOG_INFO  << "StSpectraSimPicoMaker::Finish() ntuple file " << mTupleFileName  << " closed." << endm;

	}

	return kStOK;
}


/**
 * Called for each event 
 */
Int_t StSpectraSimPicoMaker::Make(){
	// LOG_INFO << "MAKE" << endl << endm;

	// efficiently skip bad runs
	// if ( !skipRun ){

  	// analyzeTrack();
  	// LOG_INFO << "Primary pT = " << getRandomVar( "PrimaryPt" ) << endm;

	postEventCuts();
	int nPrimaryGood = 0;

	
	int totalTracks = (int) getVar( "nTracks" );
	for ( int i = 0; i < totalTracks; i++ ){

		analyzeTrack( nPrimaryGood );
		nPrimaryGood++;
	}

	postTrackLoop( nPrimaryGood );

  	return kStOK;
}
