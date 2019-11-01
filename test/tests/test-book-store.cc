
#include <UnitTesting.h>
#include <typeinfo>

#include "marlin/book/Hist.h"
#include "marlin/book/Handle.h"
#include "marlin/book/BookStore.h"
#include "marlin/book/Condition.h"
#include "marlin/book/Selection.h"
#include "ROOT/RHistData.hxx" 
#include "ROOT/RHist.hxx"
#include "marlin/book/ROOTAdapter.h"

#include <memory>
#include <string>

template<typename T>
void MergeaHist(const std::shared_ptr<T>& dst, const std::shared_ptr<T>& src) {
	ROOT::Experimental::Add(*dst, *src);
}

std::string mergedUnicStr() {
	static std::size_t num = 0;
	return std::to_string(++num);
}

using namespace marlin::book;
using namespace marlin::book::types;
using namespace ROOT::Experimental;




int main(int, char**) {
	marlin::test::UnitTest test(" MemFillerTest ");
	BookStore store{};
	


	{

		// EntrySingle entry = store.book<RH1F, RAxisConfig>("path", "name", {"a", 3, 1.0, 2.0}) ;	
		// EntrySingle entry = BookHelper<RH1F>(store, Flags::Book::Single)({"a", 3, 1.0, 2.0});
		EntrySingle entry = store.book<RH1F>("path", "name").single()({"a", 3, 1.0, 2.0});
		// EntrySingle entry = store.bookH1<RH1F>("path", "name", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle();
		hnd.fill({0}, 1);
		std::vector<typename decltype(hnd)::CoordArray_t> xs;
		std::vector<typename decltype(hnd)::Weight_t> ws;
		for(int i = 0; i < 10; ++i) {
			xs.push_back({1});
			ws.push_back(1);
		}
		hnd.fillN(xs, ws);

		auto hist = hnd.merged();
		test.test("Single Hist Filling", hist.GetEntries() == 11);

	}{

		// EntryMultiCopy entry = store.bookMultiCopy<RH1I, RAxisConfig>(2, "path2", "name", {"a", 3, 1.0, 2.0});
		EntryMultiCopy entry = store.book<RH1I>("path2", "name").multiCopy(2)({"a", 3, 1.0, 2.0});
		auto hnd = entry.handle(0);
		hnd.fill({0}, 1);

		auto hnd2 = entry.handle(1);
		hnd2.fill({0}, 1);

		auto hist = hnd.merged();
		test.test("MultiCopd Hist Filling", hist.GetBinContent({0})== 2);

	}{

		auto selection = store.find(ConditionBuilder().setName("name"));
		
		auto selection1 = store.find(ConditionBuilder().setType<RH1I>());

		auto selection2 = store.find(ConditionBuilder().setPath("path2"));

		auto selection3 = store.find(ConditionBuilder().setPath(std::regex("path(|2)")));
		
		test.test("Basic find function BookStore",
				selection.size() == 2
				&& selection1.size() == 1
				&& selection2.size() == 1
				&& selection3.size() == 2
				&& selection1.begin()->key().hash == selection2.begin()->key().hash);

		auto subSelection = selection.find(ConditionBuilder().setPath("path"), Selection::ComposeStrategie::AND);
		auto subSelection1 = store.find(subSelection.condition());

		test.test("Subselection composing AND", 
			subSelection.size() == 1
			&& subSelection1.size() == 1
			&& subSelection.begin()->key().hash == subSelection1.begin()->key().hash
		);

	} {
	
		EntryMultiShared entry = store.bookMultiShared<RH1I, RAxisConfig>("path3", "name", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle();
		hnd.fill({0}, 1);

		auto hnd2 = entry.handle();
		hnd2.fill({0}, 1);

		auto hist = hnd.merged();
		test.test("MultiShared Hist Filling", hist.GetBinContent({0}) == 2);

	}{
		
		std::string path = mergedUnicStr();
		for(int i = 0; i < 10; ++i) {
			store.book<RH1I, RAxisConfig>(path, mergedUnicStr(), {"a", 2, 0.0, 2.0});
		}
		
		Selection sel = store.find(ConditionBuilder().setPath(path));
		std::size_t n = sel.size();

		{
			Condition all = ConditionBuilder();
			Selection selC[] = {
				sel.find(all),
				sel.find(all),
				sel.find(all),
				sel.find(all)
			};

			auto itr = selC[2].begin() + 5;
			for(int i = 5; i < 10; ++i, ++itr) {
				selC[0].remove(i);
				selC[2].remove(itr);
			}
			selC[1].remove(5, 5);
			selC[3].remove(selC[3].begin() + 5, selC[3].end());

			bool equal = true;
			Selection::iterator aItr[] = {
				selC[0].begin(),
				selC[1].begin(),
				selC[2].begin(),
				selC[3].begin()};
			for(;equal && aItr[0] != selC[0].end();) {
				for(int i = 0; i < 4; ++aItr[i++]) {
					if(
						i < 3 &&
						aItr[i]->key().hash != aItr[i + 1]->key().hash) {
						equal = false;
						break;
					}
				}
			}
			test.test("Remove Elements from selection",
				selC[0].size() == selC[1].size()
				&& selC[1].size() == selC[2].size()
				&& selC[2].size() == selC[3].size()
				&& equal);
		} 	
	}{

			
		Selection sel = store.find(ConditionBuilder().setName("name"));
		Selection rem = store.find(sel.condition().Not());

		store.remove(rem);

		Selection sel2 = store.find(sel.condition());
		Selection rem2 = store.find(rem.condition());

		bool equal = true;
		auto itr2 = sel2.begin();
		for(auto itr = sel.begin(); itr != sel.end(); ++itr, ++itr2) {
			if(itr->key().hash != itr2->key().hash) {
				equal = false;
				break;
			}
		}
		if(sel2.size() != sel.size()) equal = false;

		store.clear();
		Selection selAll = store.find(ConditionBuilder());

		test.test("Remove Elements from store", 
					equal
			&& 	rem2.size() == 0 
			&& selAll.size() == 0);
	
	} 		



	return 0;
}
