
#include "StSpectraQAMaker/StSpectraQAMaker.h"

// MuDst
#include "StMuDSTMaker/COMMON/StMuUtilities.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"


ClassImp(StSpectraQAMaker);

void StSpectraQAMaker::postTrackLoop( Int_t nPrimaryGood ){
	histos->nTrack_refMult->Fill( corrRefMult, nPrimaryGood  );
}

void StSpectraQAMaker::passSingleEventCut( string name ){
	histos->eventSingleCuts->Fill( name.c_str(), 1 );


	if ( "BadRun" == name ){
		StMuEvent *muEvent = muDst->event();
		int runId = muEvent->runId();	

		int day = (runId - runRange->min) / 1000; // day of run from first ( indexed at 0)
		int drn = (runId - (runRange->min + day * 1000) ); // run in day

		histos->goodRunIds->Fill( day, drn );
	}
}

void StSpectraQAMaker::failSingleEventCut( string name ){
	if ( "BadRun" == name ){
		StMuEvent *muEvent = muDst->event();
		int runId = muEvent->runId();	

		int day = (runId - runRange->min) / 1000; // day of run from first ( indexed at 0)
		int drn = (runId - (runRange->min + day * 1000) ); // run in day

		histos->badRunIds->Fill( day, drn );
	} 
}

void StSpectraQAMaker::passEventCut( string name, bool allCuts  ){

	// Fill the histos for passing this individual cut 
	passSingleEventCut( name );
	
	// if ( "Trigger" == name || "BadRun" == name ){
	// 	StMuEvent *muEvent = muDst->event();
	// 	int runId = muEvent->runId();	

	// 	int day = (runId - runRange->min) / 1000; // day of run from first ( indexed at 0)
	// 	int drn = (runId - (runRange->min + day * 1000) ); // run in day

	// 	if ( "Trigger" == name ){
	// 		histos->pre_runIds->Fill( day, drn );
	// 	} else if ( "BadRun" == name ){
	// 		histos->runIds->Fill( day, drn );
	// 	} 
	// }


	// Passed this cut and all previous cuts
	if ( allCuts ){
		histos->eventCuts->Fill( name.c_str(), 1 );
	}
}

void StSpectraQAMaker::passTrackCut( string name, bool allCuts ){
	// Passed only a single cut?
	passSingleTrackCut( name );
	
	// passed all previous track cuts also
	if ( allCuts )
		histos->trackCuts->Fill( name.c_str(), 1 );
}

void StSpectraQAMaker::passSingleTrackCut( string name ){
	histos->trackSingleCuts->Fill( name.c_str(), 1 );
}

void StSpectraQAMaker::preEventCuts(){
	StMuEvent *muEvent = muDst->event();
	int runId = muEvent->runId();

	histos->pre_vZ->Fill( pZ );
	histos->pre_vX_vY->Fill( pX, pY );
	histos->pre_vR->Fill( TMath::Sqrt( pX*pX + pY*pY ) );
	histos->pre_nTofMatchA->Fill( nTofMatchedTracks );
	histos->pre_refMult->Fill( muEvent->refMult() );
	histos->pre_nTofMatchA_corrRefMult->Fill( nTofMatchedTracks, corrRefMult );

	// fill the independent event cut histo
	// In the future I need to improve this



}

void StSpectraQAMaker::postEventCuts(){
	StMuEvent *muEvent = muDst->event();

	histos->vZ->Fill( pZ );
	histos->vX_vY->Fill( pX, pY );
	histos->vR->Fill( TMath::Sqrt( pX*pX + pY*pY ) );
	histos->nTofMatchA->Fill( nTofMatchedTracks );
	histos->refMult->Fill( muEvent->refMult() );
	histos->corrRefMult->Fill( corrRefMult, eventWeight );
	histos->corrRefMult_bin9->Fill( cent9, corrRefMult, eventWeight );
	histos->corrRefMult_bin16->Fill( cent16, corrRefMult, eventWeight );
	histos->refMultBins->Fill( cent9, eventWeight );
	histos->refMultBinsUnweighted->Fill( cent9 );
	histos->nTofMatchA_corrRefMult->Fill( nTofMatchedTracks, corrRefMult );
}

