#ifndef ST_SPECTRA_MINIMC_PICO_MAKER_H
#define ST_SPECTRA_MINIMC_PICO_MAKER_H


#include "TTree.h"
#include "StSpectraMiniMcSkimmer/StSpectraMiniMcSkimmer.h"
#include "StSpectraPicoMaker/StSpectraTreeData.h"



class StSpectraMiniMcPicoMaker : public StSpectraMiniMcSkimmer {
 public:
	StSpectraMiniMcPicoMaker(int _gid, string _inFile, string _suffix = "_sample", string _trackType = "mc", int _maxFiles = -1);
	~StSpectraMiniMcPicoMaker();
	

protected:
	virtual void analyzeMcTrack( StTinyMcTrack* track, Int_t iGood );
	virtual void analyzeTrackPair( StMiniMcPair * pair, Int_t iGood );
	virtual void postTrackLoop( Int_t nPrimaryGood );

	virtual void postEventCuts();
						
	void bookNtuples();

	StSpectraTreeData  mData;
	TTree *mTree; 

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StSpectraMiniMcPicoMaker.h, $ built " __DATE__ " " __TIME__  ; return cvs;}

	ClassDef(StSpectraMiniMcPicoMaker,1)
};

#endif
