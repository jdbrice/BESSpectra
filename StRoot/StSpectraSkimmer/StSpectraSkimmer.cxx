#include "StSpectraSkimmer/StSpectraSkimmer.h"

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
void StSpectraSkimmer::processMuDst() {
	//LOG_INFO << "StSpectraSkimmer::processMuDst()" << endm;

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
	LOG_INFO << "# of Primary Tracks = " << nPrimary << endl << endm;
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
	LOG_INFO << "nPrimaryGood #1" << nPrimaryGood << endl << endm;

	nPrimaryGood = 0;
	TObjArray* tracks = muDst->primaryTracks() ;      // Create a TObject array containing the primary tracks
  	TObjArrayIter  GetTracks(tracks) ;                        // Create an iterator to step through the tracks
  	StMuTrack*                 track ;                        // Pointer to a track
  
  
  
  	Int_t nAllPrimary = 0;
  	while((track = (StMuTrack*)GetTracks.Next() ) ) {
  		nAllPrimary++;
    }


    LOG_INFO << "nPrimary" << nPrimary << "nIterator" << nAllPrimary << endl << endm;





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

void StSpectraSkimmer::postTrackLoop( Int_t nPrimaryGoodTracks ){
}


void StSpectraSkimmer::analyzeTrack( Int_t iNode, Int_t iGoodTrack ){
	//LOG_INFO << "StSpectraSkimmer::analyzeTrack" << endm;
}

Int_t StSpectraSkimmer::nTofMatchedTracksA(){

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
void StSpectraSkimmer::preEventCuts(){
	//LOG_INFO << "StSpectraSkimmer::preEventCuts" << endm;

}

/**
 * Called after main event cuts
 */
void StSpectraSkimmer::postEventCuts(){
	//LOG_INFO << "StSpectraSkimmer::postEventCuts" << endm;
}

void StSpectraSkimmer::passEventCut( string name ){
}

/**
 * Apply Event cuts
 * @return [description]
 */
Bool_t StSpectraSkimmer::keepEvent(){
	//LOG_INFO << "StSpectraSkimmer::keepEvent" << endm;

	passEventCut( "All");
	
	StMuEvent *muEvent = muDst->event();
	if ( !muEvent )
		return false;

	passEventCut( "MuDstEvent" );

	//-- read in TOF info
	StBTofHeader* tofHeader = muDst->btofHeader();

	
	Bool_t isTrigger = false;
	for ( int t : triggers ){
		isTrigger = isTrigger || muEvent->triggerIdCollection().nominal().isTrigger( t );
	}

	if ( !isTrigger )
		return false;
	passEventCut( "Trigger" );

	if ( isRunBad( runId ) ){
		return false;
	}
	passEventCut( "BadRun" );


	StThreeVectorD pVtx(-999., -999., -999.);  
	if( !muDst->primaryVertex() ) {
		// LOG_INFO << "No Primary Vertex" << endl << endm;
		// LOG_INFO << "MuDst N Primary : " << muDst->numberOfPrimaryVertices() << endl << endm;
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


	// The Pre event cuts hook
	preEventCuts();


	pX = pVtx.x() + cut_vR_offset->x;
	pY = pVtx.y() + cut_vR_offset->y;
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



void StSpectraSkimmer::passTrackCut( string name ){

}

void StSpectraSkimmer::preTrackCuts(StMuTrack *primaryTrack ){

}
void StSpectraSkimmer::postTrackCuts(StMuTrack *primaryTrack ){
	//LOG_INFO << "StSpectraSkimmer::postTrackCuts" << endm;
}

/**
 * Apply Track Cuts
 * @param  		iNode - Primary Track index
 * @return      true - keep track
 */
Bool_t StSpectraSkimmer::keepTrack( Int_t iNode ){
	//LOG_INFO << "StSpectraSkimmer::keepTrack( " << iNode << " )" << endm;

	StMuTrack*	primaryTrack 	= (StMuTrack*)muDst->primaryTracks(iNode);
	passTrackCut("All");

	const StMuTrack *globalTrack = primaryTrack->globalTrack();
	if (!globalTrack ) 
		return false;

	passTrackCut( "primaryWGlobal");

	StThreeVectorF pMom = primaryTrack->momentum();
	StThreeVectorF gMom = globalTrack->momentum();
	float ptRatio = gMom.perp() / pMom.perp();

	

	/**
	 * Pre Cut Hook
	 */
	preTrackCuts( primaryTrack );

	if ( !cut_flag->inInclusiveRange( primaryTrack->flag() ) )
		return false;
	passTrackCut( "flag" );

	if ( primaryTrack->nHitsFit( kTpcId ) < cut_nHitsFit->min  )
		return false;
	passTrackCut( "nHitsFit" );

	if ( (float)primaryTrack->nHitsFit(kTpcId) / (float)primaryTrack->nHitsPoss(kTpcId) < cut_nHitsRatio->min )
		return false;
	passTrackCut( "nHitsRatio" );

	if ( primaryTrack->nHitsDedx() < cut_nHitsDedx->min )
		return false;	
	passTrackCut( "nHitsDedx" );

	if ( ptRatio < cut_ptRatio->min || ptRatio > cut_ptRatio->max )
		return false;
	passTrackCut( "ptRatio" );

	if ( pMom.perp() < cut_pt->min )
		return false;
	passTrackCut( "pmmtm" );

	if ( primaryTrack->dcaGlobal().magnitude() > cut_dca->max )
		return false;
	passTrackCut( "DCA" );

	double y = rapidity( pMom.perp(), pMom.pseudoRapidity(), massAssumption );
	if ( y < cut_rapidity->min || y > cut_rapidity->max )
		return false;
	passTrackCut( "y" );

	double eta = pMom.pseudoRapidity();
	if ( eta < cut_pseudorapidity->min || eta > cut_pseudorapidity->max )
		return false;
	passTrackCut( "eta" );


	/**
	 * Post Cut Hook
	 */
	postTrackCuts( primaryTrack );

	return true;

}


ClassImp(StSpectraSkimmer)

/**
 * Creates the Maker
 */
StSpectraSkimmer::StSpectraSkimmer( const Char_t *name="rcpSkimmer", const Char_t *outname="rcp.skim.root") : StMaker(name) {
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
	cut_vR_offset = new jdb::ConfigPoint( cfgCuts, "Event.rVertexOffset" );

	cut_nTofMatch = new jdb::ConfigRange( cfgCuts, "Event.nTofMatch" );

	cut_flag = new jdb::ConfigRange( cfgCuts, "Track.flag" );
	cut_nHitsFit = new jdb::ConfigRange( cfgCuts, "Track.nHitsFit" );
	cut_nHitsDedx = new jdb::ConfigRange( cfgCuts, "Track.nHitsDedx" );
	cut_nHitsRatio = new jdb::ConfigRange( cfgCuts, "Track.nHitsRatio" );

	cut_pt = new jdb::ConfigRange( cfgCuts, "Track.pt" );
	cut_ptRatio = new jdb::ConfigRange( cfgCuts, "Track.ptRatio" );
	cut_dca = new jdb::ConfigRange( cfgCuts, "Track.dca" );
	cut_yLocal = new jdb::ConfigRange( cfgCuts, "Track.yLocal" );
	cut_zLocal = new jdb::ConfigRange( cfgCuts, "Track.zLocal" );
	cut_rapidity = new jdb::ConfigRange( cfgCuts, "Track.rapidity", -10000, 10000 );
	cut_pseudorapidity = new jdb::ConfigRange( cfgCuts, "Track.pseudorapidity", -10000, 10000 );
	massAssumption = cfgCuts->getDouble( "Track.rapidity:mass", 0.13957018 /*pi +/- mass in GeV/c */);

	triggers = cfgCuts->getIntVector( "Event.triggers" );
	badRuns = cfgCuts->getIntVector( "BadRuns" );
	runRange = new jdb::ConfigRange( cfgCuts, "RunRange" );
	// build the bad run map
	for ( int rId : badRuns ){
		badRunMap[ rId ] = true;
	}


	string cfgReport = cfgCuts->report();

	LOG_DEBUG << "Config Report : \n " << cfgReport << endm;

	LOG_INFO << "Processing Runs between " << runRange->min << " --> " << runRange->max << " (inclusive) " << endm; 
	LOG_INFO << "Excluding " << badRuns.size() << " bad runs " << endm;
	stringstream brs;
	for ( int i : badRuns ){
		brs << i << ", ";
	}
	LOG_INFO << brs.str() << endm << endm;

	// report
	LOG_INFO << endl << name << endl << endm;
	for ( int i = 0; i < triggers.size(); i++ ){
		LOG_INFO << "Trigger : " << triggers[ i ] << endm;
	}
	LOG_INFO << endl << name << endl << endm;
	LOG_INFO << "Event Cuts \n\n" << endm;
	LOG_INFO << "Z Vertex : ( " << cut_vZ->min  << ", " << cut_vZ->max << " )" << endm;
	LOG_INFO << "R Vertex : ( " << cut_vR->min  << ", " << cut_vR->max << " )" << endm;
	LOG_INFO << "R Vertex Offset (added): ( " << cut_vR_offset->x  << ", " << cut_vR_offset->y << " )" << endm;
	LOG_INFO << "nTofMatch : ( " << cut_nTofMatch->min  << ", " << cut_nTofMatch->max << " )" << endm;

	LOG_INFO << "Track Cuts \n\n" << endm;
	LOG_INFO << "flag : ( " << cut_flag->min  << ", " << cut_flag->max << " )" << endm;
	LOG_INFO << "nHitsFit : ( " << cut_nHitsFit->min  << ", " << cut_nHitsFit->max << " )" << endm;
	LOG_INFO << "nHitsDedx : ( " << cut_nHitsDedx->min  << ", " << cut_nHitsDedx->max << " )" << endm;
	LOG_INFO << "nHitsRatio : ( " << cut_nHitsRatio->min  << ", " << cut_nHitsRatio->max << " )" << endm;
	LOG_INFO << "pt : (" << cut_pt->min << "," << cut_pt->max << " ) " << endm;
	LOG_INFO << "ptRatio : (" << cut_ptRatio->min << "," << cut_ptRatio->max << " ) " << endm;
	LOG_INFO << "dca : (" << cut_dca->min << "," << cut_dca->max << " ) " << endm;
	LOG_INFO << "yLocal : (" << cut_yLocal->min << "," << cut_yLocal->max << " ) " << endm;
	LOG_INFO << "zLocal : (" << cut_zLocal->min << "," << cut_zLocal->max << " ) " << endm;
	LOG_INFO << "rapidity : (" << cut_rapidity->min << "," << cut_rapidity->max << " ) mass = " << massAssumption << "" << endm;
	LOG_INFO << "pseudorapidity : (" << cut_pseudorapidity->min << "," << cut_pseudorapidity->max << " ) " << endm;
	
	
	LOG_INFO << "\n\n\n" << endm;

}

/**
 * Destructor
 */
StSpectraSkimmer::~StSpectraSkimmer( ){ 

	if ( NULL != cfgCuts )
		delete cfgCuts;
	cfgCuts = NULL;

}

/**
 * Initialise the Maker
 */
Int_t StSpectraSkimmer::Init( ){

	refmultCorrUtil  = CentralityMaker::instance()->getRefMultCorr();




	return kStOK;
}


Int_t StSpectraSkimmer::InitRun( int runId ) {
	LOG_INFO << "Run # " << runId << endm;
	
	skipRun = false;
	if ( isRunBad( runId ) ){
		LOG_INFO << " Run is BAD, skipping " << endm;
		skipRun = true;
	}
	
	return kStOK;
}

Int_t StSpectraSkimmer::FinishRun( int runnumber ) {
  	return kStOK;
}

/// write and close the ntuple file
Int_t StSpectraSkimmer::Finish() {

	if ( (string)"" != mTupleFileName && mTupleFile ){
		mTupleFile->Write();
		mTupleFile->Close();

		LOG_INFO  << "StSpectraSkimmer::Finish() ntuple file " << mTupleFileName  << " closed." << endm;

	}

	return kStOK;
}


/**
 * Called for each event 
 */
Int_t StSpectraSkimmer::Make(){


	// efficiently skip bad runs
	if ( !skipRun ){
  		processMuDst();
  	}

  	return kStOK;
}