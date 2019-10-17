
#include <memory>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <UnitTesting.h>
#include "marlin/book/BookStore.h"
#include "marlin/book/Flags.h"
#include "marlin/book/hist.h"
#include "ROOT/RHist.hxx"

using namespace ROOT::Experimental;

int main(int, char**) {
	marlin::test::UnitTest test (" BookStore ");

	marlin::book::BookStore store{};
	auto hnd1 = store.book<RH1D, RAxisConfig>("name", "path", 0, 2, marlin::book::Flag_t{}, {"x", 2, 1.0, 0.0});
	auto hnd2 = store.book<RH1D, RAxisConfig>("name", "path", 1, 2, marlin::book::Flag_t{}, {"x", 2, 1.0, 0.0});
	hnd1->fill({1}, 1);
	hnd2->fill({1}, 1);
	hnd1->fill({0}, 1);
	test.test(
		"Concurrent Test",
		hnd1.get().GetBinContent({1}) == 2
		&& hnd2.get().GetBinContent({0}) == 1);
	
	auto hnd3 = store.book<RH1D, RAxisConfig>("name2", "path", 0, 2, marlin::book::Flags::Book::MultiInstance, {"x", 2, 1.0, 0.0});
	auto hnd4 = store.book<RH1D, RAxisConfig>("name2", "path", 1, 2, marlin::book::Flags::Book::MultiInstance, {"x", 2, 1.0, 0.0});
	hnd3->fill({1}, 1);
	hnd3->fill({1}, 1);
	hnd4->fill({0}, 1);
	test.test(
		"Parallel Test", 
		hnd3.get().GetBinContent({0}) == 1
		&& hnd4.get().GetBinContent({1}) == 2); 
	return 0;
}