void StSpectraQAMaker::preTrackCuts( StMuTrack *primaryTrack ){
	const StMuTrack *globalTrack = primaryTrack->globalTrack();
	StMuBTofPidTraits tofPid = globalTrack->btofPidTraits();

	StThreeVectorF pMom = primaryTrack->momentum();
	StThreeVectorF gMom = globalTrack->momentum();
	float ptRatio = gMom.perp() / pMom.perp();


	histos->pre_flag->Fill( primaryTrack->flag() );
	histos->pre_nHitsFit->Fill( primaryTrack->nHitsFit(kTpcId), eventWeight );
	histos->pre_nHitsPoss->Fill( primaryTrack->nHitsPoss(kTpcId), eventWeight );
	histos->pre_nHitsFitOverPoss->Fill( (float)primaryTrack->nHitsFit(kTpcId) / primaryTrack->nHitsPoss(kTpcId), eventWeight );
	histos->pre_nHitsDedx->Fill( primaryTrack->nHitsDedx(), eventWeight );
	histos->pre_ptRatio->Fill( ptRatio, eventWeight );
	histos->pre_ptRatio2D->Fill( pMom.perp(), gMom.perp(), eventWeight );
	histos->pre_dca->Fill( primaryTrack->dcaGlobal().magnitude(), eventWeight );
	histos->pre_yLocal->Fill( tofPid.yLocal(), eventWeight );
	histos->pre_zLocal->Fill( tofPid.zLocal(), eventWeight );
	histos->pre_eta->Fill( pMom.pseudoRapidity(), eventWeight );
	double y = rapidity( pMom.perp(), pMom.pseudoRapidity(), massAssumption );
	histos->pre_rapidity->Fill( y, eventWeight );
	histos->pre_eta_phi->Fill( pMom.pseudoRapidity(), pMom.phi(), eventWeight );


	histos->pre_beta_p->Fill( pMom.mag() * primaryTrack->charge(), 1.0 / tofPid.beta(), eventWeight );
	histos->pre_dEdx_p->Fill( pMom.mag() * primaryTrack->charge(), (globalTrack->dEdx()*1e6), eventWeight );

	histos->meta_track_vertex_index->Fill( primaryTrack->vertexIndex() );
}

void StSpectraQAMaker::postTrackCuts( StMuTrack *primaryTrack ){

	const StMuTrack *globalTrack = primaryTrack->globalTrack();
	StMuBTofPidTraits tofPid = globalTrack->btofPidTraits();

	StThreeVectorF pMom = primaryTrack->momentum();
	StThreeVectorF gMom = globalTrack->momentum();
	float ptRatio = gMom.perp() / pMom.perp();
	float fitRatio = (float)primaryTrack->nHitsFit(kTpcId) / (float)primaryTrack->nHitsPoss(kTpcId);


	histos->flag->Fill( primaryTrack->flag() );
	histos->nHitsFit->Fill( primaryTrack->nHitsFit(kTpcId), eventWeight );
	histos->nHitsPoss->Fill( primaryTrack->nHitsPoss(kTpcId), eventWeight );

	histos->nHitsFitOverPoss->Fill( fitRatio, eventWeight );

	histos->nHitsDedx->Fill( primaryTrack->nHitsDedx(), eventWeight );
	histos->ptRatio->Fill( ptRatio, eventWeight );
	histos->ptRatio2D->Fill( pMom.perp(), gMom.perp(), eventWeight );
	histos->dca->Fill( primaryTrack->dcaGlobal().magnitude(), eventWeight );
	histos->yLocal->Fill( tofPid.yLocal(), eventWeight );
	histos->zLocal->Fill( tofPid.zLocal(), eventWeight );
	histos->eta->Fill( pMom.pseudoRapidity(), eventWeight );
	
	double y = rapidity( pMom.perp(), pMom.pseudoRapidity(), massAssumption );
	histos->rapidity->Fill( y, eventWeight );

	histos->eta_phi->Fill( pMom.pseudoRapidity(), pMom.phi(), eventWeight );

	histos->beta_p->Fill( pMom.mag() * primaryTrack->charge(), 1.0 / tofPid.beta(), eventWeight );
	if ( tofPid.matchFlag() >= 1 ){
		double mbeta = tofPid.beta();
		if ( mbeta == -999 )
			mbeta = 0;
		histos->matched_beta_p->Fill( pMom.mag() * primaryTrack->charge(), mbeta );
	}

	if ( tofPid.beta( ) <= 0.01  ){
		LOG_DEBUG << "beta = " << tofPid.beta() << endl << endm; 
		LOG_DEBUG << "tof = " << tofPid.timeOfFlight() << endl << endm; 
	}

	histos->dEdx_p->Fill( pMom.mag() * primaryTrack->charge(), (globalTrack->dEdx()*1e6), eventWeight );

	if ( cent9 >= 0 && cent9 <= 8 )
		histos->ptSpectra[ cent9 ]->Fill( pMom.perp() * primaryTrack->charge(), eventWeight );	
}

//---------------------------------------------------------------------------
/// constructor sets default parameters
StSpectraQAMaker::StSpectraQAMaker( const Char_t *name="rcpQAMaker", const Char_t *outname="SpectraQA.root") : StSpectraSkimmer(name, outname) {
	
}

/// default empty destructor
StSpectraQAMaker::~StSpectraQAMaker( ){ 

}


Int_t StSpectraQAMaker::Init( ){

	StSpectraSkimmer::Init();

	mTupleFile = new TFile(mTupleFileName.c_str(), "RECREATE");
	histos = new StSpectraQAHistos( runRange->min, runRange->max );

	return kStOK;
}


