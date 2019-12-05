#pragma once

namespace marlin {
  namespace book {
    namespace types { 

      enum struct Categories { None, Hist};

      template<typename T>
      struct TypeInfo {
        static constexpr Categories category = Categories::None;
      };

    template<typename T>
    constexpr Categories category_of = TypeInfo<T>::category;

    } // end namespace types

    class BookStore ;
    template < typename T, types::Categories = types::category_of<T>>
    class Handle ;
    template < typename T >
    class BaseHandle ;
  } // end namespace book
} // end namespace marlin
