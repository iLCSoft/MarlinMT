#include <vector>
#include <string>
#include <cstring>
#include <exception>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <locale>
#include <chrono>
#include <optional>
#include <tuple>
#include "TGraph.h" 
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TFrame.h"
#include "TText.h"

enum struct Alignment : short { LEFT = 1, TOP=1, CENTER = 2, RIGHT = 3, BOTTOM=3};
short RootAligment(const Alignment horizontal, const Alignment vertical)  {
	return static_cast<short>(horizontal) * 10 + static_cast<short>(vertical);
}

std::istream& operator>>(std::istream& is, std::chrono::duration<float>& t) {
	float m, s;
	char mc, sc;
	is >> m >> mc >> s >> sc;
	if (mc != 'm' || sc != 's') {
		std::cerr << m << mc << s << sc << '\n'; 
		throw std::runtime_error("can't parse time!");
	}
	t = 
		std::chrono::duration<float>(s)
		+ std::chrono::duration<float, std::ratio<60>>(m);
	return is;
}
enum struct MemLayout {NOTSET, Copy, Share};
enum struct ProcessorType { NOTSET, WorstCase, BestCase };

std::istream& operator>>(std::istream& is, ProcessorType& type) {
	std::string str;
	std::getline(is, str, ',');
	is.unget();
	if (str == "Rotating") { type = ProcessorType::WorstCase ; }
	else if (str == "Continuous") { type = ProcessorType::BestCase ; }
	else if (str == "null") { type = ProcessorType::NOTSET; }
	else { 
		throw std::runtime_error(
				"unknown processorType: '" + str + "'"); }
	return is;
}

std::istream& operator>>(std::istream& is, MemLayout& layout) {
	std::string str;
	std::getline(is,str,',');
	is.unget();
	if (str == "Copy") { layout = MemLayout::Copy; }
	else if (str == "Share") { layout = MemLayout::Share; }
	else if (str == "null") { layout = MemLayout::NOTSET; }
	else {
		throw std::runtime_error(
			"unknown memLayout '" + str + "'");
	}
	return is;
}

std::istream& operator>>(std::istream& is, std::optional<bool>& b) {
	std::string str;
	std::getline(is, str, ',');
	is.unget();
	if (std::size_t pos = str.find('\n') != std::string::npos ) {
		std::size_t diff = str.size() - pos;
		for(auto i = 0; i < diff; ++i) {
			is.unget();	
		}
		 
	}
	if (str == "true") { b = true; }
	else if (str == "false") { b = false; }
	else if (str == "null") { b = std::nullopt; }
	else {
		throw std::runtime_error(
			"unknown bool: '" + str + "'"
				);
	}
	return is;
}

struct Storage : public std::tuple<MemLayout, std::optional<bool>, std::optional<bool>>{
	using std::tuple<MemLayout, std::optional<bool>, std::optional<bool>>::tuple;
};

namespace std {
	string to_string(const MemLayout& layout) {
		switch (layout) {
			case MemLayout::Copy: return std::string("Copy");
			case MemLayout::Share: return std::string("Share");
			default: throw std::runtime_error("memlayout can't be stringified.");
		};
	}
	string to_string(const ProcessorType& type) {
		switch (type) {
			case ProcessorType::BestCase: return std::string("continues");
			case ProcessorType::WorstCase: return std::string("rotating");
			default: throw std::runtime_error("processorType can't be strigified.");
		}
	}
	string to_string(const Storage& store) {
		if (!std::get<2>(store).value()) return std::string("No Filling");
		if (std::get<1>(store).value()) return std::string("Mutex");
		return std::to_string(std::get<0>(store));
	}
} // end namespace std

struct Entry {
  std::size_t     							_ncores {0} ;
	std::size_t 									_nFills {0} ;
	ProcessorType   							_processorType {ProcessorType::NOTSET};
	std::optional<bool>						_mutex {std::nullopt};
	MemLayout 										_memLayout {MemLayout::NOTSET};
	std::size_t 									_nHist {0} ;
	std::size_t 									_nBins {0} ;
  double          							_serialTime {0.} ;
  double          							_parallelTime {0.} ;
  double          							_speedup {0.} ;
	std::chrono::duration<float>  _tReal {0};
	double 												_tUnpack {0};
	double 												_tFill {0};
	std::size_t 									_nEvents {0};		
	std::optional<bool>						_fillingActive {std::nullopt};
	
