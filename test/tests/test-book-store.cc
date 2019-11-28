
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
#include "marlin/Exceptions.h"

#include <memory>
#include <string>

std::string mergedUnicStr() {
  static std::size_t num = 0;
  return std::to_string(++num);
}

using namespace marlin::book;
using namespace marlin::book::types;


int main(int /*argc*/, char** /*argv*/) {
  marlin::test::UnitTest test(" Memory Filler Test ");
	constexpr std::size_t bins = 3;
	constexpr float min = -1.F;
	constexpr float max = 5.F;
	constexpr int nItrerations = 10;
	RAxisConfig axis("a", bins, min, max);
  BookStore store{};
	try{
	{
		Handle<Manager<RH1F>> entry = store.book("path", "name", EntryData<RH1F>(axis).single());
		Handle<RH1F> handle = entry.handle(1);
	} {

    // EntrySingle entry = store.book<RH1F, RAxisConfig>("path", "name", {"a", 3, 1.0, 2.0}) ; 
    Handle<Manager<RH1F>> entry = store.book("path", "name", EntryData<RH1F>(axis).single());

    Handle<RH1F> hnd = entry.handle(0);
    std::vector<typename decltype(hnd)::CoordArray_t> xs;
    std::vector<typename decltype(hnd)::Weight_t> ws;
    for(int i = 0; i < nItrerations; ++i) {
      xs.emplace_back(1);
      ws.emplace_back(1);
    }
    hnd.fillN(xs, ws);

    auto hist = hnd.merged();
    test.test("Single Hist Filling", hist.GetEntries() == nItrerations);

  }{
    Handle<Manager<RH1I>> entry = store.book("path2", "name", EntryData<RH1I>(axis).multiCopy(2));

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

    auto subSelection = selection.find(ConditionBuilder().setPath("path"), Selection::ComposeStrategy::AND);
    auto subSelection1 = store.find(subSelection.condition());

    test.test("Subselection composing AND", 
      subSelection.size() == 1
      && subSelection1.size() == 1
      && subSelection.begin()->key().hash == subSelection1.begin()->key().hash
    );

  } {
  
    Handle<Manager<RH1I>> entry = store.book("path3", "name", EntryData<RH1I>(axis).multiShared());
    auto hnd = entry.handle(1);
    hnd.fill({0}, 1);

    auto hnd2 = entry.handle(2);
    hnd2.fill({0}, 1);

    auto hist = hnd.merged();
    test.test("MultiShared Hist Filling", hist.GetBinContent({0}) == 2);

  }{
    
    std::string path = mergedUnicStr();
		std::string name;
		EntryData config = EntryData<RH1I>(axis);
    for(int i = 0; i < nItrerations; ++i) {
			name = mergedUnicStr(); 
      store.book(path, name, config.single());
    }
    
    Selection sel = store.find(ConditionBuilder().setPath(path));

    {
      Condition all = ConditionBuilder();
			std::array selC = {
        sel.find(all),
        sel.find(all),
        sel.find(all),
        sel.find(all)
      };

      auto itr = selC[2].begin() + nItrerations / 2;
      for(int i = nItrerations / 2; i < nItrerations; ++i, ++itr) {
        selC[0].remove(i);
        selC[2].remove(itr);
      }
      selC[1].remove(nItrerations / 2, nItrerations / 2);
      selC[3].remove(selC[3].begin() + nItrerations / 2, selC[3].end());

      bool equal = true;
			std::array aItr = {
        selC[0].begin(),
        selC[1].begin(),
        selC[2].begin(),
        selC[3].begin()};
      for(;equal && aItr[0] != selC[0].end();) {
        for(int i = 0; i < nItrerations / 2 - 1; ++aItr.at(i++)) {
          if(
            i < 3 &&
            aItr.at(i)->key().hash != aItr.at(i + 1)->key().hash) {
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
    Selection rem = store.find(!sel.condition());

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
    if(sel2.size() != sel.size()) { equal = false; }

    store.clear();
    Selection selAll = store.find(ConditionBuilder());

    test.test("Remove Elements from store", 
          equal
      &&  rem2.size() == 0 
      && selAll.size() == 0);
  
  } {
    std::size_t n = store.find(ConditionBuilder()).size();

    EntryData<RH1F> entry1(axis);
    EntryData<RH2F> entry2(axis, axis);
    EntryData<RH3F> entry3(axis, axis, axis);
    store.book("path", mergedUnicStr(), entry1.single());
    store.book("path", mergedUnicStr(), entry2.single());
    store.book("path", mergedUnicStr(), entry3.single());
    store.book("path", mergedUnicStr(), entry1.multiShared());
    store.book("path", mergedUnicStr(), entry1.multiCopy(3));

    std::size_t n2 = store.find(ConditionBuilder()).size();

    test.test("BookHelper usage", n + nItrerations / 2 == n2);
  } {
    Handle<Manager<RH1F>> e = store.book("path", "my Name", EntryData<RH1F>(axis).single());
    e.handle(1).fill({0}, 1);


    Selection sel = store.find(ConditionBuilder().setName("my Name"));
    Handle<RH1F> h = sel.begin()->bind<RH1F>().handle(1);
    h.fill({0}, 1);

    test.test("Get booked entry from BookStore",
      e.handle(1).merged().GetBinContent({0}) == 2
      && h.merged().GetBinContent({0}) == 2);   

  } {
    Handle e = store.book("path", mergedUnicStr(), EntryData<RH1F>("title", axis).single());
    e.handle(1).fill({0}, 1);
    test.test("Named Histograms", e.handle(1).merged().GetEntries() == 1);
  } {
		bool errorThrown = false;
		std::thread t1([&store, &errorThrown, &axis](){
					try{
						store.book("path", "name", EntryData<RH1F>(axis).single());
					} catch (const marlin::BookStoreException&) {
						errorThrown = true;
					}
				});
		t1.join();
		test.test("prevent booking from other threads", errorThrown);
	}
	} catch(const marlin::BookStoreException& excp){
		test.test(std::string("Not expected error") + excp.what(), false);
	}
  return 0;
}
