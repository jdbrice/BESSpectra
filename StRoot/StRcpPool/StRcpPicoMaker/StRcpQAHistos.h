#ifndef ST_RCP_QA_HISTOS_H
#define ST_RCP_QA_HISTOS_H


#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1I.h"
#include "TH2.h"
#include <string>
#include <sstream>

#include "StRooBarb/Utils.h"

class StRcpQAHistos
{
public:
	StRcpQAHistos(){


		/*
			Event Cuts
		 */
		eventCuts 	= new TH1I( "event_cuts", "Event Cuts", 10, 0, 10 );
		pre_runIds 	= new TH2I( "pre_run_ids", "Run Ids Pre-Rejection; Day - 46; Run# for Day", 25, 0, 25, 150, 0, 150 );
		runIds 		= new TH2I( "run_ids", "Run Ids; Day - 46; Run# for Day", 25, 0, 25, 150, 0, 150 );
		
		nTrack_refMult = new TH1F( "event_nTrack_refMult", "nTrack vs. RefMult", 400, 0, 400 );
		
		pre_refMult = new TH1F( "event_pre_refMult", "Pre Raw RefMult", 400, 0, 400 );
		refMult 	= new TH1F( "event_refMult", "Raw RefMult", 400, 0, 400 );
		corrRefMult = new TH1F( "event_corrRefMult", "Corr RefMult", 1000, 0, 500 );
		refMultBins = new TH1F( "event_refMultBins", "Corr RefMult Bins", 10, 0, 10 );

		pre_vZ 	= new TH1F( "event_pre_vZ", "pre vZ", 400, -200, 200 );
		vZ 		= new TH1F( "event_vZ", "vZ", 100, -50, 50 );

		pre_vX_vY 	= new TH2F( "event_pre_vX_vY", "pre vX vY", 150, -3, 3, 150, -3, 3 );
		vX_vY 		= new TH2F( "event_vX_vY", "vX vY", 150, -3, 3, 150, -3, 3 );

		pre_vR 	= new TH1F( "event_pre_vR", "", 400, 0, 4 );
		vR 		= new TH1F( "event_vR", "", 110, 0, 1.1 );

		pre_nTofMatchA 	= new TH1F( "event_pre_nTofMatchA", "", 450, 0, 450 );
		nTofMatchA 		= new TH1F( "event_nTofMatchA", "", 400, 0, 400 );

		pre_nTofMatchA_corrRefMult 	= new TH2F( "event_pre_nTofMatchA_corrRefMult", "", 400, 0, 400, 400, 0, 400 ); // x = ntof, y = refmult
		nTofMatchA_corrRefMult 		= new TH2F( "event_nTofMatchA_corrRefMult", "", 400, 0, 400, 400, 0, 400 ); // x = ntof, y = refmult


		/**
		 * Track Histos
		 */
		trackCuts 		= new TH1I( "track_cuts", "Track Cuts", 20, 0, 20 );
		pre_nHitsFit 	= new TH1F( "track_pre_nHitsFit", "", 52, 0, 52 );
		nHitsFit 		= new TH1F( "track_nHitsFit", "", 52, 0, 52 );

		pre_nHitsDedx 	= new TH1F( "track_pre_nHitsDedx", "", 52, 0, 52 );
		nHitsDedx 		= new TH1F( "track_nHitsDedx", "", 52, 0, 52 );

		pre_nHitsFitOverPoss 	= new TH1F( "track_pre_nHitsFitOverPoss", "", 100, 0, 1.0 );
		nHitsFitOverPoss 		= new TH1F( "track_nHitsFitOverPoss", "", 100, 0, 1.0 );

		pre_ptRatio 	= new TH1F( "track_pre_ptRatio", "", 100, 0, 2.1  );
		ptRatio 		= new TH1F( "track_ptRatio", "", 100, 0, 2.1 );

		pre_ptRatio2D 	= new TH2F( "track_pre_ptRatio2D", "Global vs. Primary; Primary pT; Global pT", 50, 0, 5, 50, 0, 5  );
		ptRatio2D 		= new TH2F( "track_ptRatio2D", "Global vs. Primary; Primary pT; Global pT", 50, 0, 5, 50, 0, 5 );

		pre_dca 	= new TH1F( "track_pre_dca", "", 160, 0, 4.0  );
		dca 		= new TH1F( "track_dca", "", 140, 0, 3.5 );

		pre_yLocal 	= new TH1F( "track_pre_yLocal", "", 80, -2.5, 2.5  );
		yLocal 		= new TH1F( "track_yLocal", "", 80, -2.5, 2.5 );

		pre_zLocal 	= new TH1F( "track_pre_zLocal", "", 120, -3.5, 3.5  );
		zLocal 		= new TH1F( "track_zLocal", "", 120, -3.5, 3.5 );

		trackBeta 	= new TH2F( "trackBeta", "beta", 80, 0, 5, 200, -.5, 3 );

		for ( int i = 0; i < 9; i++ ){
			ptSpectra[ i ] = new TH1F( ("pt_spectra_" + jdb::ts(i)).c_str(), "p_T Spectra", 200, 0, 5  );
		}


	}
	~StRcpQAHistos();

	/**
	 * Event Histos
	 */
	TH2I *runIds, *pre_runIds;
	TH1I *eventCuts;
	TH1F *pre_vZ, *pre_vR, *pre_nTofMatchA, *nTrack_refMult, *refMult, *pre_refMult;
	TH2F *pre_vX_vY, *vX_vY, *pre_nTofMatchA_corrRefMult, *nTofMatchA_corrRefMult;
	TH1F *vZ, *vR, *nTofMatchA, *corrRefMult, *refMultBins;


	/**
	 * Track Histos
	 */
	TH1I *trackCuts;
	TH1F * pre_nHitsFit, *pre_nHitsDedx, *pre_nHitsFitOverPoss, *pre_ptRatio, *pre_dca;
	TH1F * nHitsFit, *nHitsDedx, *nHitsFitOverPoss, *ptRatio, *dca, *pre_yLocal, *pre_zLocal, *yLocal, *zLocal;
	TH2F * pre_ptRatio2D, *ptRatio2D, *eta_phi, *pre_eta_phi;
	TH2F * trackBeta;

	/**
	 * Analysis 
	 */
	TH1F * ptSpectra[9];


};



#endif