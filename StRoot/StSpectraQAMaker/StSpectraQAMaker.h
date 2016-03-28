#ifndef ST_RCP_QA_MAKER_H
#define ST_RCP_QA_MAKER_H

#include "StSpectraSkimmer/StSpectraSkimmer.h"
#include "StSpectraQAMaker/StSpectraQAHistos.h"

class StSpectraQAMaker : public StSpectraSkimmer {
 public:
	StSpectraQAMaker(const Char_t *name, const Char_t *outname);
	~StSpectraQAMaker();

	Int_t  Init();
	
private:
	
	StSpectraQAHistos * histos;

	virtual void passTrackCut( string name, bool allCuts = false );
	virtual void passSingleTrackCut( string name );
	virtual void preTrackCuts( StMuTrack *primaryTrack );
	virtual void postTrackCuts( StMuTrack *primaryTrack );

	virtual void passEventCut( string name, bool allCuts = false );
	virtual void passSingleEventCut( string name );
	virtual void preEventCuts();
	virtual void postEventCuts();

	virtual void postTrackLoop( Int_t nPrimaryGood );
	

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StSpectraQAMaker.h, $ built "  __DATE__  " "  __TIME__ ; return cvs;}

	ClassDef(StSpectraQAMaker,1)
};

#endif
