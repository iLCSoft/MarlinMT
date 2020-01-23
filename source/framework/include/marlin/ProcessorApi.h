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
#include <marlin/BookStoreManager.h>
#include <marlin/MarlinConfig.h>


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
    public:
      static constexpr book::Flag_t DefaultConfiguration{
          book::Flags::value(book::Flags::Book::MultiShared)
        | book::Flags::value(book::Flags::Book::Store)};

      // book histogram float

      /**
       *  @brief  Book  a histogram 1D, float type
       *  
       *  @param  proc       the processor booking the histogram
       *  @param  path       the histogram entry path
       *  @param  name       the histogram name
       *  @param  title      the histogram title
       *  @param  axisconfig the histogram X axis config
       *  @param  flags      the book flag policy
       */
      [[nodiscard]] static H1FEntry bookHist1F (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfig,
        const BookFlag &flags  = DefaultConfiguration) ; 

      /**
       *  @brief  Book  a histogram 2D, float type
       *  
       *  @param  proc        the processor booking the histogram
       *  @param  path        the histogram entry path
       *  @param  name        the histogram name
       *  @param  title       the histogram title
       *  @param  axisconfigX the histogram X axis configuration
       *  @param  axisconfigY the histogram Y axis configuration
       *  @param  flags       the book flag policy
       */
      [[nodiscard]] static H2FEntry bookHist2F (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfigX,
        const AxisConfigD &axisconfigY,
        const BookFlag &flags  = DefaultConfiguration) ; 

      /**
       *  @brief  Book  a histogram 3D, float type
       *  
       *  @param  proc        the processor booking the histogram
       *  @param  path        the histogram entry path
       *  @param  name        the histogram name
       *  @param  title       the histogram title
       *  @param  axisconfigX the histogram X axis configuration
       *  @param  axisconfigY the histogram Y axis configuration
       *  @param  axisconfigZ the histogram Z axis configuration
       *  @param  flags       the book flag policy
       */
      [[nodiscard]] static H3FEntry bookHist3F (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfigX,
        const AxisConfigD &axisconfigY,
        const AxisConfigD &axisconfigZ,
        const BookFlag &flags  = DefaultConfiguration) ; 

      // book histogram double

      /**
       *  @brief  Book  a histogram 1D, double type
       *  
       *  @param  proc       the processor booking the histogram
       *  @param  path       the histogram entry path
       *  @param  name       the histogram name
       *  @param  title      the histogram title
       *  @param  axisconfig the histogram X axis config
       *  @param  flags      the book flag policy
       */
      [[nodiscard]] static H1DEntry bookHist1D (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfig,
        const BookFlag &flags  = DefaultConfiguration) ; 

      /**
       *  @brief  Book  a histogram 2D, double type
       *  
       *  @param  proc        the processor booking the histogram
       *  @param  path        the histogram entry path
       *  @param  name        the histogram name
       *  @param  title       the histogram title
       *  @param  axisconfigX the histogram X axis configuration
       *  @param  axisconfigY the histogram Y axis configuration
       *  @param  flags       the book flag policy
       */
      [[nodiscard]] static H2DEntry bookHist2D (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfigX,
        const AxisConfigD &axisconfigY,
        const BookFlag &flags  = DefaultConfiguration) ; 

      /**
       *  @brief  Book  a histogram 3D, double type
       *  
       *  @param  proc        the processor booking the histogram
       *  @param  path        the histogram entry path
       *  @param  name        the histogram name
       *  @param  title       the histogram title
       *  @param  axisconfigX the histogram X axis configuration
       *  @param  axisconfigY the histogram Y axis configuration
       *  @param  axisconfigZ the histogram Z axis configuration
       *  @param  flags       the book flag policy
       */
      [[nodiscard]] static H3DEntry bookHist3D (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfigX,
        const AxisConfigD &axisconfigY,
        const AxisConfigD &axisconfigZ,
        const BookFlag &flags  = DefaultConfiguration) ; 

      // book histogram integer 
      
      /**
       *  @brief  Book  a histogram 1D, integer type
       *  
       *  @param  proc       the processor booking the histogram
       *  @param  path       the histogram entry path
       *  @param  name       the histogram name
       *  @param  title      the histogram title
       *  @param  axisconfig the histogram X axis config
       *  @param  flags      the book flag policy
       */
      [[nodiscard]] static H1IEntry bookHist1I (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfig,
        const BookFlag &flags  = DefaultConfiguration) ; 

      /**
       *  @brief  Book  a histogram 2D, integer type
       *  
       *  @param  proc        the processor booking the histogram
       *  @param  path        the histogram entry path
       *  @param  name        the histogram name
       *  @param  title       the histogram title
       *  @param  axisconfigX the histogram X axis configuration
       *  @param  axisconfigY the histogram Y axis configuration
       *  @param  flags       the book flag policy
       */
      [[nodiscard]] static H2IEntry bookHist2I (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfigX,
        const AxisConfigD &axisconfigY,
        const BookFlag &flags  = DefaultConfiguration) ; 

      /**
       *  @brief  Book  a histogram 3D, integer type
       *  
       *  @param  proc        the processor booking the histogram
       *  @param  path        the histogram entry path
       *  @param  name        the histogram name
       *  @param  title       the histogram title
       *  @param  axisconfigX the histogram X axis configuration
       *  @param  axisconfigY the histogram Y axis configuration
       *  @param  axisconfigZ the histogram Z axis configuration
       *  @param  flags       the book flag policy
       */
      [[nodiscard]] static H3IEntry bookHist3I (
        Processor *proc, 
        const std::filesystem::path &path, 
        const std::string_view &name,
        const std::string_view &title,
        const AxisConfigD &axisconfigX,
        const AxisConfigD &axisconfigY,
        const AxisConfigD &axisconfigZ,
        const BookFlag &flags  = DefaultConfiguration) ; 

      // get histogram float

      /**
       *  @brief Get handle for booked histogram 1D, float type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H1FEntry getHist1F (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      /**
       *  @brief Get handle for booked histogram 2D, float type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H2FEntry getHist2F (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      /**
       *  @brief Get handle for booked histogram 3D, float type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H3FEntry getHist3F (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      // get histogram double

      /**
       *  @brief Get handle for booked histogram 1D, double type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H1DEntry getHist1D (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      /**
       *  @brief Get handle for booked histogram 2D, double type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H2DEntry getHist2D (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      /**
       *  @brief Get handle for booked histogram 3D, double type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H3DEntry getHist3D (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      // get histogram integer

      /**
       *  @brief Get handle for booked histogram 1D, integer type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H1IEntry getHist1I (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      /**
       *  @brief Get handle for booked histogram 2D, integer type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H2IEntry getHist2I (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;

      /**
       *  @brief Get handle for booked histogram 3D, integer type.
       *
       *  @param proc the processor which booked the histogram
       *  @param path the histogram entry path
       *  @param name the histogram name
       */
      [[nodiscard]] static H3IEntry getHist3I (
        const Processor *proc,
        const std::filesystem::path &path,
        const std::string_view &name ) ;



      /**
       *  @brief register Object to write it at end of lifetime. 
       *  The same effect can archived with passing the Store flag by registration.
       *  @param proc the processor instance
       *  @param path to registered object 
       *  @param name of object 
       */
      void write(
          Processor * proc, 
          const std::filesystem::path &path,
          const std::string_view &name) ;

      /**
       *  @brief cancels writing of Object at end of lifetime.
       *  @param proc the processor instance
       *  @param path to register object
       *  @param name of object
       */
      void dontWrite(
          Processor * proc, 
          const std::filesystem::path &path,
          const std::string_view &name) ;

      /**
       *  @brief register Object to write at end of lifetime.
       *  The same effect can archived with passing the Store flag by registration
       *  @param key of Entry to Object which should be stored.
       */
      void write(
          Processor *proc,
          const book::EntryKey &key) ;

      /**
       *  @brief cancels writing of Object at end of lifetime.
       *  @param key of Entry to Object which should be stored.
       */
      void dontWrite(
          Processor *proc,
          const book::EntryKey &key) ;
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

  template <typename HANDLER>
  inline const HANDLER* ProcessorApi::geometry( const Processor *const proc ) {
    return proc->app().geometryManager().geometry<HANDLER>() ;
  }

}

#endif
