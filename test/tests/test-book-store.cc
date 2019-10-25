
#include <UnitTesting.h>
#include <typeinfo>

#include "marlin/book/Hist.h"
#include "marlin/book/Handle.h"
#include "marlin/book/BookStore.h"
#include "ROOT/RHistData.hxx" 
#include "ROOT/RHist.hxx"

#include <memory>

template<typename T>
void MergeaHist(const std::shared_ptr<T>& dst, const std::shared_ptr<T>& src) {
	ROOT::Experimental::Add(*dst, *src);
}

using namespace marlin::book;
using namespace ROOT::Experimental;

int main(int, char**) {
	marlin::test::UnitTest test(" MemFillerTest ");
	BookStore store{};

	{

		EntrySingle entry = store.book<RH<float, 1>, RAxisConfig>("test", "path", {"a", 3, 1.0, 2.0}) ;	
		auto hnd = entry.handle();
		hnd.fill({0}, 1);
		auto hist = hnd.get();
		test.test("Single Hist Filling", hist.GetEntries() == 1);

	}{

		EntryMultiCopy entry = store.bookMultiCopy<RH<int, 1>, RAxisConfig>(2, "test", "path", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle(0);
		hnd.fill({0}, 1);

		auto hnd2 = entry.handle(1);
		hnd2.fill({0}, 1);

		auto hist = hnd.get();
		test.test("MultiCopd Hist Filling", hist.GetBinContent({0})== 2);

	}

	return 0;
}
