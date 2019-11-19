#include "marlin/book/Entry.h"
#include "marlin/book/ROOTAdapter.h"

#define MARLIN_FN( T )\
	template<>void merge(const std::shared_ptr<T>&dst, const std::shared_ptr<T>&src)\
	{ types::addHists(dst, src); }

namespace marlin {
	namespace book {
		MARLIN_FN(types::RH1F);
		MARLIN_FN(types::RH2F);
		MARLIN_FN(types::RH3F);
		MARLIN_FN(types::RH1D);
		MARLIN_FN(types::RH2D);
		MARLIN_FN(types::RH3D);
		MARLIN_FN(types::RH1I);
		MARLIN_FN(types::RH2I);
		MARLIN_FN(types::RH3I);
	}
}
