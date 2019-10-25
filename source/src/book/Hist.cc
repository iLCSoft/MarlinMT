#include "marlin/book/MemLayout.h"
#include "marlin/book/Handle.h"
#include "ROOT/RHist.hxx"
#include "ROOT/RHistData.hxx"

#include "marlin/book/Hist.h"


namespace marlin::book {

template<typename T, int D>
Handle<RH<T, D>>::Handle(
	std::shared_ptr<MemLayout> obj,
	const typename Handle<RH<T, D>>::FillFn_t& fillFn
	) : BaseHandle<RH<T,D>>{obj}, _fillFn{fillFn}
	{}

template<typename T, int D>
void Handle<RH<T, D>>::fill(
	const typename Handle<RH<T,D>>::CoordArray_t& x,
	const typename Handle<RH<T,D>>::Weight_t& w
) {
	_fillFn(x, w);
}

template<typename T, int D>
EntrySingle<RH<T, D>>::EntrySingle(const Context& context) 
	: _context {context}
{}

template<typename T, int D>
Handle<RH<T, D>> EntrySingle<RH<T,D>>::handle() {
	auto hist = _context.mem->at<Type>(0); 
	return Handle<Type>(
		_context.mem,
		[hist = hist](
		const typename Type::CoordArray_t& x,
		const typename Type::Weight_t& w) 
		{
			hist->Fill(x, w);
		}
	);
}

template<typename T, int D>
EntryMultiCopy<RH<T, D>>::EntryMultiCopy(const Context& context) : _context {context} {}

template<typename T, int D>
Handle<RH<T, D>> EntryMultiCopy<RH<T, D>>::handle(std::size_t idx) {
	auto hist = _context.mem->at<Type>(idx);
	return Handle<Type>(
	_context.mem,
	[hist = hist](
		const typename Type::CoordArray_t& x,
		const typename Type::Weight_t& w
		) {
			hist->Fill(x, w);
		}
	);
}

template class Handle<RH<float, 1>>;
template class EntrySingle<RH<float, 1>>;
template class EntryMultiCopy<RH<float, 1>>;
template class Handle<RH<int, 1>>;
template class EntrySingle<RH<int, 1>>;
template class EntryMultiCopy<RH<int, 1>>;
}

