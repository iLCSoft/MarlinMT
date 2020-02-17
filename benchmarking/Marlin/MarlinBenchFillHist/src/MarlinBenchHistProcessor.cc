#include "MarlinBenchHistProcessor.h"
#include <iostream>
#include <random>
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <marlin/Global.h>
#include <marlin/ProcessorEventSeeder.h>


// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
#include <AIDA/IHistogram1D.h>
#endif // MARLIN_USE_AIDA


using namespace lcio ;
using namespace marlin ;


MarlinBenchHistProcessor aMarlinBenchHistProcessor ;


MarlinBenchHistProcessor::MarlinBenchHistProcessor() : Processor("MarlinBenchHistProcessor") {
    registerProcessorParameter("NBins", "number of bins per histogram.", _nBins, 1000);
    registerProcessorParameter("NHists10", "number of histograms to create.", _nHist10, 0);
    registerProcessorParameter("NFills", "number of fill operations per event.", _nFills,  1000);
    registerProcessorParameter("AccessType", "type to access histograms", _accessType, -1);
    _description 
      = "MarlinBenchHistProcessor creates and fills histograms to compare with MarlinMT" ;
}



void MarlinBenchHistProcessor::init() { 
    streamlog_out(DEBUG) << "   init called  " << std::endl ;
    printParameters() ;

    _nHist = static_cast<int>(powl(10, _nHist10));
    for(int i = 0; i < _nHist; ++i) {
      _hMCPEnergy.push_back(AIDAProcessor::histogramFactory(this)->
          createCloud1D( "hMCPEnergy", "energy of the MCParticles", _nBins, "autoconvert=true" ) ) ; 
    }
}


void MarlinBenchHistProcessor::processRunHeader( LCRunHeader* /*run*/) { 

} 

double getValue(MCParticle* mcp, int hist) 
{
  switch (hist%10) {
    case 0: return mcp->getEnergy();  
    case 1: return mcp->getTime();
    case 2: return mcp->getSpin()[0];
    case 3: return mcp->getSpin()[1];
    case 4: return mcp->getMass();
    case 5: return mcp->getMomentum()[0];
    case 6: return mcp->getMomentum()[1];
    case 7: return mcp->getMomentum()[2];
    case 8: return mcp->getSpin()[2];
    case 9: return mcp->getCharge();
  }
  throw std::runtime_error("My math bad ...");
}



void MarlinBenchHistProcessor::processEvent( LCEvent * evt ) { 
  LCCollection* col = evt->getCollection( "MCParticle" ) ;
  if ( !col ) throw std::runtime_error("null collection");

  if (_accessType == 1) {
    for (int j = 0; j < _nHist; ++j) {
      for (int i = 0; i < _nFills; ++i) {
        MCParticle *mcp = dynamic_cast<MCParticle*>(col->getElementAt(i));
        if (!mcp) throw std::runtime_error("null mcp");
        _hMCPEnergy[j]->fill({getValue(mcp, j)},1.);
      }
    }

  }
  else if (_accessType == 0) { // rotating 
    for( int i = 0; i < _nFills; ++i) {
      for (int j = 0; j < _nHist; ++j) {
        MCParticle *mcp = dynamic_cast<MCParticle*>(col->getElementAt(i));
        if (!mcp) throw std::runtime_error("null mcp");
        _hMCPEnergy[j]->fill({getValue(mcp, j)}, 1.);
      }
    }
  } else {
    throw std::runtime_error("unkown access Type");
  }
}



void MarlinBenchHistProcessor::check( LCEvent * /*evt*/ ) { 
}


void MarlinBenchHistProcessor::end(){ 
}

