#ifndef ST_RCP_PICO_MAKER_H
#define ST_RCP_PICO_MAKER_H


#include "TTree.h"
#include "StSpectraSkimmer/StSpectraSkimmer.h"
#include "StSpectraPicoMaker/StSpectraTreeData.h"

#include "StRooBarb/XmlFunction.h"
#include <vector>
#include <map>


#include "tofGenerator.h"
#include "dedxGenerator.h"


class StSpectraSimPicoMaker : public StMaker {
 public:
	StSpectraSimPicoMaker(const Char_t *name, const Char_t *outname);
	~StSpectraSimPicoMaker();

	virtual Int_t  Init();
	virtual Int_t  InitRun( int tun );

	Int_t  Make();
	Int_t  FinishRun(int runnumber);
	Int_t  Finish();


	

protected:
	map< string, XmlFunction*> vars;

	tofGenerator * zbGen;
	dedxGenerator * zdGen;

	double getVar( string name ){

		if ( vars.count( name ) > 0 ){
			return vars[name]->sample();
		}

	}

	virtual void analyzeTrack( Int_t iGood );
	virtual void postTrackLoop( Int_t nPrimaryGood );

	virtual void postEventCuts();
						
	void bookNtuples();

	StSpectraTreeData  mData;
	TTree *mTree; 					//! BTof calibration ntuple

	string mTupleFileName;
	TFile *mTupleFile;

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StSpectraSimPicoMaker.h, $ built "  __DATE__  " "  __TIME__  ; return cvs;}

	ClassDef(StSpectraSimPicoMaker,1)
};

#endif
