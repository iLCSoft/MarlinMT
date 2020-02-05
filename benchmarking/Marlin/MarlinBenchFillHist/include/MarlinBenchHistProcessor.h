#pragma once

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
#include <AIDA/IHistogram1D.h>
#endif // MARLIN_USE_AIDA



using namespace lcio ;
using namespace marlin ;

class MarlinBenchHistProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new MarlinBenchHistProcessor ; }
  
  
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
  std::vector<AIDA::ICloud1D*> _hMCPEnergy {} ; 
  int _nBins{0}, _nHist{0}, _nFills{0};
} ;




