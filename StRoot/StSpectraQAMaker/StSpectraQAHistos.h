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

#include <map>

class StSpectraQAHistos
{
public:
	StSpectraQAHistos( int firstRun, int lastRun = -1 ){


		// parse out the first and last day of the run
		int firstDay = (firstRun/1000) - (firstRun/100000)*100;
		int lastDay = lastRun / 1000 - (lastRun/100000)*100;
		if ( lastRun < 0 )
			lastDay = firstDay + 24;

		int nDays = lastDay - firstDay + 1;
		string rTitle = "Run Ids Pre-Rejection; Day - " + jdb::ts( firstDay ) + "; Run# for Day";
		/*
			Event Cuts
		 */
		eventCuts 	= new TH1I( "event_cuts", "Event Cuts", 10, 0, 10 );
		eventSingleCuts = new TH1I( "event_single_cuts", "Event Single Cuts", 10, 0, 10 );
		pre_runIds 	= new TH2I( "pre_run_ids", rTitle.c_str(), nDays, 0, nDays, 150, 0, 150 );
		runIds 		= new TH2I( "run_ids", rTitle.c_str(), nDays, 0, nDays, 150, 0, 150 );
		
		nTrack_refMult = new TH1F( "event_nTrack_refMult", "nTrack vs. RefMult", 400, 0, 400 );
		
		pre_refMult = new TH1F( "event_pre_refMult", "Pre Raw RefMult", 400, 0, 400 );
		refMult 	= new TH1F( "event_refMult", "Raw RefMult", 400, 0, 400 );
		corrRefMult = new TH1F( "event_corrRefMult", "Corr RefMult", 1000, 0, 500 );
		corrRefMult_bin9 = new TH2F( "event_corrRefMult_bin9", "Corr RefMult vs. bin9; bin9 ; corrRefMult", 20, 0, 20, 1000, 0, 500 );
		refMultBins = new TH1F( "event_refMultBins", "Corr RefMult Bins", 10, 0, 10 );
		refMultBinsUnweighted = new TH1F( "event_refMultBinsUnweighted", "Corr RefMult Bins (Unweighted)", 10, 0, 10 );

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
		trackSingleCuts = new TH1I( "track_single_cuts", "Track Single Cuts", 20, 0, 20 );
		pre_nHitsFit 	= new TH1F( "track_pre_nHitsFit", "", 52, 0, 52 );
		nHitsFit 		= new TH1F( "track_nHitsFit", "", 52, 0, 52 );

		pre_flag 		= new TH1F( "track_pre_flag", "", 250, 0, 1000 );
		flag 			= new TH1F( "track_flag", "", 250, 0, 1000 );

		pre_nHitsDedx 	= new TH1F( "track_pre_nHitsDedx", "", 52, 0, 52 );
		nHitsDedx 		= new TH1F( "track_nHitsDedx", "", 52, 0, 52 );

		pre_nHitsPoss 	= new TH1F( "track_pre_nHitsPoss", "", 52, 0, 52 );
		nHitsPoss 		= new TH1F( "track_nHitsPoss", "", 52, 0, 52 );

		pre_nHitsFitOverPoss 	= new TH1F( "track_pre_nHitsFitOverPoss", "", 110, 0, 1.1 );
		nHitsFitOverPoss 		= new TH1F( "track_nHitsFitOverPoss", "", 110, 0, 1.1 );

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

		pre_eta 	= new TH1F( "track_pre_eta", "pre #eta", 300, -1.5, 1.5 );
		eta 		= new TH1F( "track_eta", "#eta", 300, -1.5, 1.5 );

		pre_rapidity 	= new TH1F( "track_pre_rapidity", "pre rapidity", 300, -1.5, 1.5 );
		rapidity 		= new TH1F( "track_rapidity", "rapidity", 300, -1.5, 1.5 );

		pre_eta_phi 	= new TH2F( "track_pre_eta_phi", "pre #eta", 200, -1.5, 1.5, 60, -3.2, 3.2 );
		eta_phi 		= new TH2F( "track_eta_phi", "#eta", 200, -1.5, 1.5, 60, -3.2, 3.2 );

		beta_p 			= new TH2F( "track_beta_p", "beta; P [GeV/c]; 1/#beta", 160, -5, 5, 200, -.5, 3 );
		pre_beta_p 			= new TH2F( "track_pre_beta_p", "pre beta; P [GeV/c]; 1/#beta", 160, -5, 5, 200, -.5, 3 );

		dEdx_p 			= new TH2F( "track_dEdx_p", "dEdx; P [GeV/c]; dEdx", 400, -5, 5, 400, 1.5, 8 );
		pre_dEdx_p 			= new TH2F( "track_pre_dEdx_p", "pre dEdx; P [GeV/c]; dEdx", 400, -5, 5, 400, 1.5, 8 );

		for ( int i = 0; i < 9; i++ ){
			ptSpectra[ i ] = new TH1F( ("pt_spectra_" + jdb::ts(i)).c_str(), "p_T Spectra", 200, -5, 5  );
		}


		meta_track_vertex_index = new TH1F( "track_vertex_index", "Track Vertex Index", 10, 0, 10 );


	}
	~StSpectraQAHistos();

	/**
	 * Event Histos
	 */
	TH2I *runIds, *pre_runIds;
	TH1I *eventCuts, *eventSingleCuts;
	TH1F *pre_vZ, *pre_vR, *pre_nTofMatchA, *nTrack_refMult, *refMult, *pre_refMult;
	TH2F *pre_vX_vY, *vX_vY, *pre_nTofMatchA_corrRefMult, *nTofMatchA_corrRefMult, *corrRefMult_bin9;
	TH1F *vZ, *vR, *nTofMatchA, *corrRefMult, *refMultBins, *refMultBinsUnweighted;


	/**
	 * Track Histos
	 */
	TH1I *trackCuts, *trackSingleCuts;
	TH1F *pre_nHitsFit, *pre_nHitsDedx, *pre_nHitsPoss, *pre_nHitsFitOverPoss, *pre_ptRatio, *pre_dca, *pre_flag, *flag;
	TH1F *nHitsFit, *nHitsDedx, *nHitsPoss, *nHitsFitOverPoss, *ptRatio, *dca, *pre_yLocal, *pre_zLocal, *yLocal, *zLocal;
	TH1F *pre_eta, *eta, *pre_rapidity, *rapidity;
	TH2F *pre_ptRatio2D, *ptRatio2D, *eta_phi, *pre_eta_phi;
	TH2F *beta_p, *pre_beta_p, *dEdx_p, *pre_dEdx_p;

	/**
	 * Analysis 
	 */
	TH1F * ptSpectra[9];


	/**
	 * META
	 */
	TH1F * meta_track_vertex_index;


};



#endif