#include <exception>
#include <vector>
#include <mutex>
#include <memory>
#include <cmath>
#include <fstream>
#include <cstdlib>

#include <marlinmt/Processor.h>
#include <marlinmt/ProcessorApi.h>
#include <marlinmt/PluginManager.h>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>

#include <IMPL/LCEventImpl.h>
#include <IMPL/LCRunHeaderImpl.h>

#include <ROOT/RHist.hxx>

std::size_t getTotalVirtualMemoryUsed() {
  std::ifstream file("/proc/self/status");
  std::size_t res = 1;
  std::string str;
  while(std::getline(file, str)) {
    if(strncmp(str.c_str(), "VmSize:", 7) == 0) {
      std::cout << "HIT\n";
      const char* c = str.c_str() + 7;  
      while(*(++c) == ' ');
      res = atoll(c);
      break;
    }
  }
  return res;
}


using namespace marlinmt;

double getValue(EVENT::MCParticle* mcp, int hist); 

std::string histName(std::size_t i); 
double axisMin(std::size_t i, int a) ;
double axisMax(std::size_t i, int a) ; 

class MarlinMTHistFillingFromDST : public Processor {
public:
  MarlinMTHistFillingFromDST() : Processor("MarlinMTHistFillingFromDST") {} 
  void init() final override;
  void processEvent( EventStore * evt ) final override;
  void end() final override;
private:
  using SizeTParameter = Parameter<std::size_t> ;
  SizeTParameter _nBins 
    {*this, "NBins", "number of bins per Histogram.", 1000};
  SizeTParameter _nHists10
    {*this, "NHists10", "number of Histograms = 10 ^ NHists10", 2};
  SizeTParameter _nFills
    {*this, "NFills", "number of Fills per Histogram per event", 10};
  SizeTParameter _at 
    {*this, "AccessType", "type of accessing the histograms for filling.", 0};
  BoolParameter        _useMutex 
    {*this, "UseMutex", "use a shared instance of Histogram and lock it with a mutex"};
  std::vector<std::unique_ptr<std::mutex>> _mutex{};
  std::vector<H1FEntry> _histograms{};
  std::vector<ROOT::Experimental::RH1F> _rHistograms{};
  std::size_t _nHists;

  inline void continuesFill(EVENT::LCCollection* mcp,
    std::vector<H1FHandle>& handles); 
  inline void rotatingFill(EVENT::LCCollection* mcp,
    std::vector<H1FHandle>& handles); 
};


void MarlinMTHistFillingFromDST::init() {
  std::size_t memStart = getTotalVirtualMemoryUsed();
  _nHists = powl(10, _nHists10);
  if( _useMutex ) {
    if ( not _rHistograms.empty() ) return;
    for(std::size_t i = 0; i < _nHists; ++i) {
      _mutex.emplace_back(std::make_unique<std::mutex>());
      _rHistograms.emplace_back(
          histName(i),
          ROOT::Experimental::RAxisConfig (
            "x", _nBins, 
            axisMin(i, 0), axisMax(i, 0)));
    }
  } else {
    if ( not _histograms.empty() ) return;
    for(std::size_t i = 0; i < _nHists; ++i) {
      _histograms.push_back(
        ProcessorApi::Book::bookHist1F(
          this, "/", histName(i), "a histogram",
          { "x", book::details::safe_cast<int, std::size_t>(_nBins), 
            axisMin(i,0), axisMax(0,1)}));
    }
  } 
  std::size_t memEnd = getTotalVirtualMemoryUsed();
  streamlog_out(MESSAGE) << "mem usage raised from: " << memStart << " to "
    << memEnd << "\td=" << (memEnd - memStart) << '\n';
}



void MarlinMTHistFillingFromDST::processEvent(EventStore * e) {
  IMPL::LCEventImpl* evt = dynamic_cast<IMPL::LCEventImpl*>(
      e->event<EVENT::LCEvent>().get() );
  try {
    if ( evt ) {
      std::vector<H1FHandle> handles{};
      handles.reserve(_histograms.size());
      for(auto& entry : _histograms) {
        handles.emplace_back(entry.handle());
      }

      EVENT::LCCollection * mcp
        = evt->getCollection("MCParticle"); 

      switch (_at) {
        case 0: rotatingFill(mcp, handles); break;
        case 1: continuesFill(mcp, handles); break;
        default: 
            throw std::runtime_error("not defined accestype");
      }

    }
  } catch ( EVENT::Exception& ) {
    streamlog_out(ERROR) << " failed to process event, LC error\n";
  }
}

void MarlinMTHistFillingFromDST::continuesFill(
    EVENT::LCCollection* mcp, 
    std::vector<H1FHandle>& handles) {
  for (std::size_t i = 0; i < _nHists; ++i) {
    std::unique_ptr<std::lock_guard<std::mutex>> lock
      = _useMutex
        ? std::make_unique<std::lock_guard<std::mutex>>(
            *_mutex[i])
        : nullptr;
    for (int j = 0; j < _nFills; ++j) {
      EVENT::MCParticle* par =
        reinterpret_cast<EVENT::MCParticle*>(
            mcp->getElementAt(j));

      double val = getValue(par, i);
      if (_useMutex) {
        _rHistograms[i].Fill({val}, 1.);
      } else {
        handles[i].fill({val}, 1.);
      }
    }
  } 
}

void MarlinMTHistFillingFromDST::rotatingFill(
    EVENT::LCCollection* mcp, 
    std::vector<H1FHandle>& handles) {
  for( std::size_t j = 0; j < _nFills; ++j) {
    EVENT::MCParticle* par =
      reinterpret_cast<EVENT::MCParticle*>(
        mcp->getElementAt(j));
    for( std::size_t i = 0; i < _nHists; ++i) {
      double val = getValue(par, i);
      if (_useMutex) {
        std::lock_guard lock(*_mutex[i]);
        _rHistograms[i].Fill({val}, 1.);
      } else  {
        handles[i].fill({val}, 1.);
      }
    } 
  }
}

double getValue(
    EVENT::MCParticle* mcp, int hist) 
{
  switch (hist % 10) {
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

std::string histName(std::size_t i) {
  return std::to_string(i);
}
double axisMin(std::size_t i, int a) {
  return -1;
}
double axisMax(std::size_t i, int a) {
  return 1;
}

void MarlinMTHistFillingFromDST::end() {
  streamlog_out(MESSAGE) << "MemAtEnd: " << getTotalVirtualMemoryUsed() << '\n';
}

MARLINMT_DECLARE_PROCESSOR( MarlinMTHistFillingFromDST);
