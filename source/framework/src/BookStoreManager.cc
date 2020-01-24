
// -- marlin headers
#include <marlin/BookStoreManager.h>
#include <marlin/Application.h>
#include <marlin/Exceptions.h>
#include <marlin/Processor.h>

#include <marlin/book/StoreWriter.h>  

#include <string>

namespace marlin {

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
    
  
  void BookStoreManager::writeToDisk() const {
    if (_storeFile == "") {
      if (_entriesToWrite.empty())   {
        _logger->log<MESSAGE>() << "No Output file set!\n";
      } else {
        _logger->log<WARNING>() << "No Output file set, but " 
          << _entriesToWrite.size() << " entries to write!\n";
      }
    } else {
      book::StoreWriter writer ( _storeFile ) ;
      _bookStore.storeList(
        writer,
        _entriesToWrite.begin(), 
        _entriesToWrite.end());
    }
  }

  //--------------------------------------------------------------------------
  

  void BookStoreManager::init( const Application *app ) {
    if( isInitialized() ) {
      MARLIN_THROW( "BookStoreManager::init: Already initialized !" ) ;
    }
    _application = app ;
    _logger = _application->createLogger( "BookStoreManager" ) ;

    std::shared_ptr<StringParameters> paras =  app->bookStoreParameters();
    if (!paras) {
      _logger->log<WARNING>() 
        << "no <store> node exist on top level!\n"
        << "\tNo Output file set!\n"
        << "\tUse Default flags for Booking!\n";
      _defaultFlag = BookFlags::MultiShared | BookFlags::Store ;  
    } else {
      _storeFile =
        paras->getValue<std::string>(ParameterNames::OutPutFile, "");
      if (_storeFile == "") {
        try { 
          _storeFile = std::filesystem::temp_directory_path();
        } catch (const std::filesystem::filesystem_error&) {
          _storeFile = "";
        }
        std::stringstream strs{};
        strs << "MarlinMT_" << std::this_thread::get_id() << ".root";
        _storeFile /= strs.str(); 
        _logger->log<WARNING>()
          << "no output file for store defined, will write in temporary file: "
          << _storeFile.string() << "\n";
      }

      BookFlag_t memoryLayout(0);
      BookFlag_t storeByDefault(0);
      std::string str = 
        paras->getValue<std::string>(
          ParameterNames::DefaultMemoryLayout, 
          "Default");
      if ( str == "Share" ) {
        memoryLayout = BookFlags::MultiShared ;
      } else if ( str == "Copy" ) {
        memoryLayout = BookFlags::MultiCopy ;
      } else if ( str == "Default") {
        memoryLayout = BookFlags::MultiShared ;
      } else {
        memoryLayout = BookFlags::MultiShared ;
        _logger->log<WARNING>() << "not recognized input option: " 
          << str << '\n';
      }

      str = 
        paras->getValue<std::string>(
          ParameterNames::StoreByDefault, 
          "DEFAULT") ;
      if ( str == "YES" ) {
        storeByDefault = BookFlags::Store ; 
      } else if ( str == "NO" ) {
        storeByDefault = BookFlag_t(0);
      } else if ( str == "DEFAULT" ){
        storeByDefault = BookFlags::Store ;
      } else {
        storeByDefault = BookFlags::Store ;
        _logger->log<WARNING>() << "not recognized option for: store::"
          << ParameterNames::StoreByDefault << " set to: '"
          << str << "' but supported are: YES,NO,DEFAULT";
      }
      _defaultFlag = storeByDefault | memoryLayout ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  bool BookStoreManager::isInitialized() const {
    return ( nullptr != _application ) ;
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

    BookFlag_t flagsToPass = usedFlag &
      (   book::Flags::Book::MultiCopy 
        | book::Flags::Book::MultiShared 
        | book::Flags::Book::Single) ;
    try {
    Entry_t res = getObject<HistT>(getKey(path, name)) ;
    const book::EntryKey& key = res.key();
    if (   key.flags != flagsToPass
        || key.type !=  std::type_index(typeid(HistT))) {
      MARLIN_THROW("try to book to the same spot again");
    } 
    return res;
    } catch (const BookStoreManager::ObjectNotFound& ) {}

    book::EntryData<HistT> data(title, axesconfig);

    book::Handle<book::Entry<HistT>> entry;

    if( usedFlag.contains(book::Flags::Book::MultiCopy)) {
      entry =  _bookStore.book( path, name, data.multiCopy(_application->getConcurrency()) ) ;
    } else if ( usedFlag.contains(book::Flags::Book::MultiShared)) {
      entry =  _bookStore.book( path, name, data.multiShared() ) ;
    } else if ( usedFlag.contains(book::Flags::Book::MultiShared)) {
      entry =  _bookStore.book( path, name, data.single() ) ;
    } else {
      MARLIN_THROW("Try to book without MemoryLayout Flag");
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
