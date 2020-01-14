#ifndef MARLIN_PROCESSORAPI_h
#define MARLIN_PROCESSORAPI_h 1

// -- std headers
#include <array>
#include <filesystem>
#include <optional>
#include <string>

// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Application.h>
#include <marlin/GeometryManager.h>
#include <marlin/MarlinConfig.h>

// -- MarlinBook includes
#include "marlin/book/BookStore.h"
#include "marlin/book/Flags.h"
#include "marlin/book/Handle.h"
#include "marlin/book/Types.h"

namespace marlin {

  class Processor ;

  /**
   *  @brief  ProcessorApi class
   *
   *  Provide a static API for processors to make high level calls.
   *  For example, to register your processor for random seeds:
   *  @code{cpp}
   *  // in your processor init() function
   *  ProcessorApi::registerForRandomSeeds( this ) ;
   *  @endcode
   */
  class InternalClass;

  class ProcessorApi {
  public:
    // only static API
    ProcessorApi() = delete ;
    ~ProcessorApi() = delete ;

    /**
     *  @brief group function for booking 
     */
    class Book {
      template<typename T>
      static std::optional<book::Handle<book::Manager<T>>>
      getObject( const std::filesystem::path& pathName ) ;

      friend InternalClass;
      // TODO: add friends which set this 
      static void registerStore(std::unique_ptr<book::BookStore>&& store);
    public:
      static constexpr book::Flag_t DefaultConfiguration{
          book::Flags::value(book::Flags::Book::MultiShared)
        | book::Flags::value(book::Flags::Book::Store)};

      /**
       *  @brief Register new Histogram.
       *  @param proc the processor instance
       *  @param path to registered histogram
       *  @param axes array of axis configuration for the Histogram 
       *  @throw TODO: ErrorType if an Object at path already exist with other type
       */
      template<typename HistT>
      static  book::Handle<book::Manager<HistT>>
      create(
        const Processor * const proc,
        const std::filesystem::path& pathName,
        const std::string_view& title,
        const std::array<
          book::types::AxisConfig<typename HistT::Precision_t>,
          HistT::Dimension>& axes,
        const book::Flag_t& flags = DefaultConfiguration ) ;

      /**
       *  @brief Get Handle to existing histogram entry.
       *  Histograms can registered in steering file or with registerHistogram() 
       *  @param proc the processor instance
       *  @param path to registered histogram
       *  @throw TODO: ErrorType if path don't belong to an Object.
       */
      template<typename HistT>
      [[nodiscard]]
      static book::Handle<book::Manager<HistT>> 
      getHistogram(const Processor * proc, const std::filesystem::path& path) ;


      /**
       *  @brief register Object to write it at end of lifetime. 
       *  The same effect can archived with passing the Store flag by registration.
       *  @param proc the processor instance
       *  @param path to registered histogram
       */
      void write(const Processor * proc, const std::filesystem::path& path) ;
      void dontWrite(const Processor * proc, const std::filesystem::path& path) ;

    private:
      static std::unique_ptr<book::BookStore> _store;
    };

    /**
     *  @brief  Register the processor to get random seeds
     *
     *  @param  proc the processor to register
     */
    static void registerForRandomSeeds( Processor *const proc ) ;

    /**
     *  @brief  Get a random seed from the event.
     *  Your processor must have been registered beforehand using registerForRandomSeeds()
     *
     *  @param  proc the processor instance
     *  @param  event the current event from which to get random seeds
     */
    static unsigned int getRandomSeed( const Processor *const proc, EventStore *event ) ;

    /**
     *  @brief  Set the processor return value
     *
     *  @param  proc the processor instance
     *  @param  event the event to which the condition is attached
     *  @param  value the processor return value
     */
    static void setReturnValue( const Processor *const proc, EventStore *event, bool value ) ;

