#ifndef ST_RCP_QA_MAKER_H
#define ST_RCP_QA_MAKER_H

#include "StRcpSkimmer.h"
#include "StRcpQAHistos.h"

class StRcpQAMaker : public StRcpSkimmer {
 public:
	StRcpQAMaker(const Char_t *name, const Char_t *outname);
	~StRcpQAMaker();

	Int_t  Init();
	
private:
	
	static int firstDay;
	StRcpQAHistos * histos;

	virtual void passTrackCut( string name );
	virtual void preTrackCuts( StMuTrack *primaryTrack );
	virtual void postTrackCuts( StMuTrack *primaryTrack );

	void passEventCut( string name );
	virtual void preEventCuts();
	virtual void postEventCuts();

	virtual void postTrackLoop( Int_t nPrimaryGood );
	

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StRcpQAMaker.h, $ built "  __DATE__  " "  __TIME__ ; return cvs;}

	ClassDef(StRcpQAMaker,1)
};

#endif
