#include <chrono>
#include <random>
#include <array>
#include <utility>
#include <chrono>

#include "ROOT/RHist.hxx"

#include <UnitTesting.h>

#include "marlin/book/Handle.h"
#include "marlin/book/BookStore.h"
#include "marlin/book/Hist.h"

constexpr unsigned int NumThreads = 3;

constexpr unsigned int nWrites = 1000000;
constexpr std::pair<float, float> range(0, 1000);

int main(int /*argc*/, char * /*argv*/[])
{

  marlin::test::UnitTest test(" Performance Test Filling: ");

  marlin::book::BookStore store;
  marlin::book::Handle entry = store.book("/", "hist", marlin::book::EntryData<marlin::book::types::H1F>("title", {"a", 250, range.first, range.second}).single());
  marlin::book::Handle hist = entry.handle(0);
  ROOT::Experimental::RH1F rhist("title", {"a", 250, range.first, range.second});

  std::array<float, nWrites> numbers;
  {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(range.first, range.second);
    for(float& v : numbers) {
      v = static_cast<float>(dist(e2));
    }
  }
  std::chrono::duration<double> bookStoreFill{};
  std::chrono::duration<double> rootFill{};
  
  std::size_t count1 = 0, count2 = 0;

  for(unsigned int i = 0; i < 1000; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    for(float v : numbers) {
      hist.fill({v}, 1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    bookStoreFill += end - start;


    start = std::chrono::high_resolution_clock::now();
    for(float v : numbers) {
      rhist.Fill({v}, 1);
    }
    end = std::chrono::high_resolution_clock::now();
    rootFill += end - start;

    count1 += hist.merged().get().GetEntries();
    count2 += rhist.GetEntries();
  }
  std::cout << "hits: " << count1 << " == " << count2 << '\n';
  std::cout << "             \tStore\t\tRoot\n"
            << "Time to fill:\t"<<bookStoreFill.count() << "\t" << rootFill.count() << "\n";

  double moreTime = (bookStoreFill.count() / rootFill.count()) - 1.0;
  test.test(
      std::string("performance lose from more then 5%! ")
        + std::to_string(moreTime),
        moreTime < 0.05
      );  

  return 0;
}
