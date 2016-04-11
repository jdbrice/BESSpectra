#ifndef ST_RCP_PICO_MAKER_H
#define ST_RCP_PICO_MAKER_H


#include "TTree.h"
#include "StSpectraSkimmer/StSpectraSkimmer.h"
#include "StSpectraPicoMaker/StSpectraTreeData.h"



class StSpectraSimPicoMaker : public StMaker {
 public:
	StSpectraSimPicoMaker(const Char_t *name, const Char_t *outname);
	~StSpectraSimPicoMaker();

	virtual Int_t  Init();


	

protected:
	virtual void analyzeTrack( Int_t iNode, Int_t iGood );
	virtual void postTrackLoop( Int_t nPrimaryGood );

	virtual void postEventCuts();
						
	void bookNtuples();

	StSpectraTreeData  mData;
	TTree *mTree; 					//! BTof calibration ntuple

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StSpectraSimPicoMaker.h, $ built "  __DATE__  " "  __TIME__  ; return cvs;}

	ClassDef(StSpectraSimPicoMaker,1)
};

#endif
