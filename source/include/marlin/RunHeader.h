#pragma once

// -- std headers
#include <string>

// -- marlin headers
#include <marlin/Extensions.h>
#include <marlin/StringParameters.h>

namespace marlin {

  /**
   *  @brief  RunHeader class.
   *
   *  Provide a simple interface describing a run.
   *  The basic metadata are run number, detector name
   *  and description and can be extended with more parameters.
   *  The parameters are real properties of a run whereas the extensions
   *  can also be attached to a run object but are only valid at runtime.
   */
  class RunHeader {
  public:
    RunHeader() = default ;
    ~RunHeader() = default ;
    RunHeader(const RunHeader &) = delete ;
    RunHeader &operator=(const RunHeader &) = delete ;
    RunHeader(RunHeader &&) = default ;
    RunHeader &operator=(RunHeader &&) = default ;

    /**
     *  @brief  Get the run number
     */
    int runNumber() const ;

    /**
     *  @brief  Set the run number
     *
     *  @param  n the run number
     */
    RunHeader &setRunNumber( int n ) ;

    /**
     *  @brief  Get the run description
     */
    const std::string &description() const ;

    /**
     *  @brief  Set the run description
     *
     *  @param  desc the run description
     */
    RunHeader &setDescription( const std::string &desc ) ;

    /**
     *  @brief  Get the detector name
     */
    const std::string &detectorName() const ;

    /**
     *  @brief  Set the detector name
     *
     *  @param  det the detector name
     */
    RunHeader &setDetectorName( const std::string &det ) ;

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

  inline int RunHeader::runNumber() const {
    return _runNumber ;
  }

  //--------------------------------------------------------------------------

  inline RunHeader &RunHeader::setRunNumber( int n ) {
    _runNumber = n ;
    return *this ;
  }

  //--------------------------------------------------------------------------

  inline const std::string &RunHeader::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  inline RunHeader &RunHeader::setDescription( const std::string &desc ) {
    _description = desc ;
    return *this ;
  }

  //--------------------------------------------------------------------------

  inline const std::string &RunHeader::detectorName() const {
    return _detectorName ;
  }

  //--------------------------------------------------------------------------

  inline RunHeader &RunHeader::setDetectorName( const std::string &det ) {
    _detectorName = det ;
    return *this ;
  }

  //--------------------------------------------------------------------------

  inline const StringParameters &RunHeader::parameters() const {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  inline StringParameters &RunHeader::parameters() {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  inline Extensions &RunHeader::extensions() {
    return _extensions ;
  }

  //--------------------------------------------------------------------------

  inline const Extensions &RunHeader::extensions() const {
    return _extensions ;
  }

}
