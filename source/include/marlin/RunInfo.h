#pragma once

// -- std headers
#include <string>

// -- marlin headers
#include <marlin/Extensions.h>
#include <marlin/StringParameters.h>

namespace marlin {

  /**
   *  @brief  RunInfo class.
   *
   *  Provide a simple interface describing a run.
   *  The basic metadata are run number, detector name
   *  and description and can be extended with more parameters.
   *  The parameters are real properties of a run whereas the extensions
   *  can also be attached to a run object but are only valid at runtime.
   */
  class RunInfo {
  public:
    RunInfo() = default ;
    ~RunInfo() = default ;
    RunInfo(const RunInfo &) = delete ;
    RunInfo &operator=(const RunInfo &) = delete ;
    RunInfo(RunInfo &&) = default ;
    RunInfo &operator=(RunInfo &&) = default ;

    /**
     *  @brief  Get the run number
     */
    int runNumber() const ;

    /**
     *  @brief  Set the run number
     *
     *  @param  n the run number
     */
    RunInfo &setRunNumber( int n ) ;

    /**
     *  @brief  Get the run description
     */
    const std::string &description() const ;

    /**
     *  @brief  Set the run description
     *
     *  @param  desc the run description
     */
    RunInfo &description( const std::string &desc ) ;

    /**
     *  @brief  Get the detector name
     */
    const std::string &detectorName() const ;

    /**
     *  @brief  Set the detector name
     *
     *  @param  det the detector name
     */
    RunInfo &setDetectorName( const std::string &det ) ;

    /**
     *  @brief  Get the run parameters
     */
    const StringParameters &parameters() const ;

    /**
     *  @brief  Get the run parameters
     */
    StringParameters &parameters() ;

    /**
     *  @brief  Get the run extensions
     */
    Extensions &extensions() ;

    /**
     *  @brief  Get the run extensions
     */
    const Extensions &extensions() const ;

  private:
    /// The run number
    int                         _runNumber {0} ;
    /// The detector name
    std::string                 _detectorName {} ;
    /// The run description
    std::string                 _description {} ;
    /// The underlying event store implementation
    StringParameters            _parameters {} ;
    /// The event extensions
    Extensions                  _extensions {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  inline int RunInfo::runNumber() const {
    return _runNumber ;
  }

  //--------------------------------------------------------------------------

  RunInfo &RunInfo::setRunNumber( int n ) {
    _runNumber = n ;
    return *this ;
  }

  //--------------------------------------------------------------------------

  const std::string &RunInfo::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  RunInfo &RunInfo::description( const std::string &desc ) {
    _description = desc ;
    return *this ;
  }

  //--------------------------------------------------------------------------

  const std::string &RunInfo::detectorName() const {
    return _detectorName ;
  }

  //--------------------------------------------------------------------------

  RunInfo &RunInfo::setDetectorName( const std::string &det ) {
    _detectorName = det ;
    return *this ;
  }

  //--------------------------------------------------------------------------

  const StringParameters &RunInfo::parameters() const {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  StringParameters &RunInfo::parameters() {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  inline Extensions &RunInfo::extensions() {
    return _extensions ;
  }

  //--------------------------------------------------------------------------

  inline const Extensions &RunInfo::extensions() const {
    return _extensions ;
  }

}
