#include "StRcpMiniMcSkimmer.h"
#include "StRcpSkimmer.h"

StRcpMiniMcSkimmer::StRcpMiniMcSkimmer(int _gid, string _inFile, string _suffix, string _trackType, int _maxFiles ){

	cout << "[StRcpMiniMcSkimmer::" << __FUNCTION__ << "]" << endl;

	trackType = _trackType;
	gid = _gid;
	miniMcTree = new TChain( "StMiniMcTree" );
	loadFileList( _inFile, _maxFiles );
	

	dataFile = new TFile( (plcName() + _suffix + ".root").c_str(), "RECREATE" );


	// histos = new StEffHistos( 	0, 400,
	// 							-1.1, 1.1, .025,
	// 							0, 5, .05 );

	cout << "GEANT ID = " << gid << endl;

	refmultCorrUtil  = CentralityMaker::instance()->getRefMultCorr();

	// load up the cuts from an xml config
	cfgCuts = NULL;
	cfgCuts = new jdb::XmlConfig( "./cuts.xml" );

	// make the cuts variables
	cut_vZ = new jdb::ConfigRange( cfgCuts, "Event.zVertex" );
	cut_vR = new jdb::ConfigRange( cfgCuts, "Event.rVertex" );
	cut_nTofMatch = new jdb::ConfigRange( cfgCuts, "Event.nTofMatch" );

	cut_nHitsFit = new jdb::ConfigRange( cfgCuts, "Track.nHitsFit" );
	cut_nHitsDedx = new jdb::ConfigRange( cfgCuts, "Track.nHitsDedx" );
	cut_nHitsRatio = new jdb::ConfigRange( cfgCuts, "Track.nHitsRatio" );

	cut_pt = new jdb::ConfigRange( cfgCuts, "Track.pt" );
	cut_ptRatio = new jdb::ConfigRange( cfgCuts, "Track.ptRatio" );
	cut_dca = new jdb::ConfigRange( cfgCuts, "Track.dca" );
	cut_yLocal = new jdb::ConfigRange( cfgCuts, "Track.yLocal" );
	cut_zLocal = new jdb::ConfigRange( cfgCuts, "Track.zLocal" );
	cut_rapidity = new jdb::ConfigRange( cfgCuts, "Track.rapidity", -10000, 10000 );
	cut_pseudorapidity = new jdb::ConfigRange( cfgCuts, "Track.pseudorapidity", -10000, 10000 );
	massAssumption = cfgCuts->getDouble( "Track.rapidity:mass", 0.13957018 /*pi^+ mass in GeV/c */);

	badRuns = cfgCuts->getIntVector( "BadRuns" );
	runRange = new jdb::ConfigRange( cfgCuts, "RunRange" );
	// build the bad run map
	for ( int rId : badRuns ){
		badRunMap[ rId ] = true;
	}


	// report
	cout << endl << "MiniMc Skimmer for " << trackType << " tracks" << endl << endm;
	
	LOG_INFO << "Processing Runs between " << runRange->min << " --> " << runRange->max << " (inclusive) " << endm; 
	
	LOG_INFO << "Excluding " << badRuns.size() << " bad runs " << endm;
	for ( int i : badRuns ){
		LOG_INFO << i << ", ";
	}
	LOG_INFO << endm << endm;

	LOG_INFO << endl << endl << endm;
	LOG_INFO << "Event Cuts \n\n" << endm;
	LOG_INFO << "Z Vertex : ( " << cut_vZ->min  << ", " << cut_vZ->max << " )" << endm;
	LOG_INFO << "R Vertex : ( " << cut_vR->min  << ", " << cut_vR->max << " )" << endm;
	LOG_INFO << "nTofMatch : ( " << cut_nTofMatch->min  << ", " << cut_nTofMatch->max << " )" << endm;

	LOG_INFO << "Track Cuts \n\n" << endm;
	LOG_INFO << "nHitsFit : ( " << cut_nHitsFit->min  << ", " << cut_nHitsFit->max << " )" << endm;
	LOG_INFO << "nHitsDedx : ( " << cut_nHitsDedx->min  << ", " << cut_nHitsDedx->max << " )" << endm;
	LOG_INFO << "nHitsRatio : ( " << cut_nHitsRatio->min  << ", " << cut_nHitsRatio->max << " )" << endm;
	LOG_INFO << "pt : (" << cut_pt->min << "," << cut_pt->max << " ) " << endm;
	LOG_INFO << "ptRatio : (" << cut_ptRatio->min << "," << cut_ptRatio->max << " ) " << endm;
	LOG_INFO << "dca : (" << cut_dca->min << "," << cut_dca->max << " ) " << endm;
	LOG_INFO << "yLocal : (" << cut_yLocal->min << "," << cut_yLocal->max << " ) " << endm;
	LOG_INFO << "zLocal : (" << cut_zLocal->min << "," << cut_zLocal->max << " ) " << endm;
	LOG_INFO << "rapidity : (" << cut_rapidity->min << "," << cut_rapidity->max << " ) mass = " << massAssumption << "" << endm;
	LOG_INFO << "pseudorapidity : (" << cut_pseudorapidity->min << "," << cut_pseudorapidity->max << " ) " << endm;
	
	
	LOG_INFO << "\n\n\n" << endm;


	

}

