#pragma once

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>

#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
#include <AIDA/IHistogram1D.h>

#include <TH1.h>



using namespace lcio ;
using namespace marlin ;

class MarlinBenchHistProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new MarlinBenchHistProcessor{} ; }
  
  
  MarlinBenchHistProcessor() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;
  
  
 protected:
  std::vector<TH1F*> _hMCPEnergy {} ; 
  int _nBins{0}, _nHist10{0}, _nHist{0}, _nFills{0}, _accessType{0};
} ;




