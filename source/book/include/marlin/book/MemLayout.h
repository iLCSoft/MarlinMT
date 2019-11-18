#pragma once

// -- std header
#include <memory>
#include <utility>
#include <vector>

namespace marlin {
  namespace book {

    /**
     *  @brief MemLayout base class to store booked objects in MarlinMT
     */
    class MemLayout {
    public:
      /**
       *  @brief Get Resource for an instance.
       *  @param idx instance id
       *  @return const pointer to Resource
       */
      template < typename T >
      std::shared_ptr< const T > at( std::size_t idx ) const {
        return std::static_pointer_cast< const T >( impAt( idx ) ) ;
      }

      /**
       *  @brief Get Resource for an instance.
       *  @param idx instance id
       *  @return pointer to Resource
       */
      template < typename T >
      std::shared_ptr< T > at( std::size_t idx ) {
        return std::static_pointer_cast< T >( impAt( idx ) ) ;
      }

      /**
       *  @brief Get Completed Resource. Same for every Instance.
       *  @note may recalculation for every call
       */
      template < typename T >
      std::shared_ptr< const T > merged() {
        return std::static_pointer_cast< const T >( impMerged() ) ;
      }

      virtual ~MemLayout() = default ;

    protected:
      /// implementation from at
      virtual std::shared_ptr< void > impAt( std::size_t idx ) const = 0 ;
      /// implementation from merged
      virtual std::shared_ptr< void > impMerged() = 0 ;
    } ;

    /**
     *  @brief MemLayout for mutable object instances.
     *  @tparam T stored Object Type
     *  @tparam MERGE function(to, from) which merge to instances of Object
     */
    template < typename T,
               void ( *MERGE )( const std::shared_ptr< T > & /* dst */,
                                const std::shared_ptr< T > & /* src */
                                ),
               typename... Args_t >
    class SharedMemLayout : public MemLayout {

    public:
      /**
       *  @brief Constructor.
       *  @param amount of instances amount of Resource Instances
       *  @param args Arguments for Object Construction
       */
      SharedMemLayout( std::size_t num_instances, Args_t... args )
        : _objects{num_instances, nullptr}, _ctor_p{args...} {
        for ( std::shared_ptr< T > &pObj : _objects ) {
          pObj = std::make_shared< T >( args... ) ;
        }
      }

      ~SharedMemLayout() override = default ;

    private:
      /// Get Resource for Instance. Lazy operation.
      std::shared_ptr< void > imp_at( std::size_t idx ) const override final {
        return _objects[idx] ;
      }

      std::shared_ptr< void > imp_merged() override final {
        _mergedObj
          = std::make_shared< T >( std::make_from_tuple< T >( _ctor_p ) ) ;
        for ( std::shared_ptr< T > &pObj : _objects ) {
          if ( pObj ) {
            ( *MERGE )( _mergedObj, pObj ) ;
          }
        }
        return _mergedObj ;
      }

      std::vector< std::shared_ptr< T > >
                              _objects; ///< mutable for lazy operation
      std::shared_ptr< T >    _mergedObj{nullptr} ;
      std::tuple< Args_t... > _ctor_p ;
    } ;

    /**
     *  @brief MemLayout for Single object instance.
     *  @tparam T stored Object Type
     */
    template < typename T, typename... Args_t >
    class SingleMemLayout : public MemLayout {
    public:
      SingleMemLayout( Args_t... args )
        : _object{std::make_shared< T >( args... )} {}

      ~SingleMemLayout() override = default ;

    private:
      std::shared_ptr< void >
      imp_at( std::size_t /*idx*/ ) const override final {
        return _object ;
      }

      /// @note cheap merge
      std::shared_ptr< void > imp_merged() override final { return _object; }

      std::shared_ptr< T > _object{nullptr} ;
    } ;

  } // end namespace book
} // end namespace marlin
