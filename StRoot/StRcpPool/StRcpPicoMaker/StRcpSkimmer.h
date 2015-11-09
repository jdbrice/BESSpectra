#ifndef ST_RCP_SKIMMER_H
#define ST_RCP_SKIMMER_H



// StRefMultCorr
#include "StRefMultCorr/StRefMultCorr.h"
#include "StRefMultCorr/CentralityMaker.h"

// StRoot
#include "StMaker.h"

// roobarb
#include "StRooBarb/XmlConfig.h"
#include "StRooBarb/ConfigRange.h"

class StMuDstMaker;
class StMuDst;
class StMuEvent;
class StMuTrack;

class StRcpSkimmer : public StMaker {
 public:
	StRcpSkimmer(const Char_t *name, const Char_t *outname);
	~StRcpSkimmer();
	 
	Int_t  Init();
	Int_t  InitRun(int runnumber);
	Int_t  Make();
	Int_t  FinishRun(int runnumber);
	Int_t  Finish();

	static int badRuns[];
	static int nBadRuns;
	map< int, bool > badRunMap;


protected:

	
	jdb::XmlConfig * cfgCuts;
	// event
	jdb::ConfigRange *cut_vZ, *cut_vR, *cut_nTofMatch;
	//track
	jdb::ConfigRange *cut_nHitsFit, *cut_nHitsDedx, *cut_nHitsRatio;
	jdb::ConfigRange *cut_pt, *cut_ptRatio, *cut_dca, *cut_yLocal, *cut_zLocal;


	Int_t nTofMatchedTracksA();

	/**
	 * Event Selection
	 */
	Bool_t keepEvent();
	virtual void passEventCut( string name );
	virtual void preEventCuts();
	virtual void postEventCuts();

	// Event variables to make available to children
	Int_t runId;
	Float_t eventWeight;
	Float_t corrRefMult;
	Int_t cent9;
	Int_t cent16;
	Int_t nTofMatchedTracks;
	Float_t pX, pY, pZ;


	virtual void postTrackLoop( Int_t nPrimaryGood );

	/**
	 * Track Selection
	 */
	Bool_t keepTrack( Int_t iNode );
	virtual void passTrackCut( string name );
	virtual void preTrackCuts( StMuTrack *primaryTrack );
	virtual void postTrackCuts( StMuTrack *primaryTrack );

	virtual void analyzeTrack( Int_t iNode, Int_t iGoodTrack );

	StRefMultCorr* refmultCorrUtil;

	void processMuDst();

	
	StMuDst *muDst;
	string mTupleFileName;
	TFile *mTupleFile;
	
	Bool_t  doPrintMemoryInfo; 		//! control debug memory data
	Bool_t  doPrintCpuInfo; 		//! control debug timing data

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StRcpSkimmer.h, $ built "  __DATE__  "  "  __TIME__ ; return cvs;}

	ClassDef(StRcpSkimmer,1)
};

#endif
