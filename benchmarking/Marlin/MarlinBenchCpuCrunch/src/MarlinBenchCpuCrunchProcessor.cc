#include "MarlinBenchCpuCrunchProcessor.h"
#include <iostream>
#include <random>
#include <chrono>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>


#include <marlin/ProcessorEventSeeder.h>
#include <marlin/Global.h>
// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"


using namespace lcio ;
using namespace marlin ;

class clock {
public:
  using clock_type = std::chrono::steady_clock;
  using time_point = clock_type::time_point;
  using duration_rep = float;

  using seconds = std::chrono::duration<duration_rep>;
  using milliseconds = std::chrono::duration<duration_rep, std::milli>; 

  clock() = delete;
  ~clock() = delete;

  static time_point now() {
    return clock_type::now();
  }

  template <class unit = seconds>
  static duration_rep time_difference(const time_point &older, const time_point &ealier ) {
    return std::chrono::duration_cast<unit>(ealier - older).count(); 
  }

  template<class unit = seconds>
  static void crunchFor( duration_rep crunchTime ) {
    auto start = now();
    auto now = start;
    duration_rep timediff = 0;
    do {
      (void)std::sqrt(timediff);
      now = clock::now();
      timediff = clock::time_difference<unit>(start, now);
    } while( timediff < crunchTime );
  }
};


MarlinBenchCpuCrunchProcessor aMarlinBenchCpuCrunchProcessor ;


MarlinBenchCpuCrunchProcessor::MarlinBenchCpuCrunchProcessor() : Processor("MarlinBenchCpuCrunchProcessor") {
  registerProcessorParameter("CrunchTime",
      "The crunching time (unit ms)", _crunchTime, 200);
  registerProcessorParameter("CrunchSigma",
      "Smearing factor on the crunching time using a gaussian generator (unit ms)", 
      _crunchSigma, 0.f);
}




void MarlinBenchCpuCrunchProcessor::init() { 
  Global::EVENTSEEDER->registerProcessor(this);
  printParameters();
}


void MarlinBenchCpuCrunchProcessor::processRunHeader( LCRunHeader* /*run*/) { 
} 



void MarlinBenchCpuCrunchProcessor::processEvent( LCEvent * /*evt*/ ) { 
  auto seed = Global::EVENTSEEDER->getSeed(this);
  std::default_random_engine generator( seed );
  std::normal_distribution<clock::duration_rep> distribution(0, _crunchSigma);

  clock::duration_rep totalCrunchTime
    = static_cast<clock::duration_rep>(_crunchTime)
    + distribution(generator);

  clock::crunchFor<clock::milliseconds>(totalCrunchTime);
}


void MarlinBenchCpuCrunchProcessor::check( LCEvent * /*evt*/ ) { 
}


void MarlinBenchCpuCrunchProcessor::end(){ 
}

