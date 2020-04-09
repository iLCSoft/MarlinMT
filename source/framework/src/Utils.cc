
// -- marlinmt headers
#include <marlinmt/Utils.h>

namespace marlinmt {
  
  namespace details {
  
    // specializations for type_info
    const char* type_info<std::nullptr_t>::type = "null" ;
    const char* type_info<bool>::type = "bool" ;
    const char* type_info<short>::type = "short" ;
    const char* type_info<int>::type = "int" ;
    const char* type_info<unsigned int>::type = "unsigned int" ;
    const char* type_info<float>::type = "float" ;
    const char* type_info<double>::type = "double" ;
    const char* type_info<char>::type = "char" ;
    const char* type_info<std::string>::type = "string" ;
    const char* type_info<std::vector<short>>::type = "vector<short>" ;
    const char* type_info<std::vector<int>>::type = "vector<int>" ;
    const char* type_info<std::vector<unsigned int>>::type = "vector<unsigned int>" ;
    const char* type_info<std::vector<float>>::type = "vector<float>" ;
    const char* type_info<std::vector<double>>::type = "vector<double>" ;
    const char* type_info<std::vector<char>>::type = "vector<char>" ;
    const char* type_info<std::vector<std::string>>::type = "vector<string>" ;
    const char* type_info<std::vector<bool>>::type = "vector<bool>" ;
    
  }
  
}