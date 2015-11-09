#ifndef ST_RCP_MINIMC_SKIMMER_H
#define ST_RCP_MINIMC_SKIMMER_H

//STAR
#include "StMiniMcEvent/StMiniMcEvent.h"
#include "StMiniMcEvent/StMiniMcPair.h"
#include "StMiniMcEvent/StTinyMcTrack.h"
#include "StMiniMcEvent/StTinyRcTrack.h"

// ROOT
#include "TChain.h"
#include "TFile.h"

// STL
#include <string>
#include <fstream>
using namespace std;

// StRefMultCorr
#include "StRefMultCorr/StRefMultCorr.h"
#include "StRefMultCorr/CentralityMaker.h"

// StRcpPicoMaker
#include "StRcpTreeData.h"

// roobarb
#include "StRooBarb/XmlConfig.h"
#include "StRooBarb/ConfigRange.h"

class StRcpMiniMcSkimmer {
 public:
	StRcpMiniMcSkimmer(int _gid, string _inFile, string _suffix, string _trackType = "mc", int _maxFiles = -1); // trackType = 'mc' or 'rc'
	~StRcpMiniMcSkimmer();

	map< int, bool > badRunMap; // uses list of bad runs in StRcpSkimmer::badRuns

	void make(  ); 

protected:

	// MiniMc
	TChain * miniMcTree;
	StMiniMcEvent * event;
	TFile * dataFile;
	string trackType;

	int gid;
	double mass;



	void loadFileList( string fList, int maxFiles );
	string plcName(  );
	double plcMass(  );
	int plcCharge(  );
	double rapidity( double p, double pz );

	virtual void analyzeMcTrack( StTinyMcTrack * track, int iGoodTrack ) ;
	virtual void analyzeTrackPair( StMiniMcPair * pair, int iGoodTrack ) ;


	jdb::XmlConfig * cfgCuts;
	// event
	jdb::ConfigRange *cut_vZ, *cut_vR, *cut_nTofMatch;
	//track
	jdb::ConfigRange *cut_nHitsFit, *cut_nHitsDedx, *cut_nHitsRatio;
	jdb::ConfigRange *cut_pt, *cut_ptRatio, *cut_dca, *cut_yLocal, *cut_zLocal;


	/**
	 * Event Selection
	 */
	Bool_t keepEvent();
	virtual void passEventCut( string name ) {}
	virtual void preEventCuts() {}
	virtual void postEventCuts() {}

	// Event variables to make available to children
	Int_t runId;
	Float_t eventWeight;
	Float_t corrRefMult;
	Int_t cent9;
	Int_t cent16;


	virtual void postTrackLoop( Int_t nPrimaryGood ) {}

	/**
	 * Track Selection
	 */
	Bool_t keepTrack( StMiniMcPair * track );
	virtual void passTrackCut( string name ) { /*cout << "\t" << name << endl;*/ }
	virtual void preTrackCuts( StMiniMcPair *primaryTrack ) {}
	virtual void postTrackCuts( StMiniMcPair *primaryTrack ) {}

	StRefMultCorr* refmultCorrUtil;

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StRcpMiniMcSkimmer.h, $ built " __DATE__ " " __TIME__ ; return cvs;}

	ClassDef(StRcpMiniMcSkimmer,1)
};

#endif