StRcpMiniMcSkimmer::~StRcpMiniMcSkimmer(){
	cout << "[StRcpMiniMcSkimmer::" << __FUNCTION__ << "]" << endl;
	
	dataFile->Write();

	delete miniMcTree;
	delete event;
}


void StRcpMiniMcSkimmer::loadFileList( string fList, int maxFiles ){
	cout << "[StRcpMiniMcSkimmer::" << __FUNCTION__ << "]" << endl;
	
	cout << "filelist : " << fList << endl;
	
	ifstream inFile( fList.c_str() );

	if ( maxFiles < 0 )
		maxFiles = 100000;

	int fRead = 0;
	string fileURL = "";
	while ( getline( inFile, fileURL ) && fRead < maxFiles ){
		int r = miniMcTree->AddFile( (fileURL).c_str() );
		fRead++;
	}

	inFile.close();
	cout << "\tLoaded " << fRead << " files" << endl;
	cout << "[StRcpMiniMcSkimmer::~" << __FUNCTION__ << "]" << endl;
}

void StRcpMiniMcSkimmer::make( ){
	cout << "[StRcpMiniMcSkimmer::" << __FUNCTION__ << "]" << endl;

	event = 0;
	miniMcTree->SetBranchAddress( "StMiniMcEvent", &event );

	int nEvents = miniMcTree->GetEntries();
	cout << "\tLoaded " << nEvents << " events "<< endl;

	for ( int i = 0; i < nEvents; i ++ ){

		miniMcTree->GetEntry( i );



		if ( !keepEvent() ) continue;

		int nPrimaryGood = 0;

		// make refmult corr
		corrRefMult = refmultCorrUtil->getRefMultCorr();

		if ( "mc" == trackType ){
			TClonesArray *mcTracks = event->tracks( (Category)MC );
		    TIter nextPair(mcTracks);
		    StTinyMcTrack * mcTrack;
		    
		    while( (mcTrack=(StTinyMcTrack*) nextPair.Next()) ) {
		        // use mcTrack
		        // only look at the plcs we want
		        
				if ( mcTrack->geantId() == gid /*&& mcTrack->parentGeantId() == 0*/ && mcTrack->ptMc() >= 0.1 && mcTrack->chargeMc() == plcCharge( ) ){	// should not be needed with geantId cut
		        	analyzeMcTrack( mcTrack, nPrimaryGood );
		        	nPrimaryGood++;
		        }
		    }
		}
		

		if ( "rc" == trackType ){
		    TClonesArray *mcPairs = event->tracks( (Category)MATCHED );
		    TIter next(mcPairs);
		    StMiniMcPair * mcPair;
		    while( (mcPair=(StMiniMcPair*) next.Next()) ) {
		        // use mcPair
		        if ( keepTrack( mcPair ) ){	// track cuts for matched pairs
			        analyzeTrackPair( mcPair, nPrimaryGood );
			        nPrimaryGood++;
			    }
		    }
		}

		postTrackLoop( nPrimaryGood );


	}


	cout << "[StRcpMiniMcSkimmer::~" << __FUNCTION__ << "]" << endl;
}



void StRcpMiniMcSkimmer::analyzeMcTrack( StTinyMcTrack * track, int iGood ){
	
	cout << "HELLO" << endl;
	double y = rapidity( track->pMc(), track->pzMc() );
	double pt = track->ptMc();


}
void StRcpMiniMcSkimmer::analyzeTrackPair( StMiniMcPair * pair, int iGood ){

}



