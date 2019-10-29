
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

std::string getUnicStr() {
	static std::size_t num = 0;
	return std::to_string(++num);
}

using namespace marlin::book;
using namespace ROOT::Experimental;

int main(int, char**) {
	marlin::test::UnitTest test(" MemFillerTest ");
	BookStore store{};

	{

		// EntrySingle entry = store.book<RH<float, 1>, RAxisConfig>("test", "path", {"a", 3, 1.0, 2.0}) ;	
		EntrySingle entry = store.bookH1<RH1F>("path", "name", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle();
		hnd.fill({0}, 1);
		auto hist = hnd.get();
		test.test("Single Hist Filling", hist.GetEntries() == 1);

	}{

		EntryMultiCopy entry = store.bookMultiCopy<RH1I, RAxisConfig>(2, "path2", "name", {"a", 3, 1.0, 2.0});
		auto hnd = entry.handle(0);
		hnd.fill({0}, 1);

		auto hnd2 = entry.handle(1);
		hnd2.fill({0}, 1);

		auto hist = hnd.get();
		test.test("MultiCopd Hist Filling", hist.GetBinContent({0})== 2);

	}{

		auto selection = store.find(ConditionBuilder().setName("name"));
		
		auto selection1 = store.find(ConditionBuilder().setType<RH1I>());

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

	} {
		
		std::string path = getUnicStr();
		for(int i = 0; i < 10; ++i) {
			store.book<RH1I, RAxisConfig>(path, getUnicStr(), {"a", 2, 0.0, 2.0});
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
		
		//test.test("Remove Elements from store",
		//	);
		

	}

	return 0;
}
