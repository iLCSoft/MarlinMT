
#include <memory>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <UnitTesting.h>
#include "marlin/book/bookStore.h"
#include "marlin/book/hist.h"
#include "marlin/book/Flags.h"
#include "ROOT/RHist.hxx"


std::mutex cv_m;
std::condition_variable cv;
void worker(int i) {
	std::cout << "Wait ... id: " << i << '\n';
	std::lock_guard<std::mutex> look(cv_m);
	std::cout << "fin .... id: " << i << '\n';
}

int main(int, char**) {
	marlin::test::UnitTest test (" BookStore ");

	ROOT::Experimental::RH1D hist1d{{"test", 10, 0, 1}};
	std::cout << "histogramm\n";
	marlin::BookStore store{};
	std::cout << "store\n";
	marlin::BookStore::histCtor_t<1, float> ctor_p{std::tuple(std::string("x"), 2, 1.0, 0.0)};
	std::cout << "ctor_peai\n";
	auto hnd = store.bookHist<1, float>("name", "path", ctor_p, marlin::Flag_t{});
	std::cout << "hnd\n";
	hist1d.Fill({1}, 1);
	std::cout << "fill dircet\n";
	hnd.Fill({1}, 1);
	std::cout << "fill\n";
	test.test("success", true);
	return 0;
}
