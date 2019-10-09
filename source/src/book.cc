#include "marlin/book.h"
#include "marlin/hist.h"
#include <typeinfo>
#include <utility>
#include "RHist.hxx"

typename BookStore::EntrieMap::iterator
BookStore::AddEntrie(
	const std::string& path,
	const Flags& flags,
	std::size_t typeHash) {

	Entrie& entrie = _pathToHist[path];	


	entrie.nrHistInstances = 0; 

	entrie.begin = _hists.size();
	_hists.resize(_hists.size() + 
		static_cast<bool>(flags & Flags::MultiInstances)
		? _maxInstances
		: 1);
	
	entrie.flags = flags;

	entrie.typeHash =typeHash;	

	return _pathToHist.find(path);
}