bool StRcpMiniMcSkimmer::keepEvent(){


	if ( event->vertexZ() > cut_vZ->max  || event->vertexZ() < cut_vZ->min)
		return false;
	passEventCut( "vZ");

	
	//float xOffset = - 0.09797;		// 11GeV
	//float yOffset = 0.2274;			// 11GeV
	//float xOffset = - 0.3685;		// 19GeV
	//float yOffset = 0.03097;		// 19GeV
	float xOffset = 0.0;			// 15GeV
	float yOffset = 0.89;			// 15GeV

	float vX = event->vertexX() + xOffset;
	float vY = event->vertexY() + yOffset;
	double vR2 = vX*vX + vY*vY;
	if ( TMath::Sqrt( vR2 ) > cut_vR->max )
		return false;
	passEventCut( "vR");

	int rId = event->runId();
	if ( gid < 0 )
		rId = 15055000;
	refmultCorrUtil->init( rId );
	refmultCorrUtil->initEvent(event->nUncorrectedPrimaries(), event->vertexZ() );

	corrRefMult 	= refmultCorrUtil->getRefMultCorr();
	cent9  			= refmultCorrUtil->getCentralityBin9();
	if ( cent9 < 0 )
		cent9 = 50;

	cent16  		= refmultCorrUtil->getCentralityBin16();
	if ( cent16 < 0 )
		cent16 = 50;
	eventWeight  	= refmultCorrUtil->getWeight();
	runId 			= event->runId(); 

	// The Post event cuts hook
	postEventCuts();

	return true;
}



Bool_t StRcpMiniMcSkimmer::keepTrack( StMiniMcPair * track ){
	//LOG_INFO << "StRcpMiniMcSkimmer::keepTrack( " << iNode << " )" << endm;

	passTrackCut("All");

	float ptRatio = track->ptGl() / track->ptPr();


	/**
	 * Pre Cut Hook
	 */
	preTrackCuts( track );

	// one plc at a time please
	// gid = -1 for UrQMD with lots of plcs
	if ( gid >= 0 && track->geantId() != gid )
		return false;
	if ( gid >= 0 && track->charge() != plcCharge() )
		return false;


	if ( track->fitPts() < cut_nHitsFit->min )
		return false;
	passTrackCut( "nHitsFit" );

	if ( (double)track->fitPts() / track->nPossiblePts() < cut_nHitsRatio->min )
		return false;
	passTrackCut( "nHitsRatio" );

	if ( track->dedxPts() < cut_nHitsDedx->min ) 
		return false;
	passTrackCut( "nHitsDedx" );

	if ( ptRatio < cut_ptRatio->min || ptRatio > cut_ptRatio->max )
		return false;
	passTrackCut( "ptRatio" );

	if ( track->ptPr() < cut_pt->min )
		return false;
	passTrackCut( "pmmtm" );
	
	if (track->dcaGl() > cut_dca->max )
		return false;
	passTrackCut( "DCA" );

	/**
	 * Post Cut Hook
	 */
	postTrackCuts( track );

	return true;

}



double StRcpMiniMcSkimmer::plcMass( ){

	if ( 8 == gid || 9 == gid )
		return 0.1395702; 			// pi+ or pi-
	if ( 11 == gid || 12 == gid )
		return 0.493677;			// k+ or k-
	if ( 14 == gid || 15 == gid )
		return 0.938272;			// proton antiproton
	cout << "ERROR UNKOWN : " << gid << endl;
	return -1;
}

int StRcpMiniMcSkimmer::plcCharge(){
	switch ( gid ) {

		case 8 : // pi +
			return 1;
		case 9 : // pi -
			return -1;
		case 11 : // k +
			return 1;
		case 12 : // k -
			return -1;
		case 14 : // p
			return 1;
		case 15 : // pbar
			return -1;
		default : 
			return 0;
	}
	return 0;
}

string StRcpMiniMcSkimmer::plcName(  ){
	if ( 8 == gid )
		return "Pi_p";
	if ( 9 == gid )
		return "Pi_n";

	if ( 11 == gid )
		return "K_p";
	if ( 12 == gid )
		return "K_n";

	if ( 14 == gid )
		return "P_p";
	if ( 15 == gid )
		return "P_n";
	return "UNKOWN";
}



double StRcpMiniMcSkimmer::rapidity( double p, double pz ){

	double mass = plcMass();
	double energy = sqrt( p*p + mass * mass );
	return 0.5 * log( (energy + pz) / (energy - pz) );
}