	static char& checkDelim(char& c) {
		if (c != ',') {
			std::cerr << "wrong delimiter '" << c << "' used!";
			throw std::runtime_error("wrong delimiter") ;	
		}
		return c;
	}
	friend std::istream& operator>>(std::istream& is, Entry& entry) {
		char c;
		if (is >> entry._ncores >> c) {
			if ( is >> entry._nFills >> checkDelim(c)) 
				if (is >> entry._processorType >> checkDelim(c))
					if ( operator>>(is, entry._mutex) >> checkDelim(c))
						if ( is >> entry._memLayout >> checkDelim(c))
							if ( is >> entry._nHist >> checkDelim(c))
								if ( is >> entry._nBins >> checkDelim(c))
									if ( is >> entry._serialTime >> checkDelim(c))
										if ( is >> entry._parallelTime >> checkDelim(c))
											if ( is >> entry._speedup >> checkDelim(c))
												if ( is >> entry._tReal >> checkDelim(c))
													if ( is >> entry._tUnpack >> checkDelim(c))
														if ( is >> entry._tFill >> checkDelim(c))
															if ( is >> entry._nEvents >> checkDelim(c)) 
																if (operator>> (is, entry._fillingActive)) {
																		checkDelim(c);
																		return is;
																	}
			std::string str;
			std::getline(is, str);
			throw std::runtime_error("Entry parsing error!->" + str);
		} else { return is; } 
	}
	friend std::ostream& operator<<(std::ostream& os, const Entry& entry) {
		/*os
			<< entry._ncores << ", "
			<< entry._nFills << ", "
			<< entry._processorType << ", "
			<< entry._mutex << ", "
			<< entry._memLayout << ", "
			<< entry._nHist << ", "
			<< entry._serialTime << ", "
			<< entry._parallelTime << ", "
			<< entry._speedup << ", "
			<< entry._tReal << ", "
			<< entry._tUnpack << ", "
			<< entry._tFill << ", "
			<< entry._nEvents; */
		return os;
	}
	static bool checkHeader(const std::string_view& sv) {
		static const char exp[] = "concurrency,nFills,accessType,mutex,"
			"memoryLayout,nHists,nBins,tSerial,tParallel,scaling,"
			"tReal,tUnpack,tFill,nEvents,fillingActive"  ; 
		bool res =  strncasecmp(exp, sv.data(), sizeof(exp)) == 0;
		if (!res) {
			std::cerr << "CSV header missmatch!:\n\texpected: "
				<< exp << "\n\tget:      " << sv << '\n';
		}
		return res;
	}
};

static const std::vector<int> plotColors = { kGray, kBlue, kRed, kViolet, kOrange, kGreen, kOrange, kAzure } ; 

int getColor( int index ) {
  int trueIndex = index % plotColors.size() ;
  int colorShift = (index / plotColors.size() ) + 1 ;
  return plotColors[ trueIndex ] + colorShift ;
}

template<typename MapFrom_t>
using map_t = std::map<MapFrom_t, std::vector<Entry>, typename MapFrom_t::Compare>;
template<typename MapFrom_t>
using itr_t = typename map_t<MapFrom_t>::const_iterator;

