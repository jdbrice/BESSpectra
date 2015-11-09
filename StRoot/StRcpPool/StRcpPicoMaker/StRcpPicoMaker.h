#ifndef ST_RCP_PICO_MAKER_H
#define ST_RCP_PICO_MAKER_H


#include "TTree.h"
#include "StRcpSkimmer.h"
#include "StRcpTreeData.h"



class StRcpPicoMaker : public StRcpSkimmer {
 public:
	StRcpPicoMaker(const Char_t *name, const Char_t *outname);
	~StRcpPicoMaker();

	virtual Int_t  Init();


	

protected:
	virtual void analyzeTrack( Int_t iNode, Int_t iGood );
	virtual void postTrackLoop( Int_t nPrimaryGood );

	virtual void postEventCuts();
						
	void bookNtuples();

	StRcpTreeData  mData;
	TTree *mTree; 					//! BTof calibration ntuple

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StRcpPicoMaker.h, $ built "  __DATE__  " "  __TIME__  ; return cvs;}

	ClassDef(StRcpPicoMaker,1)
};

#endif
