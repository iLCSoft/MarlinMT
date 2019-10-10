#pragma once

#include <atomic>
#include <memory>
#include <functional>

#include "ROOT/RHist.hxx"
#include "ROOT/RHistConcurrentFill.hxx"

namespace marlin {

	constexpr int FillerBufferSize = 1024;

	template<std::size_t DIM, typename T> struct hnd_hist_trait;
	template<std::size_t DIM, typename T> class HistHnd;	

	template<>
	struct hnd_hist_trait<1, float> {
		using Type = HistHnd<1, float>; 
		using Hist_t = ROOT::Experimental::RH1F;
	};

	template<std::size_t DIM, typename T>
	class HistHnd {
		friend class BookStore;
		
		using Hist_t = typename hnd_hist_trait<DIM, T>::Hist_t;
		using Filler_t =
			ROOT::Experimental::RHistConcurrentFiller<
				Hist_t,
				FillerBufferSize>;
	public:
		using CoordArry_t = typename Hist_t::CoordArray_t;
		using Weight_t = typename Hist_t::Weight_t;

	private:
		bool _valid;
		Flag_t _f;
		bool _modified;

		std::shared_ptr<void> _filler;
		std::shared_ptr<Hist_t> _resHist;
		
		std::function<
			void(std::shared_ptr<void>&,const CoordArry_t&, const Weight_t&)>				_fnFill;
		static void
		FillConcurrent(std::shared_ptr<void>& filler, const CoordArry_t& x, const Weight_t& w) {
			std::static_pointer_cast<Filler_t>
				(filler)->Fill(x, w);
		}
		static void
		FillDirect(std::shared_ptr<void>& filler, const CoordArry_t& x, const Weight_t& w) {
			std::static_pointer_cast<Hist_t>
			(filler)->Fill(x, w);
		}

		HistHnd(
			const Flag_t& flags,
			const std::shared_ptr<void>& filler,
			const std::shared_ptr<void>& resHist)
			: _valid{true},
			  _f{flags},
			  _modified{false},
			  _filler{filler},
			  _resHist{std::static_pointer_cast<Hist_t>(resHist)},
			  _fnFill{[](std::shared_ptr<void>&, const CoordArry_t&, const Weight_t&){ throw "Bullshit";}}
		{
			_fnFill = flags.Contains(BookFlags::MultiInstance)
				? FillDirect
				: FillConcurrent;
		}
		
		

	public:
		HistHnd() : _valid{false}{}

		void
		Fill(const CoordArry_t& x, const Weight_t& w) {
			if(!_modified) _modified = true;
			_fnFill(_filler, x, w);
		}
		
		const Hist_t&
		GetHist() const {
			return *_resHist;
		}
		
		bool
		isShared() const { return !_f.Contains(BookFlags::MultiInstance);}
		bool
		isDuplicated() const { return _f.Contains(BookFlags::MultiInstance);}
	};
}
