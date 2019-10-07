#include "marlin/book.h"
#include "marlin/hist.h"
#include <typeinfo>
#include <utility>
#include "RHist.hxx"


class RH1D;

template<>
HistHnd<RH1D>
BookStore::Book<RH1D>(
	const std::string& path,
	const Flags& flags
) {	
	if(_state != State::Init)
		MARLIN_THROW_T(BookException, "booking is only in init possible.");

	std::shared_ptr<RH1D> ptr;

	auto itr =  _pathToHist.find(path);
	if (itr == _pathToHist.end()) {
		itr = AddEntrie(path, flags, typeid(RH1D).hash_code());
	}

	if(itr->second.typeHash != typeid(RH1D).hash_code()) {
		MARLIN_THROW_T(BookException, "Allrdeady booked with other Type.");
	}

	Count_t nr = itr->second.nrHistInstances ++;

	if(static_cast<bool>(
		itr->second.flags & Flags::MultiInstances)) {
		if(nr >= _maxInstances) {
			MARLIN_THROW_T(BookException, "created too many instances");
		}

		ptr = std::make_shared<RH1D>();
		_hists[itr->second.begin + nr] = ptr;

	} else {

		if(nr == 1) {
			ptr = std::make_shared<RH1D>();
			_hists[itr->second.begin] = ptr;
		} else {
			ptr = std::static_pointer_cast<RH1D>
				(_hists[itr->second.begin]);
			itr->second.nrHistInstances -= 1;
		}
	}

	return HistHnd<RH1D>(ptr, flags, itr->second, *this);
}

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
