#ifndef MARLIN_PARAMETER_h
#define MARLIN_PARAMETER_h 1

// -- std headers
#include <iostream>
#include <string>
#include <sstream>
#include <typeindex>
#include <memory>
#include <vector>
#include <functional>

// -- marlin headers
#include <marlin/Utils.h>

namespace marlin {
  
  /**
   *  @brief  EParameterType enumerator
   *  Enumerates parameter types supported by Marlin
   */
  enum class EParameterType {
    eSimple,              /// Simple (scalar) parameter
    eVector               /// Vector parameter
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  ParameterImpl class
   *  Abstract internal implementation of a parameter
   */
  class ParameterImpl {
  public:
    ParameterImpl() = delete ;
    ~ParameterImpl() = default ;
    
    /**
     *  @brief  Constructor
     *  
     *  @param  paramType the parameter type
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  addr the address of the parameter variable
     */
    template <typename T>
    inline ParameterImpl( EParameterType paramType, const std::string &na, const std::string & desc, std::shared_ptr<T> addr ) :
      _type(paramType),
      _name(na),
      _description(desc),
      _value(addr),
      _typeIndex(typeid(T)) {
      _typeFunction = [] { return details::type_info<T>::type ; };
      _resetFunction = [this] { *std::static_pointer_cast<T>( _value ).get() = T() ; };
      _strFunction = [this] { return details::convert<T>::to_string( get<T>() ) ; };
      _fromStrFunction = [this] ( const std::string &value ) { 
        *std::static_pointer_cast<T>( _value ).get() = details::convert<T>::from_string( value ) ; 
      };
    }
    
    /**
     *  @brief  Get the parameter name
     */
    EParameterType type() const ;
        
    /**
     *  @brief  Get the parameter name
     */
    const std::string& name() const ;
    
    /**
     *  @brief  Get the parameter description
     */
    const std::string& description() const ;
    
    /**
     *  @brief  Whether the parameter has been set
     */
    bool isSet() const ;
    
    /**
     *  @brief  Get the parameter value as string
     */
    std::string str() const ;
    
    /**
     *  @brief  Set the parameter value from a string
     */
    void str( const std::string &value ) const ;
    
    /**
     *  @brief  Get the parameter type as string
     */
    std::string typeStr() const ;
    
    /**
     *  @brief  Get a type index object of the underlying type
     */
    const std::type_index &typeIndex() const ;
    
    /**
     *  @brief  Check whether the template parameter matches the internal implementation type
     */
    template <typename T>
    inline bool isType() const {
      return ( std::type_index(typeid(T)) == _typeIndex ) ;
    }
    
    /**
     *  @brief  Throw an exception if the internal type doesn't match the template parameter type 
     */
    template <typename T>
    inline void checkType() const {
      if( not isType<T>() ) {
        MARLIN_THROW( "checkType failure. Given: " + std::string(details::type_info<T>::type) + ", stored: " + typeStr() ) ;
      }
    }
    
    /**
     *  @brief  Set the parameter value.
     *  Throw if the type is not matching the one on creation
     *  
     *  @param  val the parameter value to set 
     */
    template <typename T>
    inline void set( const T &val ) {
      checkType<T>() ;
      *std::static_pointer_cast<T>( _value ).get() = val ;
      _isSet = true ;
    }
    
    /**
     *  @brief  Get the parameter value. The type must match the one on creation.
     *  Throw if the parameter is not set
     */
    template <typename T>
    inline T get() const {
      checkType<T>() ;
      if( not isSet() ) {
        MARLIN_THROW( "Parameter '" + name() +  "' not set" ) ;
      }
      return *std::static_pointer_cast<T>( _value ).get() ;
    }

    /**
     *  @brief  Get the parameter value. The type must match the one on creation.
     *  If the parameter is not set, return the fallback value.
     *  
     *  @param  fallback the fallback value if the parameter is not set
     */
    template <typename T>
    inline T get( const T &fallback ) const {
      checkType<T>() ;
      if( not isSet() ) {
        return fallback ;
      }
      return *std::static_pointer_cast<T>( _value ).get() ;
    }
    
    /**
     *  @brief  Reset the parameter value
     */
    void reset() ;
    
