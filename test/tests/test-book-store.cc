
#include <UnitTesting.h>
#include <typeinfo>

#include "marlin/book/Hist.h"
#include "marlin/book/Handle.h"
#include "marlin/book/BookStore.h"
#include "marlin/book/Condition.h"
#include "marlin/book/Selection.h"
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

		// EntrySingle entry = store.book<RH<float, 1>, RAxisConfig>("test", "path", {"a", 3, 1.0, 2.0}) ;	
		EntrySingle entry = store.bookH1<float>("path", "name", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle();
		hnd.fill({0}, 1);
		auto hist = hnd.get();
		test.test("Single Hist Filling", hist.GetEntries() == 1);

	}{

		EntryMultiCopy entry = store.bookMultiCopy<RH<int, 1>, RAxisConfig>(2, "path2", "name", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle(0);
		hnd.fill({0}, 1);

		auto hnd2 = entry.handle(1);
		hnd2.fill({0}, 1);

		auto hist = hnd.get();
		test.test("MultiCopd Hist Filling", hist.GetBinContent({0})== 2);

	}{

		auto selection = store.find(ConditionBuilder().setName("name"));
		
		auto selection1 = store.find(ConditionBuilder().setType<RH<int, 1>>());

		auto selection2 = store.find(ConditionBuilder().setPath("path2"));
	
		test.test("Basic find function BookStore",
				selection.size() == 2
				&& selection1.size() == 1
				&& selection2.size() == 1
				&& selection1.begin()->key().hash == selection2.begin()->key().hash);

		auto subSelection = selection.find(ConditionBuilder().setPath("path"), Selection::ComposeStrategie::AND);
		auto subSelection1 = store.find(subSelection.condition());

		test.test("Subselection composing AND", 
			subSelection.size() == 1
			&& subSelection1.size() == 1
			&& subSelection.begin()->key().hash == subSelection1.begin()->key().hash
		);
	}

	return 0;
}
