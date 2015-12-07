#include "StRcpQAMaker.h"
#include "StMuDSTMaker/COMMON/StMuUtilities.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"


ClassImp(StRcpQAMaker);

void StRcpQAMaker::postTrackLoop( Int_t nPrimaryGood ){
	histos->nTrack_refMult->Fill( corrRefMult, nPrimaryGood  );
}

void StRcpQAMaker::passEventCut( string name ){
	histos->eventCuts->Fill( name.c_str(), 1 );

	if ( "Trigger" == name || "BadRun" == name ){
		StMuEvent *muEvent = muDst->event();
		int runId = muEvent->runId();	

		int day = (runId - runRange->min) / 1000; // day of run from first ( indexed at 0)
		int drn = (runId - (runRange->min + day * 1000) ); // run in day

		if ( "Trigger" == name ){
			histos->pre_runIds->Fill( day, drn );
		} else if ( "BadRun" == name ){
			histos->runIds->Fill( day, drn );
		} 
	}
}

void StRcpQAMaker::passTrackCut( string name ){
	histos->trackCuts->Fill( name.c_str(), 1 );
}

void StRcpQAMaker::preEventCuts(){
	StMuEvent *muEvent = muDst->event();
	int runId = muEvent->runId();

	histos->pre_vZ->Fill( pZ );
	histos->pre_vX_vY->Fill( pX, pY );
	histos->pre_vR->Fill( TMath::Sqrt( pX*pX + pY*pY ) );
	histos->pre_nTofMatchA->Fill( nTofMatchedTracks );
	histos->pre_refMult->Fill( muEvent->refMult() );
	histos->pre_nTofMatchA_corrRefMult->Fill( nTofMatchedTracks, corrRefMult );
}

void StRcpQAMaker::postEventCuts(){
	StMuEvent *muEvent = muDst->event();

	histos->vZ->Fill( pZ );
	histos->vX_vY->Fill( pX, pY );
	histos->vR->Fill( TMath::Sqrt( pX*pX + pY*pY ) );
	histos->nTofMatchA->Fill( nTofMatchedTracks );
	histos->refMult->Fill( muEvent->refMult() );
	histos->corrRefMult->Fill( corrRefMult, eventWeight );
	histos->refMultBins->Fill( cent9, eventWeight );
	histos->refMultBinsUnweighted->Fill( cent9 );
	histos->nTofMatchA_corrRefMult->Fill( nTofMatchedTracks, corrRefMult );
}

void StRcpQAMaker::preTrackCuts( StMuTrack *primaryTrack ){
	const StMuTrack *globalTrack = primaryTrack->globalTrack();
	StMuBTofPidTraits tofPid = globalTrack->btofPidTraits();

	StThreeVectorF pMom = primaryTrack->momentum();
	StThreeVectorF gMom = globalTrack->momentum();
	float ptRatio = gMom.perp() / pMom.perp();

	histos->pre_nHitsFit->Fill( globalTrack->nHitsFit(kTpcId), eventWeight );
	histos->pre_nHitsFitOverPoss->Fill( (float)globalTrack->nHitsFit(kTpcId) / globalTrack->nHitsPoss(kTpcId), eventWeight );
	histos->pre_nHitsDedx->Fill( globalTrack->nHitsDedx(), eventWeight );
	histos->pre_ptRatio->Fill( ptRatio, eventWeight );
	histos->pre_ptRatio2D->Fill( pMom.perp(), gMom.perp(), eventWeight );
	histos->pre_dca->Fill( primaryTrack->dcaGlobal().magnitude(), eventWeight );
	histos->pre_yLocal->Fill( tofPid.yLocal(), eventWeight );
	histos->pre_zLocal->Fill( tofPid.zLocal(), eventWeight );
	histos->pre_eta->Fill( pMom.pseudoRapidity(), eventWeight );
	double y = rapidity( pMom.perp(), pMom.pseudoRapidity(), massAssumption );
	histos->pre_rapidity->Fill( y, eventWeight );
	histos->pre_eta_phi->Fill( pMom.pseudoRapidity(), pMom.phi(), eventWeight );


	histos->trackBeta->Fill( pMom.mag(), 1.0 / tofPid.beta(), eventWeight );
}

void StRcpQAMaker::postTrackCuts( StMuTrack *primaryTrack ){

	const StMuTrack *globalTrack = primaryTrack->globalTrack();
	StMuBTofPidTraits tofPid = globalTrack->btofPidTraits();

	StThreeVectorF pMom = primaryTrack->momentum();
	StThreeVectorF gMom = globalTrack->momentum();
	float ptRatio = gMom.perp() / pMom.perp();

	histos->nHitsFit->Fill( globalTrack->nHitsFit(kTpcId), eventWeight );
	histos->nHitsFitOverPoss->Fill( (float)globalTrack->nHitsFit(kTpcId) / globalTrack->nHitsPoss(kTpcId), eventWeight );
	histos->nHitsDedx->Fill( globalTrack->nHitsDedx(), eventWeight );
	histos->ptRatio->Fill( ptRatio, eventWeight );
	histos->ptRatio2D->Fill( pMom.perp(), gMom.perp(), eventWeight );
	histos->dca->Fill( primaryTrack->dcaGlobal().magnitude(), eventWeight );
	histos->yLocal->Fill( tofPid.yLocal(), eventWeight );
	histos->zLocal->Fill( tofPid.zLocal(), eventWeight );
	histos->eta->Fill( pMom.pseudoRapidity(), eventWeight );
	
	double y = rapidity( pMom.perp(), pMom.pseudoRapidity(), massAssumption );
	histos->rapidity->Fill( y, eventWeight );

	histos->eta_phi->Fill( pMom.pseudoRapidity(), pMom.phi(), eventWeight );

	histos->ptSpectra[ cent9 ]->Fill( pMom.perp(), eventWeight );	
}

//---------------------------------------------------------------------------
/// constructor sets default parameters
StRcpQAMaker::StRcpQAMaker( const Char_t *name="rcpQAMaker", const Char_t *outname="rcpQA.root") : StRcpSkimmer(name, outname) {
	
}

/// default empty destructor
StRcpQAMaker::~StRcpQAMaker( ){ 

}


Int_t StRcpQAMaker::Init( ){

	StRcpSkimmer::Init();

	mTupleFile = new TFile(mTupleFileName.c_str(), "RECREATE");
	histos = new StRcpQAHistos( runRange->min, runRange->max );

	return kStOK;
}


