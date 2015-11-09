#include "StRcpSkimmer.h"

//// StRoot
// StMuDst
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"

#include "StMemoryInfo.hh"
#include "StMessMgr.h"
#include "StTimer.hh"


/**
 * Processes the MuDst
 */
void StRcpSkimmer::processMuDst() {
	//LOG_INFO << "StRcpSkimmer::processMuDst()" << endm;

	StMuDstMaker *muDstMaker = (StMuDstMaker*) GetMaker( "MuDst" );

  	if( !muDstMaker ) {
		LOG_WARN << "WARNING: No MuDstMaker" << endm;
		return;
  	}
  	muDst = muDstMaker->muDst();
  	if(!muDst) {
		LOG_WARN << "WARNING: No MuDst" << endm;
		return;
  	}

	StTimer timer;

  	if (doPrintCpuInfo) 
  		timer.start();
  	if (doPrintMemoryInfo)
  		StMemoryInfo::instance()->snapshot();

 
 	// Event selection
  	if ( !keepEvent() )
  		return;

  	StMuEvent *muEvent = muDst->event();

  	// Primary vertex
	StThreeVectorD pVtx(-999., -999., -999.);  
	pVtx = muDst->primaryVertex()->position();        

	/**
	 * Loop over primary tracks
	 */
	UInt_t nPrimary = muDst->primaryTracks()->GetEntries();
	nPrimary = muDst->numberOfPrimaryTracks();
	
	Int_t nPrimaryGood = 0;
	for (int iNode = 0; iNode < nPrimary; iNode++ ){
		
		StMuTrack*	tPrimary 	= (StMuTrack*)muDst->primaryTracks(iNode);
		if ( !tPrimary ) continue;
		StMuTrack*	tGlobal 	= (StMuTrack*)tPrimary->globalTrack();

		if ( !keepTrack( iNode ) )
			continue;

		analyzeTrack( iNode, nPrimaryGood );

		nPrimaryGood++;
	}

	postTrackLoop( nPrimaryGood );


	//- debug info`
	if (doPrintMemoryInfo) {
		StMemoryInfo::instance()->snapshot();
		StMemoryInfo::instance()->print();
	}
	if (doPrintCpuInfo) {
		timer.stop();
		LOG_INFO << "CPU time for StEventMaker::Make(): " << timer.elapsedTime() << " sec\n" << endm;
	}


}

void StRcpSkimmer::postTrackLoop( Int_t nPrimaryGoodTracks ){


}


void StRcpSkimmer::analyzeTrack( Int_t iNode, Int_t iGoodTrack ){
	//LOG_INFO << "StRcpSkimmer::analyzeTrack" << endm;
}

Int_t StRcpSkimmer::nTofMatchedTracksA(){

	Int_t nPrimary 	= muDst->primaryTracks()->GetEntries();
	Int_t nTofMatched = 0;
	for (int iNode = 0; iNode < nPrimary; iNode++ ){
		StMuTrack*	tPrimary 	= (StMuTrack*)muDst->primaryTracks(iNode);
		StMuTrack*	tGlobal 	= (StMuTrack*)tPrimary->globalTrack();

		if ( !tPrimary ) continue;
		if ( tPrimary->vertexIndex() != 0 ) continue;

		/**
		 * Get Tof info
		 */
		StMuBTofPidTraits bTofPidTraits	= tPrimary->btofPidTraits();

		if ( bTofPidTraits.matchFlag() > 0 ) 
			nTofMatched++;
	}

	return nTofMatched;

}


/**
 * Called before main event cuts but after trigger selection
 */
void StRcpSkimmer::preEventCuts(){
	//LOG_INFO << "StRcpSkimmer::preEventCuts" << endm;

}

/**
 * Called after main event cuts
 */
void StRcpSkimmer::postEventCuts(){
	//LOG_INFO << "StRcpSkimmer::postEventCuts" << endm;


}

void StRcpSkimmer::passEventCut( string name ){
	
}

/**
 * Apply Event cuts
 * @return [description]
 */
