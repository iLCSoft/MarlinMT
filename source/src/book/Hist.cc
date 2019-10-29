#include "marlin/book/MemLayout.h"
#include "marlin/book/Handle.h"

#include "marlin/book/ROOTAdapter.h"
#include "marlin/book/Hist.h"


namespace marlin::book {

template<int D, typename T, template<int, class>class ... STAT>
Handle<RH<D, T, STAT ...>>::Handle(
	std::shared_ptr<MemLayout> obj,
	const typename Handle<RH<D, T, STAT ...>>::FillFn_t& fillFn
	) : BaseHandle<RH<D, T, STAT ...>>{obj}, _fillFn{fillFn}
	{}

template<int D, typename T, template<int, class>class ... STAT>
void Handle<RH<D, T, STAT ...>>::fill(
	const typename Handle<RH<D, T, STAT ...>>::CoordArray_t& x,
	const typename Handle<RH<D, T, STAT ...>>::Weight_t& w
) {
	_fillFn(x, w);
}

template<int D, typename T, template<int, class>class ... STAT>
EntrySingle<RH<D, T, STAT ...>>::EntrySingle(const Context& context) 
	: _context {context}
{}

template<int D, typename T, template<int, class>class ... STAT>
Handle<RH<D, T, STAT ...>> EntrySingle<RH<D, T, STAT ...>>::handle() {
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

template<int D, typename T, template<int, class>class ... STAT>
EntryMultiCopy<RH<D, T, STAT ...>>::EntryMultiCopy(const Context& context)
	: _context {context} {}

template<int D, typename T, template<int, class>class ... STAT>
Handle<RH<D, T, STAT ...>>
EntryMultiCopy<RH<D, T, STAT ...>>::handle(std::size_t idx) {
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

#define LinkType( TYPE ) \
	template class Handle< TYPE >; 				\
	template class EntrySingle< TYPE >;		\
	template class EntryMultiCopy< TYPE > \

	LinkType(RH1D);
	LinkType(RH1F);
	LinkType(RH1I);
}