template<typename MapFrom_t, std::size_t I = 0> 
void printForEach(
	const std::string& name,
	TCanvas* canvas,
	std::array<double, 2> maxXY,
	itr_t<MapFrom_t>  begin, itr_t<MapFrom_t> end)
{
	constexpr bool multiY = I == MapFrom_t::dim;
	constexpr std::size_t id = multiY
		? -1
		: static_cast<std::size_t>( MapFrom_t::permutation[I]);
	constexpr const char* const paramName = multiY
		? ""
		: MapFrom_t::valName[id];
	if constexpr (
			(MapFrom_t::yValues == 1 && I + 1 == MapFrom_t::dim)
			|| I == MapFrom_t::dim ) {
		int color = 0;
		TMultiGraph *mGraph = new TMultiGraph();
		
		auto MakeGraph = [&](itr_t<MapFrom_t> itr, std::size_t run) {
			const auto& entries = itr->second;
			std::string key_str;
			if constexpr (multiY) {
				key_str = std::string(MapFrom_t::yNames[run]);
			} else {
				key_str = std::to_string(std::get<id>(itr->first.values));
			}
			TGraph *graph = new TGraph(entries.size());
			graph->SetName(key_str.c_str());
			graph->SetTitle(key_str.c_str());
			mGraph->Add(graph);
			graph->SetLineWidth( 3 );
			graph->SetLineColor( getColor(color) );
			graph->SetMarkerStyle( 0 ) ;

			for(std::size_t i = 0; i < entries.size(); ++i) {
				const Entry& entry = entries[i];
				float x = MapFrom_t::getX(entry), y = MapFrom_t::getY(entry, run);
				graph->SetPoint(i, x, y); 
			}
		};
		auto bItr = begin;
		if (++bItr != end) {
			if (MapFrom_t::yValues != 1) { throw std::runtime_error("only support 1 varriety!"); }

			for(auto itr = begin; itr != end; ++itr, ++color) {
				MakeGraph(itr, 0);
			}
		} else {
			for (std::size_t i = 0; i < MapFrom_t::yValues; ++i, ++color) {
				MakeGraph(begin, i);
			}
		}

		canvas->cd();
		canvas->SetGridx();
		canvas->SetGridy();

		mGraph->SetTitle(MapFrom_t::title);

		mGraph->Draw("alp");
		TAxis *x = mGraph->GetXaxis(), *y = mGraph->GetYaxis();
		x->SetTitle(MapFrom_t::titleX);
		y->SetTitle(MapFrom_t::titleY);
		x->SetTitleSize( 0.05 );
		y->SetTitleSize( 0.05 );
		x->SetRangeUser(0, std::get<0>(maxXY));
		y->SetRangeUser(0, std::get<MapFrom_t::bLinear?0:1>(maxXY));


		TLegend* legend = MapFrom_t::aLegend == Alignment::LEFT
			? canvas->BuildLegend(0.15, 0.47, 0.47, 0.89, "")
			: canvas->BuildLegend(0.7, 0.5, 1., 0.9, "");
		legend->SetTextSize(0.035);
		legend->SetHeader(paramName, "C");
		legend->SetBorderSize( 1 );
		legend->SetFillColorAlpha(kWhite, 0.5f);
		if constexpr ( MapFrom_t::bLinear ) {
			TF1 *xyline = new TF1("y=x", "x", 0., std::get<0>(maxXY));
			xyline->SetFillColor( kBlack );
			xyline->SetLineStyle( 7 );
			xyline->SetLineWidth( 2 );
			xyline->Draw("same");
		}

		TText *t = new TText(0.01, 0.95, name.c_str());
		t->SetNDC();
		t->SetTextAlign(13);
		t->SetTextColor(kGray+1);
		t->SetTextFont(43);
		t->SetTextSize(24);
		t->Draw();

		canvas->GetFrame()->SetFillColor(19);
		canvas->SaveAs((name + ".pdf").c_str());
	} else {
		auto b = begin;
		auto itr = begin;
		do {
			auto key = std::get<id>(b->first.values);
			while(itr != end && std::get<id>((++itr)->first.values) == std::get<id>(b->first.values)); 	
			printForEach<MapFrom_t, I+1>(
					name + paramName + std::to_string(key).c_str(), 
					canvas, maxXY,
					b, itr);
			b = itr;
		} while(itr != end);
	}
}

template<typename MapFrom_t>
void PlotScalingBookStore( 
		const std::string &fname, 
		const std::string &name ) 
{
	// setup
  std::ifstream file ( fname ) ;
  if( not file ) {
    throw std::runtime_error( "Input file is invalid" ) ;
  }
	map_t<MapFrom_t>  tToEntries;
	
	// parsing
	Entry entry;
	std::string header;
	std::getline(file, header);
	if ( not Entry::checkHeader(header) ) {
		throw std::runtime_error("Input has different csv header");
	}
	std::array<double, 2> maxXY{0,0};
  while( file >> entry ) {
		if (!MapFrom_t::filter(entry)) continue;
		std::get<0>(maxXY) = std::max<double>(MapFrom_t::getX(entry), std::get<0>(maxXY));
		for(std::size_t i = 0; i < MapFrom_t::yValues; ++i) {
			std::get<1>(maxXY) = std::max<double>(MapFrom_t::getY(entry, i), std::get<1>(maxXY));
		}
		tToEntries[MapFrom_t(entry)].push_back(entry);
  }

	if (tToEntries.empty()) {
		throw std::runtime_error("no entries left after filter!");
	}

	TCanvas *canvas = new TCanvas( MapFrom_t::name, MapFrom_t::title, 800, 800);
  canvas->SetMargin( 0.130326, 0.0538847, 0.130491, 0.0917313 ) ;

	std::get<0>(maxXY) *= 1.1;
	std::get<1>(maxXY) *= 1.1;
	printForEach<MapFrom_t>(name, canvas, maxXY, tToEntries.begin(), tToEntries.end());
}