  private:
    /// The parameter type
    EParameterType                               _type {} ;
    /// The parameter name
    std::string                                  _name {} ;
    /// The parameter description
    std::string                                  _description {} ;
    /// The function converting the parameter type to string
    std::function<std::string()>                 _typeFunction {} ;
    /// The function converting the parameter value to string
    std::function<std::string()>                 _strFunction {} ;
    ///
    std::function<void(const std::string &)>     _fromStrFunction {} ;
    /// The function resetting the parameter value
    std::function<void()>                        _resetFunction {} ;
    /// Whether the parameter is set
    bool                                         _isSet {false} ;
    /// The address to the parameter value
    std::shared_ptr<void>                        _value {nullptr} ;
    /// The type index object of the underlying parameter type
    std::type_index                              _typeIndex ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  Configurable class
   *  Interface for configuring components in the framework
   */
  class Configurable {
  public:
    Configurable() = default ;
    virtual ~Configurable() = default ;
    
    using ParameterMap = std::map<std::string, std::shared_ptr<ParameterImpl>> ;
    using iterator = ParameterMap::iterator ;
    using const_iterator = ParameterMap::const_iterator ;
    
    /**
     *  @brief  Add a parameter. Throw if already exists.
     *  
     *  @param  paramType the parameter type
     *  @param  name the parameter name
     *  @param  desc the parameter description
     *  @param  value the address to the parameter value
     */
    template <typename T>
    inline std::shared_ptr<ParameterImpl> addParameter( EParameterType paramType, const std::string &name, const std::string &desc, std::shared_ptr<T> value ) {
      checkParameter( name ) ;
      auto param = std::make_shared<ParameterImpl>( paramType, name, desc, value ) ;
      _parameters[ name ] = param ;
      return param ;
    }
    
    /**
     *  @brief  Get a parameter value
     * 
     *  @param  name the parameter name to get
     */
    template <typename T>
    inline T parameter( const std::string &name ) const {
      checkParameter( name ) ;
      return _parameters.find( name )->second->get<T>()  ;
    }
    
    /**
     *  @brief  Get a parameter value. Returns the fallback value if the parameter is not set.
     *  Throw an exception if the parameter is not registered
     * 
     *  @param  name the parameter name
     *  @param  fallback the fallback value if the parameter is not set
     */
    template <typename T>
    inline T parameter( const std::string &name, const T &fallback ) const {
      checkParameter( name ) ;
      return _parameters.find( name )->second->get<T>( fallback )  ;
    }
    
    /**
     *  @brief  Check if the parameter has been registered
     *  
     *  @param  name the parameter name to check
     */
    void checkParameter( const std::string &name ) const ;

    /**
     *  @brief  Return true if the parameter has been registered
     * 
     *  @param  name the parameter name to check
     */
    bool exists( const std::string &name ) const ;

    /**
     *  @brief  Returns true if the parameter exists and is set, false otherwise
     * 
     *  @param  name the parameter name to check
     */
    bool isSet( const std::string &name ) const ;

    /**
     *  @brief  Remove all parameters
     */
    void clear() ;
    
    /**
     *  @brief  Unset all registered parameters
     */
    void unset() ;
    
    iterator begin() ;
    const_iterator begin() const ;
    iterator end() ;
    const_iterator end() const ;
    