Bool_t StRcpSkimmer::keepEvent(){
	//LOG_INFO << "StRcpSkimmer::keepEvent" << endm;

	passEventCut( "All");
	
	StMuEvent *muEvent = muDst->event();
	if ( !muEvent )
		return false;

	passEventCut( "MuDstEvent" );

	//-- read in TOF info
	StBTofHeader* tofHeader = muDst->btofHeader();

	//BBC_MB
	// 15GeV
	Bool_t isTrigger = 
			( 	muEvent->triggerIdCollection().nominal().isTrigger(440005) ||	// BBC_MB but not used
				muEvent->triggerIdCollection().nominal().isTrigger(440015) ||	// BBC_MB
				muEvent->triggerIdCollection().nominal().isTrigger(440001) || 	// VPD_MB
				muEvent->triggerIdCollection().nominal().isTrigger(440004) ); 	// ZDC_MB
	
	// MB
	// 11 GeV
	// Bool_t isTrigger = 
	// 		( 	muEvent->triggerIdCollection().nominal().isTrigger(310014) );
	
	// MB
	// 19 GeV
	// Bool_t isTrigger = 
	// 		( 	muEvent->triggerIdCollection().nominal().isTrigger(340011) || 
	//  			muEvent->triggerIdCollection().nominal().isTrigger(340021) || 
	//  			muEvent->triggerIdCollection().nominal().isTrigger(340001) ||
	//  			muEvent->triggerIdCollection().nominal().isTrigger(340002) ||
	//  			muEvent->triggerIdCollection().nominal().isTrigger(340061) ||
	//  			muEvent->triggerIdCollection().nominal().isTrigger(340060) );
	
	

	if ( !isTrigger )
		return false;
	passEventCut( "Trigger" );

	// if ( badRunMap.find( runId ) != badRunMap.end() && badRunMap[ runId ] == true ){
	// 	//cout << runId << " is BAD " << endl << endm;
	// 	return false;
	// }
	// passEventCut( "BadRun" );


	StThreeVectorD pVtx(-999., -999., -999.);  
	if( !muDst->primaryVertex() ) {
		//LOG_INFO << "No Primary Vertex" << endm;
		return false;
	}
	passEventCut( "VertexExists" );

	nTofMatchedTracks = nTofMatchedTracksA();
	pVtx = muDst->primaryVertex()->position();

	// Initialize the refMult Corr
	refmultCorrUtil->init( muEvent->runId() );
	refmultCorrUtil->initEvent( muEvent->refMult(), pVtx.z() );

	corrRefMult 	= (Float_t)refmultCorrUtil->getRefMultCorr();
	cent9  			= refmultCorrUtil->getCentralityBin9();
	cent16  		= refmultCorrUtil->getCentralityBin16();
	eventWeight  	= refmultCorrUtil->getWeight();


	runId = muEvent->runId();
	
	//cout << " testing bad run : 15070010 " << ( badRunMap.find( 15070010 ) != badRunMap.end() && badRunMap[ 15070010 ] == true ) << endl << endm;
	

	// The Pre event cuts hook
	preEventCuts();

	

	// Initialize the vertex	
	// float xOffset = - 0.09797;		// 11GeV
	// float yOffset = 0.2274;			// 11GeV
	// float xOffset = - 0.3685;		// 19GeV
	// float yOffset = 0.03097;			// 19GeV
	float xOffset = 0.0;			// 15GeV
	float yOffset = 0.89;			// 15GeV

	pX = pVtx.x() + xOffset;
	pY = pVtx.y() + yOffset;
	pZ = pVtx.z();


	// dont need to keep the events in 80-100% centrality range
	if ( cent9 < 0 )
		return false;


	if ( pVtx.z() > cut_vZ->max  || pVtx.z() < cut_vZ->min)
		return false;
	passEventCut( "vZ");

	

	if ( TMath::Sqrt( pX*pX + pY*pY ) > cut_vR->max )
		return false;
	passEventCut( "vR");

		
	if ( nTofMatchedTracks < cut_nTofMatch->min )
		return false;
	passEventCut( "nTofMatch");

	// The Post event cuts hook
	postEventCuts();

	return true;

}



void StRcpSkimmer::passTrackCut( string name ){

}

void StRcpSkimmer::preTrackCuts(StMuTrack *primaryTrack ){

}
void StRcpSkimmer::postTrackCuts(StMuTrack *primaryTrack ){
	//LOG_INFO << "StRcpSkimmer::postTrackCuts" << endm;
}

/**
 * Apply Track Cuts
 * @param  		iNode - Primary Track index
 * @return      true - keep track
 */
