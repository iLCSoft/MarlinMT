#pragma once

// -- std includes
#include <functional>
#include <memory>
#include <typeinfo>

// -- MarlinBook includes
#include "marlin/book/MemLayout.h"

namespace marlin {
  namespace book {

    // -- MarlinBook forward declaration
    class BookStore ;
    class MemLayout ;

    /**
     *  @brief class which basic functionality for every handle.
     *  @tparam T type which should be handled.
     */
    template < typename T >
    class BaseHandle {
      std::shared_ptr< MemLayout > _mem ;
      std::shared_ptr< T >         _obj ;

    protected:
      BaseHandle( const std::shared_ptr< MemLayout > &mem,
                  const std::shared_ptr< T >         &obj )
        : _mem{mem}, _obj{obj} {}

      /// get access to Object. Used by children to abstract storage.
      T &get() { return *_obj; }

    public:
      /**
       *  @brief get final object.
       *  access the final version of the Object.
       *  @attention this action is not thread save.
       *  @note the finalization is may expensive.
       *  @return object which is contains data from every handle.
       */
      const T &merged() { return *_mem->template merged< T >(); }
    } ;

    /**
     *  @brief vanilla Handle.
     *  @tparam T type which should be handled.
     */
    template < typename T >
    class Handle : public BaseHandle< T > {
    public:
      /// constructor
      Handle( const std::shared_ptr< T >         &obj,
              const std::shared_ptr< MemLayout > &mem )
        : BaseHandle< T >( obj, mem ) {}
    } ;

  } // end namespace book
} // end namespace marlin
