#ifndef ST_RCP_MINIMC_PICO_MAKER_H
#define ST_RCP_MINIMC_PICO_MAKER_H


#include "TTree.h"
#include "StRcpMiniMcSkimmer.h"
#include "StRcpTreeData.h"



class StRcpMiniMcPicoMaker : public StRcpMiniMcSkimmer {
 public:
	StRcpMiniMcPicoMaker(int _gid, string _inFile, string _suffix = "_sample", string _trackType = "mc", int _maxFiles = -1);
	~StRcpMiniMcPicoMaker();
	

protected:
	virtual void analyzeMcTrack( StTinyMcTrack* track, Int_t iGood );
	virtual void analyzeTrackPair( StMiniMcPair * pair, Int_t iGood );
	virtual void postTrackLoop( Int_t nPrimaryGood );

	virtual void postEventCuts();
						
	void bookNtuples();

	StRcpTreeData  mData;
	TTree *mTree; 

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StRcpMiniMcPicoMaker.h, $ built " __DATE__ " " __TIME__  ; return cvs;}

	ClassDef(StRcpMiniMcPicoMaker,1)
};

#endif