    /**
     *  @brief  Set the named processor return value
     *
     *  @param  proc the processor instance
     *  @param  event the event to which the condition is attached
     *  @param  name the name of the return value
     *  @param  value the processor return value
     */
    static void setReturnValue( const Processor *const proc, EventStore *event, const std::string &name, bool value ) ;

    /**
     *  @brief  Whether the event is the first event to be processed.
     *  WARNING! Use this method with caution in multi-threading mode.
     *  The first event is the first that has been pushed in the scheduler
     *  queue, maybe not the first one received in Processor::processEvent().
     *  Don't rely on this method to initialize data on first event, but rather
     *  use the init() method for that purpose.
     *
     *  @param  event the event to test
     */
    static bool isFirstEvent( EventStore *event ) ;

    /**
     *  @brief  Get the geometry handle as a given type
     *
     *  @param  proc the processor accessing the geometry
     */
    template <typename HANDLER>
    static const HANDLER* geometry( const Processor *const proc ) ;

    /**
     *  @brief  Notify the application to skip the current event processing
     *  and go directly to the next event by skipping next processors in the sequence
     *
     *  @param  proc the processor instance initiating the call
     */
    static void skipCurrentEvent( const Processor *const proc ) ;

    /**
     *  @brief  Abort program execution properly
     *
     *  @param  proc the processor initiating the abort call
     *  @param  reason the reason why the processor aborts the program
     */
    static void abort( const Processor *const proc, const std::string &reason ) ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename HANDLER>
  inline const HANDLER* ProcessorApi::geometry( const Processor *const proc ) {
    return proc->app().geometryManager().geometry<HANDLER>() ;
  }

  //--------------------------------------------------------------------------

  template<typename HistT>
  book::Handle<book::Manager<HistT>>
  ProcessorApi::Book::create(
    const Processor * const proc,
    const std::filesystem::path& pathName,
    const std::string_view& title,
    const std::array<
      book::types::AxisConfig<typename HistT::Precision_t>,
      HistT::Dimension>& axes,
    const book::Flag_t& flags ) {
    using namespace book;
    std::optional<Handle<Manager<HistT>>> res = getObject<HistT>(pathName);
    // TODO: decision: need more test? 
    if ( res ) {
      return std::move(res.value());
    }
    EntryData<HistT> data(title, axes);
    std::filesystem::path path 
      = (std::filesystem::path(proc->name()) / pathName).remove_filename() ;
    if ( flags.contains(book::Flags::Book::MultiCopy) ) {
      return _store->book(
        path,
        pathName.filename().string(),
        data.multiCopy(1 /* FIXME: need a number */));
    } else if ( flags.contains(book::Flags::Book::MultiShared)){
      return _store->book(
        path,
        pathName.filename().string(),
        data.multiShared()); 
    } 
    return _store->book( 
        path,
        pathName.filename().string(),
        data.single());
  } 

  //--------------------------------------------------------------------------

  template<typename HistT>
  book::Handle<book::Manager<HistT>> 
  ProcessorApi::Book::getHistogram(const Processor * proc, const std::filesystem::path& pathName) 
  {
    using namespace book;
    std::optional<Handle<Manager<HistT>>> res = getObject<HistT>(pathName);
    if ( res ) {
      return res.value();
    }
    throw std::exception();
  }

  //--------------------------------------------------------------------------

  template<typename T>
  std::optional<book::Handle<book::Manager<T>>>
  ProcessorApi::Book::getObject( const std::filesystem::path& pathName ) {
    using namespace book;
    using namespace book;
    if(!_store) { throw "no store :O"; }
    Selection res = _store->find( 
        ConditionBuilder()
          .setType(typeid(T))
          .setPath(std::filesystem::path(pathName).remove_filename().string())
          .setName(pathName.filename().c_str()));
    if ( res.size() != 1 ) {
      return std::nullopt;
    }
    return std::optional(res.get(0).handle<T>());

  }

  class InternalClass {
  public:
    InternalClass() {
      ProcessorApi::Book::registerStore(
        std::make_unique<book::BookStore>(false)
          );
    }
  };

}

#endif
