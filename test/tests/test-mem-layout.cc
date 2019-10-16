#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <cmath>

#include <UnitTesting.h>
#include "marlin/book/MemLayout.h"
#include "ROOT/RHist.hxx"
#include "ROOT/RAxis.hxx"

using namespace ROOT::Experimental;
using namespace marlin::book;
struct Type1 {
	std::array<int, 5> bins;
	Type1(int a0, int a1, int a2, int a3, int a4)
		: bins{a0, a1, a2, a3, a4} {}
};

void MergeType1(const std::shared_ptr<Type1>& p1, const std::shared_ptr<Type1>& p2) {
	for(int i = 0; i < 5; ++i) {
		p1->bins[i] += p2->bins[i];
	}
}
template<typename ... Args_t>
class SharedType1 : public SharedMemLayout<Type1, MergeType1, Args_t ... > {
public:
	SharedType1 (std::size_t num_inst, Args_t ... args) : SharedMemLayout<Type1, MergeType1, Args_t ... >{num_inst, args ...}{}
};

void MergeHist(const std::shared_ptr<RH1D>& p1, const std::shared_ptr<RH1D>& p2) {
	ROOT::Experimental::Add(*p1, *p2);
}

class SharedRH1D : public SharedMemLayout<RH1D, MergeHist, RAxisConfig> {
public:
	SharedRH1D (std::size_t num_inst, RAxisConfig config) : SharedMemLayout<RH1D, MergeHist, RAxisConfig>{num_inst, config} {}
};

int main (int, char**) {
	std::srand(std::time(nullptr));
	marlin::test::UnitTest test(" MemLayout Test ");
	SharedType1 sMem(3, 1, 0, 0, 0, 0);
	auto ptr1 = sMem.at<Type1>(0);
	auto ptr2 = sMem.at<Type1>(1);
	ptr1->bins[0] += 3;
	ptr2->bins[0] += 2;
	test.test("bin content test ", sMem.merged<Type1>()->bins[0] == 9);

	std::cout << "test" << std::endl;
	SharedRH1D sMemH(3, {"x", 2, 0, 1});
	RH1D refHist({"x", 2, 0, 1});
	auto ptrH1 = sMemH.at<RH1D>(0);
	auto ptrH2 = sMemH.at<RH1D>(1);

	const double max = static_cast<double>(RAND_MAX);
	for(int i = 0; i < 10; ++i) {
		double x = static_cast<double>(std::rand()) / max;
		double w = static_cast<double>(std::rand()) / max;
		if(std::rand() > RAND_MAX / 2) {
			ptrH1->Fill({x}, w);
		} else {
			ptrH2->Fill({x}, w);
		}
		refHist.Fill({x}, w);
	}
	test.test("Test shared RH1D", 
		std::abs(refHist.GetBinContent({0}) - sMemH.merged<RH1D>()->GetBinContent({0})) 
			< std::numeric_limits<double>::epsilon());
	return 0;
}

