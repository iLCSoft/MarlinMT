#include <marlin/ProcessorApi.h>

// -- std headers
#include <utility>

// -- marlin headers
#include <marlin/Exceptions.h>
#include <marlin/EventExtensions.h>



namespace marlin {

  
  /**
   *  @brief construct absolute path from relative path and processor. 
   *  @param proc Processor who owned the path
   *  @param path absolute path in processor
   */
  std::filesystem::path constructPath(
    const Processor *proc,
    const std::filesystem::path &path ) {
    return (std::filesystem::path("/")
      += proc->name()) += path;
  }

  //--------------------------------------------------------------------------

  template<typename HistT>
  book::Handle<book::Entry<HistT>> getObject(
    const BookStoreManager &storeManager,
    const std::filesystem::path &path,
    const std::string_view &name) {

    return storeManager.getObject<HistT>(
        storeManager.getKey(path, name));
  }

  //--------------------------------------------------------------------------
  
  H1FEntry ProcessorApi::Book::bookHist1F (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfig,
    const BookFlag_t &flags )  {
    return proc->app().bookStoreManager().bookHist<Hist1F>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfig},
      flags);
  } 

  //--------------------------------------------------------------------------

  H1FEntry ProcessorApi::Book::getHist1F (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist1F>( 
        proc->app().bookStoreManager(), 
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------

  H2FEntry ProcessorApi::Book::bookHist2F (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfigX,
    const AxisConfigD &axisconfigY,
    const BookFlag_t &flags) {
    return proc->app().bookStoreManager().bookHist<Hist2F>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfigX, &axisconfigY},
      flags);
  } 

  //--------------------------------------------------------------------------

  H2FEntry ProcessorApi::Book::getHist2F (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist2F>(
        proc->app().bookStoreManager(),
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------
  
  H3FEntry ProcessorApi::Book::bookHist3F (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfigX,
    const AxisConfigD &axisconfigY,
    const AxisConfigD &axisconfigZ,
    const BookFlag_t &flags )  {
    return proc->app().bookStoreManager().bookHist<Hist3F>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfigX, &axisconfigY, &axisconfigZ},
      flags);
  } 

  //--------------------------------------------------------------------------

  H3FEntry ProcessorApi::Book::getHist3F (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist3F>( 
        proc->app().bookStoreManager(), 
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------
  
  H1DEntry ProcessorApi::Book::bookHist1D (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfig,
    const BookFlag_t &flags )  {
    return proc->app().bookStoreManager().bookHist<Hist1D>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfig},
      flags);
  } 

  //--------------------------------------------------------------------------

  H1DEntry ProcessorApi::Book::getHist1D (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist1D>( 
        proc->app().bookStoreManager(), 
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------

  H2DEntry ProcessorApi::Book::bookHist2D (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfigX,
    const AxisConfigD &axisconfigY,
    const BookFlag_t &flags) {
    return proc->app().bookStoreManager().bookHist<Hist2D>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfigX, &axisconfigY},
      flags);
  } 

  //--------------------------------------------------------------------------

  H2DEntry ProcessorApi::Book::getHist2D (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist2D>(
        proc->app().bookStoreManager(),
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------
  
  H3DEntry ProcessorApi::Book::bookHist3D (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfigX,
    const AxisConfigD &axisconfigY,
    const AxisConfigD &axisconfigZ,
    const BookFlag_t &flags )  {
    return proc->app().bookStoreManager().bookHist<Hist3D>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfigX, &axisconfigY, &axisconfigZ},
      flags);
  } 

  //--------------------------------------------------------------------------

  H3DEntry ProcessorApi::Book::getHist3D (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist3D>( 
        proc->app().bookStoreManager(), 
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------
  
  H1IEntry ProcessorApi::Book::bookHist1I (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfig,
    const BookFlag_t &flags )  {
    return proc->app().bookStoreManager().bookHist<Hist1I>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfig},
      flags);
  } 

  //--------------------------------------------------------------------------

  H1IEntry ProcessorApi::Book::getHist1I (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist1I>( 
        proc->app().bookStoreManager(), 
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------

  H2IEntry ProcessorApi::Book::bookHist2I (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfigX,
    const AxisConfigD &axisconfigY,
    const BookFlag_t &flags) {
    return proc->app().bookStoreManager().bookHist<Hist2I>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfigX, &axisconfigY},
      flags);
  } 

  //--------------------------------------------------------------------------

  H2IEntry ProcessorApi::Book::getHist2I (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist2I>(
        proc->app().bookStoreManager(),
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------
  
  H3IEntry ProcessorApi::Book::bookHist3I (
    Processor *proc, 
    const std::filesystem::path &path, 
    const std::string_view &name,
    const std::string_view &title,
    const AxisConfigD &axisconfigX,
    const AxisConfigD &axisconfigY,
    const AxisConfigD &axisconfigZ,
    const BookFlag_t &flags )  {
    return proc->app().bookStoreManager().bookHist<Hist3I>(
      constructPath(proc, path),
      name,
      title,
      {&axisconfigX, &axisconfigY, &axisconfigZ},
      flags);
  } 

  //--------------------------------------------------------------------------

  H3IEntry ProcessorApi::Book::getHist3I (
    const Processor *proc,
    const std::filesystem::path &path,
    const std::string_view &name ) {
    return getObject<Hist3I>( 
        proc->app().bookStoreManager(), 
        constructPath(proc, path), name);
  }

  //--------------------------------------------------------------------------
  
  void ProcessorApi::Book::write( 
      Processor *proc,
      const book::EntryKey &key) 
  {
    proc->app().bookStoreManager().addToWrite(key);
  }

  //--------------------------------------------------------------------------
  
  void ProcessorApi::Book::dontWrite(
      Processor *proc,
      const book::EntryKey &key)
  {
    proc->app().bookStoreManager().removeFromWrite(key);
  }

  //--------------------------------------------------------------------------
  
  void ProcessorApi::registerForRandomSeeds( Processor *const proc ) {
    proc->app().randomSeedManager().addEntry( proc ) ;
  }

  //--------------------------------------------------------------------------

  unsigned int ProcessorApi::getRandomSeed( const Processor *const proc, EventStore *event ) {
    auto randomSeeds = event->extensions().get<extensions::RandomSeed, RandomSeedExtension>();
    if( nullptr == randomSeeds ) {
      MARLIN_THROW( "No random seed extension in event" ) ;
    }
    return randomSeeds->randomSeed( proc ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::setReturnValue( const Processor *const proc, EventStore *event, bool value ) {
    auto procConds = event->extensions().get<extensions::ProcessorConditions, ProcessorConditionsExtension>() ;
    if( nullptr == procConds ) {
      MARLIN_THROW( "No processor conditions extension in event" ) ;
    }
    return procConds->set( proc, value ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::setReturnValue( const Processor *const proc, EventStore *event, const std::string &name, bool value ) {
    auto procConds = event->extensions().get<extensions::ProcessorConditions, ProcessorConditionsExtension>() ;
    if( nullptr == procConds ) {
      MARLIN_THROW( "No processor conditions extension in event" ) ;
    }
    return procConds->set( proc, name, value ) ;
  }

  //--------------------------------------------------------------------------

  bool ProcessorApi::isFirstEvent( EventStore *event ) {
    return *event->extensions().get<extensions::IsFirstEvent, bool>() ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::skipCurrentEvent( const Processor *const proc ) {
    proc->log<WARNING>() << "Skipping current event !" << std::endl ;
    MARLIN_SKIP_EVENT( proc ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::abort( const Processor *const proc, const std::string &reason ) {
    proc->log<WARNING>() << "Stopping application: " << reason << std::endl ;
    MARLIN_STOP_PROCESSING( proc ) ;
  }

}
