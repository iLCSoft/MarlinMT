
// -- marlinmt headers
#include <marlinmt/BookStoreManager.h>
#include <marlinmt/Application.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/Processor.h>

// -- MarlinMTBook headers
#include <marlinmt/book/StoreWriter.h>  

// -- std headers
#include <string>

// -- unix specific includes
#include <sys/types.h>
#include <unistd.h>

namespace marlinmt {

#define INSTANCIATIONS_HIST(type) \
  template book::Handle<book::Entry<type>> BookStoreManager::getObject<type>(\
    const book::EntryKey&) const ;\
                            \
  template book::Handle<book::Entry<type>> BookStoreManager::bookHist<type>(\
      const std::filesystem::path&,\
      const std::string_view&,\
      const std::string_view&,\
      const std::array<\
        const AxisConfig<typename type::Precision_t>*,\
        type::Dimension>&,\
      const BookFlag_t&) 

  INSTANCIATIONS_HIST(Hist1F);
  INSTANCIATIONS_HIST(Hist1D);
  INSTANCIATIONS_HIST(Hist1I);
  INSTANCIATIONS_HIST(Hist2F);
  INSTANCIATIONS_HIST(Hist2D);
  INSTANCIATIONS_HIST(Hist2I);
  INSTANCIATIONS_HIST(Hist3F);
  INSTANCIATIONS_HIST(Hist3D);
  INSTANCIATIONS_HIST(Hist3I);

  //--------------------------------------------------------------------------
  
  BookStoreManager::BookStoreManager() :
    Component("BookStoreManager") {
    setName("BookStore") ;
  }
  
  //--------------------------------------------------------------------------
  
  void BookStoreManager::writeToDisk() const {
    book::StoreWriter writer ( _outputFile.get() ) ;
    _bookStore.storeList( writer, _entriesToWrite.begin(), _entriesToWrite.end());
  }

  //--------------------------------------------------------------------------
  
  void BookStoreManager::initialize() {
    auto &config = application().configuration() ;
    if( config.hasSection("bookstore") ) {
      const auto &section = config.section("bookstore") ;
      setParameters( section ) ;
    }
    // possible options for flags
    static const std::map<std::string, BookFlag_t> flags {
      {"share", BookFlags::MultiShared},
      {"copy", BookFlags::MultiCopy},
      {"default", BookFlags::MultiShared},
    } ;
    BookFlag_t memoryLayout( 0 ) ;
    std::string memLayoutStr = _defaultMemLayout.get() ;
    details::to_lower( memLayoutStr ) ;
    // if one thread only to layout is also the same: single
    if( 1 == application().cmdLineParseResult()._nthreads ) {
      memoryLayout = BookFlags::Single ;
    }
    else {
      auto iter = flags.find( memLayoutStr ) ;
      if( flags.end() == iter ) {
        MARLINMT_THROW( "Unknown memory layout flag '" + _defaultMemLayout.get() + "'" ) ;
      }
      memoryLayout = iter->second ;
    }
    _defaultFlag = BookFlags::Store | memoryLayout ;
  }

  //--------------------------------------------------------------------------

  template<typename HistT>
  book::Handle<book::Entry<HistT>> BookStoreManager::bookHist (
    const std::filesystem::path &path,
    const std::string_view &name,
    const std::string_view &title,
    const std::array<
      const AxisConfig<typename HistT::Precision_t>*,
      HistT::Dimension> &axesconfig,
    const BookFlag_t &flag) {

    const BookFlag_t& usedFlag = flag == BookFlags::Default
      ? _defaultFlag
      : flag;

    using Entry_t = book::Handle<book::Entry<HistT>>;

    bool store = usedFlag.contains(book::Flags::Book::Store);

    BookFlag_t flagsToPass = usedFlag & book::Masks::Book::MemoryLayout ;
    auto nthreads = application().cmdLineParseResult()._nthreads ;
    if ( 1 == nthreads ) {
      flagsToPass = book::Flags::Book::Single;
    }

    try {
    Entry_t res = getObject<HistT>(getKey(path, name)) ;
    const book::EntryKey& key = res.key();
    if (   key.flags != flagsToPass
        || key.type !=  std::type_index(typeid(HistT))) {
      MARLINMT_THROW("try to book to the same spot again");
    } 
    return res;
    } catch (const BookStoreManager::ObjectNotFound& ) {}

    book::EntryData<HistT> data(title, axesconfig);

    book::Handle<book::Entry<HistT>> entry;
    
    if( flagsToPass.contains(book::Flags::Book::MultiCopy)) {
      entry =  _bookStore.book( path, name, data.multiCopy(nthreads) ) ;
    } 
    else if ( flagsToPass.contains(book::Flags::Book::MultiShared)) {
      entry =  _bookStore.book( path, name, data.multiShared(nthreads) ) ;
    } 
    else if ( flagsToPass.contains(book::Flags::Book::Single)) {
      if ( nthreads != 1) {
        _logger->log<ERROR>() << "Single Memory layout can't be used"
          " with concurrency! \n"
          "\tuse MarlinMT for workflows without concurrency";
        MARLINMT_THROW("single only supported without concurrency!!");
      }
      entry =  _bookStore.book( path, name, data.single() ) ;
    } 
    else {
      MARLINMT_THROW("Try to book without MemoryLayout Flag");
    }

    if (store) {
      addToWrite( entry.key() ) ;
    }

    return entry;
  }
  
  //--------------------------------------------------------------------------
  
  template<typename T>
  book::Handle<book::Entry<T>> BookStoreManager::getObject(
      const book::EntryKey &key) const {
      if (key.type == std::type_index(typeid(T))) {
        return _bookStore.entry<T>(key);
      }
      throw ObjectNotFound("try to access Object which wrong type!");
  }

  //--------------------------------------------------------------------------
  
  void BookStoreManager::addToWrite( const book::EntryKey& key) {
    _entriesToWrite.insert(key);
  }

  //--------------------------------------------------------------------------
  
  void BookStoreManager::removeFromWrite( const book::EntryKey& key) {
    _entriesToWrite.erase(key);
  }

  //--------------------------------------------------------------------------
  
  const book::EntryKey& BookStoreManager::getKey(
    const std::filesystem::path &path,
    const std::string_view &name) const
  {
    book::WeakEntry res = _bookStore.findFirst(
      book::ConditionBuilder()
        .setPath(path.string())
        .setName(name) ) ;
    if (!res.valid()) {
      throw ObjectNotFound("Object for key not found.");
    }
    return res.key();
  }

}