Bool_t StRcpSkimmer::keepTrack( Int_t iNode ){
	//LOG_INFO << "StRcpSkimmer::keepTrack( " << iNode << " )" << endm;

	StMuTrack*	tPrimary 	= (StMuTrack*)muDst->primaryTracks(iNode);
	passTrackCut("All");

	const StMuTrack *globalTrack = tPrimary->globalTrack();
	if (!globalTrack ) 
		return false;

	passTrackCut( "primaryWGlobal");

	StThreeVectorF pMom = tPrimary->momentum();
	StThreeVectorF gMom = globalTrack->momentum();
	float ptRatio = gMom.perp() / pMom.perp();

	

	/**
	 * Pre Cut Hook
	 */
	preTrackCuts( tPrimary );

	if ( globalTrack->nHitsFit(kTpcId) < cut_nHitsFit->min  )
		return false;
	passTrackCut( "nHitsFit" );

	if ( (float)globalTrack->nHitsFit(kTpcId) / globalTrack->nHitsPoss() < cut_nHitsRatio->min )
		return false;
	passTrackCut( "nHitsRatio" );

	if ( globalTrack->nHitsDedx() < cut_nHitsDedx->min )
		return false;	
	passTrackCut( "nHitsDedx" );

	if ( ptRatio < cut_ptRatio->min || ptRatio > cut_ptRatio->max )
		return false;
	passTrackCut( "ptRatio" );

	if ( pMom.perp() < cut_pt->min )
		return false;
	passTrackCut( "pmmtm" );

	if ( tPrimary->dcaGlobal().magnitude() > cut_dca->max )
		return false;
	passTrackCut( "DCA" );


	/**
	 * Post Cut Hook
	 */
	postTrackCuts( tPrimary );

	return true;

}






ClassImp(StRcpSkimmer)

/**
 * Creates the Maker
 */
StRcpSkimmer::StRcpSkimmer( const Char_t *name="rcpSkimmer", const Char_t *outname="rcp.skim.root") : StMaker(name) {
	// the output filename
	mTupleFileName = outname;

	// default debug options
	doPrintMemoryInfo = kFALSE;
	doPrintCpuInfo    = kFALSE;

	// load up the cuts from an xml config
	cfgCuts = NULL;
	cfgCuts = new jdb::XmlConfig( "./cuts.xml" );

	// make the cuts variables
	cut_vZ = new jdb::ConfigRange( cfgCuts, "Event.zVertex" );
	cut_vR = new jdb::ConfigRange( cfgCuts, "Event.rVertex" );
	cut_nTofMatch = new jdb::ConfigRange( cfgCuts, "Event.nTofMatch" );

	cut_nHitsFit = new jdb::ConfigRange( cfgCuts, "Track.nHitsFit" );
	cut_nHitsDedx = new jdb::ConfigRange( cfgCuts, "Track.nHitsDedx" );
	cut_nHitsRatio = new jdb::ConfigRange( cfgCuts, "Track.nHitsRatio" );

	cut_pt = new jdb::ConfigRange( cfgCuts, "Track.pt" );
	cut_ptRatio = new jdb::ConfigRange( cfgCuts, "Track.ptRatio" );
	cut_dca = new jdb::ConfigRange( cfgCuts, "Track.dca" );
	cut_yLocal = new jdb::ConfigRange( cfgCuts, "Track.yLocal" );
	cut_zLocal = new jdb::ConfigRange( cfgCuts, "Track.zLocal" );

	// report
	cout << endl << name << endl << endm;
	cout << "Event Cuts \n\n" << endm;
	cout << "Z Vertex : ( " << cut_vZ->min  << ", " << cut_vZ->max << " )\n" << endm;
	cout << "R Vertex : ( " << cut_vR->min  << ", " << cut_vR->max << " )\n" << endm;
	cout << "nTofMatch : ( " << cut_nTofMatch->min  << ", " << cut_nTofMatch->max << " )\n" << endm;

	cout << "Track Cuts \n\n" << endm;
	cout << "nHitsFit : ( " << cut_nHitsFit->min  << ", " << cut_nHitsFit->max << " )\n" << endm;
	cout << "nHitsDedx : ( " << cut_nHitsDedx->min  << ", " << cut_nHitsDedx->max << " )\n" << endm;
	cout << "nHitsRatio : ( " << cut_nHitsRatio->min  << ", " << cut_nHitsRatio->max << " )\n" << endm;
	cout << "pt : (" << cut_pt->min << "," << cut_pt->max << " ) \n" << endm;
	cout << "ptRatio : (" << cut_ptRatio->min << "," << cut_ptRatio->max << " ) \n" << endm;
	cout << "dca : (" << cut_dca->min << "," << cut_dca->max << " ) \n" << endm;
	cout << "yLocal : (" << cut_yLocal->min << "," << cut_yLocal->max << " ) \n" << endm;
	cout << "zLocal : (" << cut_zLocal->min << "," << cut_zLocal->max << " ) \n" << endm;


	// create the bad run map - sanity check - first attempt at bad run filtering wasn't working as expected
	for ( int i = 0; i < nBadRuns; i++ ){
		badRunMap[ badRuns[ i ] ] = true;
	}

}

/**
 * Destructor
 */
StRcpSkimmer::~StRcpSkimmer( ){ 

	if ( NULL != cfgCuts )
		delete cfgCuts;
	cfgCuts = NULL;

}

/**
 * Initialise the Maker
 */
