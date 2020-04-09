#pragma once

// -- std headers
#include <string>
#include <map>

// -- marlinmt headers
#include <marlinmt/Extensions.h>
#include <marlinmt/Utils.h>

namespace marlinmt {

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
    using ParameterMap = std::map<std::string, std::string> ;
    
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
    const ParameterMap &parameters() const ;

    /**
     *  @brief  Get the run parameters
     */
    ParameterMap &parameters() ;
    
    /**
     *  @brief  Get a parameter by name
     * 
     *  @param  name the parameter name
     */
    template <typename T>
    std::optional<T> parameter( const std::string &name ) const ;
    
    /**
     *  @brief  Set a run parameter
     * 
     *  @param  name the parameter name
     *  @param  value the parameter value
     *  @return *this
     */
    template <typename T>
    RunHeader &setParameter( const std::string &name, const T &value ) ;

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
    /// The run parameters map
    ParameterMap                _parameters {} ;
    /// The run extensions
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

  inline const RunHeader::ParameterMap &RunHeader::parameters() const {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  inline RunHeader::ParameterMap &RunHeader::parameters() {
    return _parameters ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline std::optional<T> RunHeader::parameter( const std::string &name ) const {
    auto iter = _parameters.find( name ) ;
    if( _parameters.end() != iter ) {
      return details::convert<T>::from_string( iter->second ) ;
    }
    return std::nullopt ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline RunHeader &RunHeader::setParameter( const std::string &name, const T &value ) {
    _parameters[name] = details::convert<T>::to_string( value ) ;
    return *this ;
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
