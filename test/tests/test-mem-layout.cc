#include <iostream>
#include <UnitTesting.h>
#include "marlin/book/MemLayout.h"
#include "ROOT/RHist.hxx"
struct Type1 {
	std::array<int, 5> bins;
	Type1(int a0, int a1, int a2, int a3, int a4) {
		bins[0] = a0;
		bins[1] = a1;
		bins[2] = a2;
		bins[3] = a3;
		bins[4] = a4;
	}
};

void MergeType1(std::shared_ptr<Type1>& p1, std::shared_ptr<Type1>& p2) {
	for(int i = 0; i < 5; ++i) {
		p1->bins[i] += p2->bins[i];
	}
}

template<typename ... Args_t>
class SharedType1 : public SharedMemLayout<Type1, MergeType1, Args_t ... > {
public:
	SharedType1 (std::size_t num_inst, Args_t ... args) : SharedMemLayout<Type1, MergeType1, Args_t ... >{num_inst, args ...}{}
};

int main (int, char**) {
	marlin::test::UnitTest test(" MemLayout Test ");
	SharedType1<int, int, int, int, int> sMem(3, 1, 0, 0, 0, 0);
	auto ptr1 = sMem.at<Type1>(0);
	auto ptr2 = sMem.at<Type1>(1);
	
	ptr1->bins[0] += 3;
	ptr2->bins[0] += 2;
	
	test.test("bin content test ", sMem.merged<Type1>()->bins[0] == 8);
	return 0;
}

