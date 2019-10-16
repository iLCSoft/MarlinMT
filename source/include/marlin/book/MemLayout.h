   * \return ptr to Resource
   */
    template <typename T>
    std::shared_ptr<T> at( std::size_t idx ) {
      return std::static_pointer_cast<T>( imp_at( idx ) ) ;
    }
=======
namespace marlin::book {
>>>>>>> new book store iteration

  /*! MemLayout BaseClass to store booked objets in MarlinMT 
   */
  class MemLayout {
  public:

    /*! get Resource for a instance
     * \param idx instance id
     * \return const ptr to Resource
     */
    template <typename T>
    std::shared_ptr<const T> at( std::size_t idx ) const {
      return std::static_pointer_cast<const T>( imp_at( idx ) ) ;
    }

    /*! get Resoucre for a instance
     * \param idx instance id
     * \return ptr to Resource
     */
    template <typename T>
    std::shared_ptr<T> at( std::size_t idx ) {
      return std::static_pointer_cast<T>( imp_at( idx ) ) ;
    }

    /*! get Completed Resource. Same for every Instance
     * \note may recalculation for every call
     */
    template <typename T>
    std::shared_ptr<const T> merged() {
      return std::static_pointer_cast<const T>( imp_merged() ) ;
    }
    
    virtual ~MemLayout() = default;

  protected:
    /*! implimentation from at */
    virtual std::shared_ptr<void> imp_at( std::size_t idx ) const= 0 ;
    /*! implimentation from merged */
    virtual std::shared_ptr<void> imp_merged() = 0 ;
  };

  /*! MemLayout for Mutible object instances.
   * \tparam T stored Object Type
   * \tparam MERGE function(to, from) wich merge to instances of Object
   */
  template 
    <typename T,
    void(*MERGE)(std::shared_ptr<T>& /* dst */, std::shared_ptr<T>& /* src */),
    typename ... Args_t>
  class SharedMemLayout : public MemLayout {
  public:
    /*! 
     * \param num_instances amount of Resource Instances
     * \param args Arguments for Object Construction 
     */
    SharedMemLayout(std::size_t num_instances, Args_t ... args)
      : _objects{num_instances, nullptr},
        _ctor_p{args ...}
    {
      for(std::shared_ptr<T>& pObj : _objects) {
        pObj = std::make_shared<T>(args ...);
      }
    }

    ~SharedMemLayout() override = default;

  private:
    /*! get Rescource for Instance. lazy operation*/  
    std::shared_ptr<void> imp_at(std::size_t idx) const override final {
      return _objects[idx];
    }
    
    std::shared_ptr< void> imp_merged() override final {
      _mergedObj = std::make_shared<T>(std::make_from_tuple<T>(_ctor_p)); 
      for(std::shared_ptr<T>& pObj : _objects) {
        if(pObj)
          (*MERGE)(_mergedObj, pObj);
      }
      return _mergedObj;
    }

    std::vector<std::shared_ptr<T>> _objects; ///< mutable for lazy operation
    std::shared_ptr<T> _mergedObj{nullptr};
    std::tuple<Args_t ...> _ctor_p;
  };

  /*! MemLayout for Single object instance.
   * \tparam T stored Object Type
   */
  template <typename T, typename ... Args_t>
  class SingleMemLayout : public MemLayout {
  public:
    SingleMemLayout(Args_t ... args) 
      : _object{std::make_shared<T>(args ...)}
    {}

     ~SingleMemLayout() override = default;

  private:
     std::shared_ptr<void> imp_at( std::size_t /*idx*/ ) const override final {
     return _object ;
     }

     ///\note cheap merge
     std::shared_ptr< void> imp_merged() override final {
     return _object ;
     }

     std::shared_ptr<T>          _object {nullptr};
  };
}
