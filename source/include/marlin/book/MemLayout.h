#pragma once

#include <memory>
#include <functional>
#include <utility>
#include <iostream>

class MemLayout {
public:

   template <typename T>
   std::shared_ptr<const T> at( std::size_t idx ) const {
     return std::static_pointer_cast<const T>( _at( idx ) ) ;
   }

   template <typename T>
   std::shared_ptr<T> at( std::size_t idx ) {
     return std::static_pointer_cast<T>( _at( idx ) ) ;
   }

   template <typename T>
   std::shared_ptr<const T> merged() {
     return std::static_pointer_cast<const T>( _merged() ) ;
   }
  
   virtual ~MemLayout() = default;

protected:
   virtual std::shared_ptr<void> _at( std::size_t idx ) const= 0 ;
   virtual std::shared_ptr<const void> _merged() = 0 ;
};


template 
  <typename T,
  void(*MERGE)(std::shared_ptr<T>& /* dst */, std::shared_ptr<T>& /* src */),
  typename ... Args_t>
class SharedMemLayout : public MemLayout {
public:
  SharedMemLayout(std::size_t num_instances, Args_t ... args)
    : _objects{num_instances, nullptr},
        _ctor_p{args ...} 
  {
  }
  ~SharedMemLayout() override = default;

private:
  std::shared_ptr<void> _at(std::size_t idx) const override {
    if(!_objects[idx]) {
      _objects[idx] = std::make_shared<T>(std::make_from_tuple<T>(_ctor_p));
    }

    return _objects[idx];
  }

  std::shared_ptr<const void> _merged() override {
    _mergedObj = std::make_shared<T>(std::make_from_tuple<T>(_ctor_p)); 

    for(std::shared_ptr<T>& pObj : _objects) {
      if(pObj)
        (*MERGE)(_mergedObj, pObj);
    }
    return _mergedObj;
  }

  mutable std::vector<std::shared_ptr<T>> _objects;
  std::shared_ptr<T> _mergedObj{nullptr};
  std::tuple<Args_t ...> _ctor_p;
};

template <typename T>
class SingleMemLayout : public MemLayout {
public:
   SingleMemLayout() {

   }
   ~SingleMemLayout() override = default;

private:
   std::shared_ptr<void> _at( std::size_t /*idx*/ ) const override {
     return _object ;
   }

   std::shared_ptr<const void> _merged() override {
     return _object ;
   }

   std::shared_ptr<T>          _object {nullptr} ;
};