// helper functions
template<typename Obj, typename T>
std::optional<bool> compare (
		const Obj& lh, 
		const Obj& rh,
		const T Obj::* field)  
{
	if ( lh.*field != rh.*field ) {
		return lh.*field < rh.*field;
	}
	return std::nullopt;
}


template<typename T>
using Permutation_t = std::array<T, static_cast<std::size_t>(T::SIZE)>;

template<typename MapFrom_t>
struct PermCompare {
	bool operator()(const MapFrom_t& lh, const MapFrom_t& rh) const {
		return compare(lh, rh);
	}
	private:
		template<std::size_t I = 0>
		bool compare(const MapFrom_t& lh, const MapFrom_t& rh) const {
			constexpr std::size_t id = 
				static_cast<std::size_t>(MapFrom_t::permutation[I]);
			auto lha = std::get<id>(lh.values);
			auto rha = std::get<id>(rh.values);
			if (lha != rha ) {
				return lha < rha; 
			}
			if constexpr ( I + 1 < MapFrom_t::permutation.size()) {
				return compare<I+1>(lh, rh);
			} else {
				return false;
			}
		}
};

// output diagrams
// scaling from filling depending on NBins, NHists, NFills, Crunch = 0
struct ScalingToCores {
	static constexpr std::size_t yValues = 1;
	static constexpr Alignment aLegend{Alignment::LEFT};
	static constexpr bool bLinear{true};
	enum struct Values { 
		NHists, 
		Type,
		Layout,
		SIZE};
	static constexpr Permutation_t<Values> permutation = {
		Values::Type,
		Values::NHists,
		Values::Layout,
	}	;
	static constexpr const char* valName[] = {
		"NHists", 
		"Type",
		"Storage"
		};
	std::tuple<
		std::size_t, 
		ProcessorType,
		Storage> values;
	template<std::size_t I>
	using value_t = std::tuple_element_t<I, decltype(values)>;
	static constexpr std::size_t dim = std::tuple_size_v<decltype(values)>;
	static constexpr char name[] = "ScalingToCores";
	static constexpr char title[] = "Scaling for #Cores";
	static constexpr char titleX[] = "# Cores" ;
	static constexpr char titleY[] = "Scaling";
	ScalingToCores() = default;
	ScalingToCores(const Entry& e) : 
		values{e._nHist, e._processorType, {e._memLayout, e._mutex, e._fillingActive}} {}
	static float getX(const Entry& e) { return e._ncores; }
	static float getY(const Entry& e, std::size_t r ) { if (r != 0) { throw std::runtime_error("only supports 1 Y value!");} return e._speedup; }
	struct Filter {
		bool operator()(const Entry& e) const {
			return true;
		}
	};
	static bool filter(const Entry& e) {
		constexpr Filter f{};
		return f(e);
	}
	using Compare = PermCompare<ScalingToCores>;
};


struct TimeToCores : public ScalingToCores {
	using ScalingToCores::ScalingToCores;
	static constexpr bool bLinear{false};
	static constexpr char name[] = "TimeToCores";
	static constexpr char title[] = "Time for #Cores";
	static constexpr char titleY[] = "Time in s";
	static float getX(const Entry& e) { return e._ncores; }
	static float getY(const Entry& e, std::size_t r) { if (r!=0) {throw std::runtime_error("only supports 1 Y value!"); }return e._tFill; }
	struct Filter {
		bool operator()(const Entry& e) const {
			return e._fillingActive.value();
		}
	};
	static bool filter(const Entry& e) {
		constexpr Filter f{};
		return f(e);
	}
};

struct TimesToCores : public TimeToCores{
		using TimeToCores::TimeToCores;
		static constexpr char name[] = "TimesToCores";
		static constexpr char title[] = "Times for #Cores";
		static constexpr std::size_t yValues = 4;
		static constexpr const char* yNames[] = {
			"real",
			"unpack",
			"fill",
			"serial"
		};
		static float getX(const Entry& e) { return e._ncores; }
		static float getY(const Entry& e, std::size_t r) {
			switch (r) {
				case 0: return e._tReal.count();
				case 1: return e._tUnpack;
				case 2: return e._tFill;
				case 3: return e._serialTime;
				default: throw std::runtime_error("only supports 3 Y values! Got '" + std::to_string(r) + "'");
			}
		}
};


auto PlotScalingToCore = PlotScalingBookStore<ScalingToCores>;
auto PlotTimeToCore = PlotScalingBookStore<TimeToCores>;
auto PlotTimesToCore = PlotScalingBookStore<TimesToCores>;
