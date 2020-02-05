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
    registerProcessorParameter("NHists", "number of histograms to create.", _nHist, 5);
    registerProcessorParameter("NFills", "number of fill operations per event.", _nFills,  1000);
    _description 
      = "MarlinBenchHistProcessor creates and fills histograms to compare with MarlinMT" ;
}



void MarlinBenchHistProcessor::init() { 
    streamlog_out(DEBUG) << "   init called  " << std::endl ;
    printParameters() ;


    for(int i = 0; i < _nHist; ++i) {
      _hMCPEnergy.push_back(AIDAProcessor::histogramFactory(this)->
          createCloud1D( "hMCPEnergy", "energy of the MCParticles", _nBins, "autoconvert=true" ) ) ; 
    }
    Global::EVENTSEEDER->registerProcessor(this);
}


void MarlinBenchHistProcessor::processRunHeader( LCRunHeader* /*run*/) { 

} 



void MarlinBenchHistProcessor::processEvent( LCEvent * /*evt*/ ) { 
  auto seed = Global::EVENTSEEDER->getSeed(this);
  std::mt19937 generator{ seed };
  std::normal_distribution<float> distributionV{0, 10.f};
  auto itr = _hMCPEnergy.begin();
  for( int i = 0; i < _nFills; ++i) {
    (*itr)->fill({distributionV(generator)}, 1.);
    if(++itr == _hMCPEnergy.end()) {
      itr = _hMCPEnergy.begin();
    }
  }
}



void MarlinBenchHistProcessor::check( LCEvent * /*evt*/ ) { 
}


void MarlinBenchHistProcessor::end(){ 
}