  private:
    /// The parameter map
    ParameterMap               _parameters {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  ParameterBase<T> class
   *  Base interface for user parameters. See daughter classes for usage
   */
  template <typename T>
  class ParameterBase {
  public:
    /// Default destructor
    virtual ~ParameterBase() = default ;
    
    /**
     *  @brief  Constructor
     * 
     *  @param  paramType the parameter type
     *  @param  na the parameter name
     *  @param  desc the parameter description
     */
    inline ParameterBase( EParameterType paramType, const std::string &na, const std::string &desc ) {
      _impl = std::make_shared<ParameterImpl>( paramType, na, desc, _value ) ;
    }
    
    /**
     *  @brief  Constructor
     * 
     *  @param  paramType the parameter type
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  defVal the default parameter value
     */
    inline ParameterBase( EParameterType paramType, const std::string &na, const std::string &desc, const T &defVal ) :
      _defaultValue( defVal ) {
      _impl = std::make_shared<ParameterImpl>( paramType, na, desc, _value ) ;
    }
    
    /**
     *  @brief  Constructor
     * 
     *  @param  conf a configurable object to which the parameter is added
     *  @param  paramType the parameter type
     *  @param  na the parameter name
     *  @param  desc the parameter description
     */
    inline ParameterBase( Configurable &conf, EParameterType paramType, const std::string &na, const std::string &desc ) {
      _impl = conf.addParameter( paramType, na, desc, _value ) ;
    }
    
    /**
     *  @brief  Constructor
     * 
     *  @param  conf a configurable object to which the parameter is added
     *  @param  paramType the parameter type
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  defVal the default parameter value
     */
    inline ParameterBase( Configurable &conf, EParameterType paramType, const std::string &na, const std::string &desc, const T &defVal ) :
      _defaultValue( defVal ) {
      _impl = conf.addParameter( paramType, na, desc, _value ) ;
    }

    /**
     *  @brief  Get the parameter type
     */
    inline EParameterType type() const {
      return _impl->type() ;
    }
    
    /**
     *  @brief  Get the property name
     */
    inline const std::string& name() const {
      return _impl->name() ;
    }
    
    /**
     *  @brief  Get the parameter description
     */
    inline const std::string& description() const {
      return _impl->description() ;
    }
    
    /**
     *  @brief  Whether the parameter has been set
     */
    inline bool isSet() const {
      return _impl->isSet() ;
    }
    
    /**
     *  @brief  Get the parameter value as string
     */
    inline std::string str() const {
      return _impl->str() ;
    }
    
    /**
     *  @brief  Get the parameter type as string
     */
    inline std::string typeStr() const {
      return _impl->typeStr() ;
    }
    
    /**
     *  @brief  Get a type index object of the underlying type
     */
    inline const std::type_index &typeIndex() const {
      return _impl->typeIndex() ;
    }
    
    /**
     *  @brief  Implicit conversion operator to get the parameter value
     */
    inline operator T() const {
      return *_value.get() ;
    }
    
    /**
     *  @brief  Get the parameter value. Throws if the parameter is 
     *  not set and has no default value
     */
    inline T get() const {
      if( isSet() ) {
        return *_value.get() ;
      }
      return _defaultValue.value() ;
    }

    /**
     *  @brief  Reset the parameter value (only)
     */
    inline void reset() {
      _impl->reset() ;
    }
    
  protected:
    /// The parameter value address
    std::shared_ptr<T>             _value { std::make_shared<T>() } ;
    /// The optional default value
    std::optional<T>               _defaultValue {} ;
    /// A shared pointer on the parameter implementation
    std::shared_ptr<ParameterImpl> _impl {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  Parameter<T> class
   *  High level interface to register simple parameter values (int, float, ...).
   */
  template <typename T>
  class Parameter : public ParameterBase<T> {
  public:
    /// Deleted constructor
    Parameter() = delete ;
    /// Default copy constructor
    Parameter( const Parameter<T> & ) = default ;
    /// Default assignement operator
    Parameter<T> &operator=( const Parameter<T> & ) = default ;
    /// Default destructor
    ~Parameter() = default ;

    /**
     *  @brief  Constructor
     *  
     *  @param  na the parameter name
     *  @param  desc the parameter description
     */
    inline Parameter( const std::string &na, const std::string &desc ) :
      ParameterBase<T>( EParameterType::eSimple, na, desc ) {
      /* nop */
    }
    
    /**
     *  @brief  Constructor
     *  
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  defVal the parameter default value
     */
    inline Parameter( const std::string &na, const std::string &desc, const T &defVal ) :
      ParameterBase<T>( EParameterType::eSimple, na, desc, defVal ) {
      /* nop */
    }
    
    /**
     *  @brief  Constructor
     *  
     *  @param  conf the configurable object owning the parameter
     *  @param  na the parameter name
     *  @param  desc the parameter description
     */
    inline Parameter( Configurable &conf, const std::string &na, const std::string &desc ) :
      ParameterBase<T>( conf, EParameterType::eSimple, na, desc ) {
      /* nop */
    }
    
    /**
     *  @brief  Constructor
     *  
     *  @param  conf the configurable object owning the parameter
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  defVal the parameter default value
     */
    inline Parameter( Configurable &conf, const std::string &na, const std::string &desc, const T &defVal ) :
      ParameterBase<T>( conf, EParameterType::eSimple, na, desc, defVal ) {
      /* nop */
    }
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  VectorParameter<T> class.
   *  High level interface to register vector parameters (std::vector<int>, std::vector<float>, ...).
   *  Defines also alias methods to std::vector<T> for easy use
   */
  template <typename T>
  class VectorParameter : public ParameterBase<std::vector<T>> {
  public:
    using Base = ParameterBase<std::vector<T>> ;
    
    /// Deleted constructor
    VectorParameter() = delete ;
    /// Default copy constructor
    VectorParameter( const VectorParameter<T> & ) = default ;
    /// Default assignement operator
    VectorParameter<T> &operator=( const VectorParameter<T> & ) = default ;
    /// Default destructor
    ~VectorParameter() = default ;
    
    /**
     *  @brief  Constructor
     *  
     *  @param  conf the configurable object owning the parameter
     *  @param  na the parameter name
     *  @param  desc the parameter description
     */
    inline VectorParameter( const std::string &na, const std::string &desc ) :
      Base( EParameterType::eVector, na, desc ) {
      /* nop */
    }
    
    /**
     *  @brief  Constructor
     *  
     *  @param  conf the configurable object owning the parameter
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  defVal the parameter default value
     */
    inline VectorParameter( const std::string &na, const std::string &desc, const T &defVal ) :
      Base( EParameterType::eVector, na, desc, defVal ) {
      /* nop */
    }
    
    /**
     *  @brief  Constructor
     *  
     *  @param  conf the configurable object owning the parameter
     *  @param  na the parameter name
     *  @param  desc the parameter description
     */
    inline VectorParameter( Configurable &conf, const std::string &na, const std::string &desc ) :
      Base( conf, EParameterType::eVector, na, desc ) {
      /* nop */
    }
    
    /**
     *  @brief  Constructor
     *  
     *  @param  conf the configurable object owning the parameter
     *  @param  na the parameter name
     *  @param  desc the parameter description
     *  @param  defVal the parameter default value
     */
    inline VectorParameter( Configurable &conf, const std::string &na, const std::string &desc, const T &defVal ) :
      Base( conf, EParameterType::eVector, na, desc, defVal ) {
      /* nop */
    }
    
    // vector const (only) interface aliases
    auto at( typename std::vector<T>::size_type idx ) const { return Base::_value->at(idx) ; }
    auto operator[]( typename std::vector<T>::size_type idx ) const { return *(Base::_value)[idx] ; }
    auto front() const { return Base::_value->front() ; }
    auto back() const { return Base::_value->back() ; }
    auto data() const { return Base::_value->data() ; }
    auto begin() const { return Base::_value->begin() ; }
    auto end() const { return Base::_value->end() ; }
    auto cbegin() const { return Base::_value->cbegin() ; }
    auto cend() const { return Base::_value->cend() ; }
    auto rbegin() const { return Base::_value->rbegin() ; }
    auto rend() const { return Base::_value->rend() ; }
    auto crbegin() const { return Base::_value->crbegin() ; }
    auto crend() const { return Base::_value->crend() ; }
    auto empty() const { return Base::_value->empty() ; }
    auto size() const { return Base::_value->size() ; }
    auto max_size() const { return Base::_value->max_size() ; }
  };
  
  //--------------------------------------------------------------------------

  template <typename T>
  inline std::ostream &operator <<( std::ostream &stream, const Parameter<T> &rhs ) {
    stream << rhs.get() ;
    return stream ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename S>
  inline bool operator ==( const Parameter<T> &lhs, const S &rhs ) {
    return ( lhs.get() == rhs ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename S>
  inline bool operator !=( const Parameter<T> &lhs, const S &rhs ) {
    return ( lhs.get() != rhs ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename S>
  inline bool operator <( const Parameter<T> &lhs, const S &rhs ) {
    return ( lhs.get() < rhs ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename S>
  inline bool operator <=( const Parameter<T> &lhs, const S &rhs ) {
    return ( lhs.get() <= rhs ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename S>
  inline bool operator >( const Parameter<T> &lhs, const S &rhs ) {
    return ( lhs.get() > rhs ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename S>
  inline bool operator >=( const Parameter<T> &lhs, const S &rhs ) {
    return ( lhs.get() >= rhs ) ;
  }
  
  //--------------------------------------------------------------------------
  
  // helper types
  using IntParameter = Parameter<int> ;
  using UIntParameter = Parameter<unsigned int> ;
  using FloatParameter = Parameter<float> ;
  using DoubleParameter = Parameter<double> ;
  using BoolParameter = Parameter<bool> ;
  using StringParameter = Parameter<std::string> ;
  using IntVectorParameter = VectorParameter<int> ;
  using UIntVectorParameter = VectorParameter<unsigned int> ;
  using FloatVectorParameter = VectorParameter<float> ;
  using DoubleVectorParameter = VectorParameter<double> ;
  using BoolVectorParameter = VectorParameter<bool> ;
  using StringVectorParameter = VectorParameter<std::string> ;

} // end namespace marlin
#endif