Int_t StRcpSkimmer::Init( ){

	refmultCorrUtil  = CentralityMaker::instance()->getRefMultCorr();




	return kStOK;
}


Int_t StRcpSkimmer::InitRun( int runnumber ) {

	return kStOK;
}

Int_t StRcpSkimmer::FinishRun( int runnumber ) {
  	return kStOK;
}

/// write and close the ntuple file
Int_t StRcpSkimmer::Finish() {

	if ( (string)"" != mTupleFileName && mTupleFile ){
		mTupleFile->Write();
		mTupleFile->Close();

		LOG_INFO  << "StRcpSkimmer::Finish() ntuple file " << mTupleFileName  << " closed." << endm;

	}

	return kStOK;
}


/**
 * Called for each event 
 */
Int_t StRcpSkimmer::Make(){

  processMuDst();

  return kStOK;
}




int StRcpSkimmer::nBadRuns = 313;
int StRcpSkimmer::badRuns[] = { 
15046073, 
15046089, 
15046094, 
15046096, 
15046102, 
15046103, 
15046104, 
15046105, 
15046106, 
15046107, 
15046108, 
15046109, 
15046110, 
15046111, 
15047004, 
15047015, 
15047016, 
15047019, 
15047021, 
15047023, 
15047024, 
15047026, 
15047027, 
15047028, 
15047029, 
15047030, 
15047039, 
15047040, 
15047041, 
15047044, 
15047047, 
15047050, 
15047052, 
15047053, 
15047056, 
15047057, 
15047061, 
15047062, 
15047063, 
15047064, 
15047065, 
15047068, 
15047069, 
15047070, 
15047071, 
15047072, 
15047074, 
15047075, 
15047082, 
15047085, 
15047086, 
15047087, 
15047093, 
15047096, 
15047097, 
15047098, 
15047100, 
15047102, 
15047104, 
15047106, 
15048003, 
15048004, 
15048012, 
15048013, 
15048014, 
15048016, 
15048017, 
15048018, 
15048019, 
15048020, 
15048021, 
15048023, 
15048024, 
15048025, 
15048026, 
15048028, 
15048029, 
15048030, 
15048031, 
15048033, 
15048034, 
15048074, 
15048075, 
15048076, 
15048077, 
15048078, 
15048079, 
15048080, 
15048081, 
15048082, 
15048083, 
15048084, 
15048085, 
15048086, 
15048087, 
15048088, 
15048089, 
15048091, 
15048092, 
15048093, 
15048094, 
15048095, 
15048096, 
15048097, 
15048098, 
15049002, 
15049003, 
15049009, 
15049013, 
15049014, 
15049015, 
15049016, 
15049017, 
15049018, 
15049019, 
15049020, 
15049021, 
15049022, 
15049023, 
15049025, 
15049026, 
15049027, 
15049028, 
15049030, 
15049031, 
15049032, 
15049033, 
15049037, 
15049038, 
15049039, 
15049040, 
15049041, 
15049074, 
15049077, 
15049083, 
15049084, 
15049085, 
15049086, 
15049087, 
15049088, 
15049089, 
15049090, 
15049091, 
15049092, 
15049093, 
15049094, 
15049096, 
15049097, 
15049098, 
15049099, 
15050001, 
15050002, 
15050003, 
15050004, 
15050005, 
15050006, 
15050010, 
15050011, 
15050012, 
15050013, 
15050014, 
15050015, 
15050016, 
15051131, 
15051132, 
15051133, 
15051134, 
15051137, 
15051141, 
15051144, 
15051146, 
15051147, 
15051148, 
15051149, 
15051156, 
15051157, 
15051159, 
15051160, 
15052001, 
15052004, 
15052005, 
15052006, 
15052007, 
15052008, 
15052009, 
15052010, 
15052011, 
15052014, 
15052015, 
15052016, 
15052017, 
15052018, 
15052019, 
15052020, 
15052021, 
15052022, 
15052023, 
15052024, 
15052025, 
15052026, 
15052040, 
15052041, 
15052042, 
15052043, 
15052060, 
15052061, 
15052062, 
15052063, 
15052064, 
15052065, 
15052066, 
15052067, 
15052068, 
15052069, 
15052070, 
15052073, 
15052074, 
15052075,  
15053027, 
15053028, 
15053029, 
15053034, 
15053035, 
15053052, 
15053054, 
15053055, 
15054053, 
15054054, 
15055018, 
15055137, 
15056117, 
15057055, 
15057059, 
15058006, 
15058011, 
15058021, 
15059057, 
15059058, 
15061001, 
15061009, 
15062006, 
15062069, 
15065012, 
15065014, 
15066070, 
15068013, 
15068014, 
15068016, 
15068018, 
15069036, 
15070008, 
15070009, 
15070010
};
