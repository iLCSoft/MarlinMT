#pragma once

// -- std heared
#include <memory>
#include <functional>


namespace marlin {
  namespace book {

    class Modifier;

    /**
     *  @brief Handle for objects stored in bookStore.
     */
    template<class T>
    class Handle {
    public:
      using GetFn_t = std::function<const typename T::Type&(std::size_t)>;

      /**
       *  @brief Constructo.
       *  @param id of the Entry in the BookStore
       *  @param getFn functor to get the merged object.
       */
      Handle(
        const std::shared_ptr<Modifier>& pModifier,
        std::size_t id,
        GetFn_t getFn
      ) : _id {id},
          _modifier 
            {std::static_pointer_cast<typename T::Modifier>( pModifier )},
          _getFn {getFn} 
      {}

      /**
       *  @brief Get Finalized object.
       *  @returns const& to Final objet.
       */
      const typename T::Type& get() {
        return _getFn( _id ) ;
      };

      /**
       *  @brief Returns Modifier.
       */
      std::shared_ptr< typename T::Modifier > operator->() {
        return _modifier ;
      }

    private:
      /// id from entry in BookStrore.
      std::size_t                           _id       {0}       ;
      /// Modifier to modifie object. 
      std::shared_ptr<typename T::Modifier> _modifier {nullptr} ;
      /// Functor to get merged object.
      GetFn_t                               _getFn    {}        ;
    };

  } // end namespace book
} // end namespace marlin
