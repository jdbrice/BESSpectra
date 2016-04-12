#include "XmlFunction.h"
#include "Utils.h"


ClassImp( jdb::XmlFunction );

namespace jdb {

	int XmlFunction::instances = 0; 



	XmlFunction::XmlFunction( XmlConfig * cfg, string nodePath, string pMod, string eMod, string formMod, string covMod ){

		string formula = cfg->getString( nodePath + formMod );
		func = (new TF1( ("xmlfunction_" + ts(instances)).c_str(), formula.c_str() ) );
	 
		sampleMin = 0;
		sampleMax = 0;

		int p = 0;
		while ( cfg->exists( nodePath + pMod + ts(p) ) ){
			double val = cfg->getDouble( nodePath + pMod + ts(p) );
			
			func->SetParameter( p, val );

			// set errors if they are there
			if ( cfg->exists( nodePath + eMod + ts(p) ) ){
				double error = cfg->getDouble( nodePath + eMod + ts(p) );
				func->SetParError( p, error );

				DEBUG( "XmlFunction", "p" << p << " = " << val << " +/- " << error )
			} else {
				DEBUG( "XmlFunction", "p" << p << " = " << val )
			}

			p++;
		}

		if ( cfg->exists( nodePath + covMod ) ){
			cov = cfg->getDoubleVector( nodePath + covMod );
			DEBUG( "XmlFunction", "cov array length : " << cov.size() );
		}


		if ( cfg->exists( nodePath + ":min" ) && cfg->exists( nodePath + ":max" ) ){
			sampleMin = cfg->getDouble( nodePath + ":min" );
			sampleMax = cfg->getDouble( nodePath + ":max" );
			func->SetRange( sampleMin, sampleMax );

			DEBUG( "XmlFunction", "Range(" << sampleMin << ", " << sampleMax << "" );
		}

	}
}
