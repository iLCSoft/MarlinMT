
#include <UnitTesting.h>
#include <typeinfo>

#include "marlin/book/MemLayout.h"
#include "marlin/book/FillBale.h"

using namespace ROOT::Experimental;
namespace marlin::book {

void MergeHist(std::shared_ptr<RH1F>& p1, std::shared_ptr<RH1F>& p2) {
	ROOT::Experimental::Add(*p1, *p2);
}

using SingleRH1F = SingleMemLayout<RH1F, RAxisConfig>;
using SharedRH1F = SharedMemLayout<RH1F, MergeHist, RAxisConfig>;

}
using namespace marlin::book;
int main(int, char**) {
	marlin::test::UnitTest test("book filler test");
		
	SingleRH1F memHist1({"x", 2, 0, 1});
	FillerMgr fillMgr(memHist1);
	auto filler = fillMgr.CreateFiller();
	filler->Fill({1}, 1);
	filler->Flush();
	test.test("filler works", memHist1.merged<RH1F>()->GetEntries() == 1);
	
	SharedRH1F memHist2(3, {"x", 2, 0, 1});
	FillerMgr fillMgr2(memHist2);
	auto filler2 = fillMgr2.CreateFiller(1);
	std::cout << "test\n";
	std::cout << typeid(filler2).name() << '\n';
	filler2->Fill({1}, 1);
	std::cout << "meg\n";
	test.test("shared aproach", memHist2.merged<RH1F>()->GetBinContent({1}) == 1);
}
