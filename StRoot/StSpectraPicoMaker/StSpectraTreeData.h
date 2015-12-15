#ifndef ST_SPECTRA_TREE_DATA_H
#define ST_SPECTRA_TREE_DATA_H

const Int_t kMaxTracks = 10000;

struct StSpectraTreeData {
	
	Int_t       runId;
	Float_t     weight;						// event weight
	Float_t    	corrRefMult;                // Corrected
	UShort_t	nTofMatchedTracks;			// number of tof matched tracks
	UShort_t 	bin16;						// bin16
	UShort_t 	bin9;						// bin9
	Int_t    	nTracks;      				// These are what we are keeping!!
											// these are Primary TPC tracks

	/**
	 * Track variables
	 */
	Float_t     pPt[kMaxTracks],			// Primary Pt * charge
				pP[kMaxTracks],
				pEta[kMaxTracks];

	Float_t 	gPt[ kMaxTracks ];			// Global Pt of any track we keep

	UShort_t    dedx[kMaxTracks];        	// dEdx * 1000
	UShort_t    beta[kMaxTracks];         	// beta * 20000

	UChar_t 	matchFlag[ kMaxTracks ];	// tofpid.matchflag
	Float_t 	yLocal[ kMaxTracks ];		// tofpid.yLocal
	Float_t 	zLocal[ kMaxTracks ];		// tofpid.zLocal

	Float_t 	dca[ kMaxTracks ];			// global dca
	UChar_t 	nHitsFit[ kMaxTracks ]; 	// num fit points
	UChar_t 	nHitsDedx[ kMaxTracks ]; 	// num dEdx points
	UChar_t 	nHitsPossible[ kMaxTracks ]; // num possible hits



};  
#endif
