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
		throw std::runtime_error("can't parse time!");
	}
	t = 
		std::chrono::duration<float>(s)
		+ std::chrono::duration<float, std::ratio<60>>(m);
	return is;
}

struct Entry {
  std::size_t     _ncores {0} ;
  std::size_t     _crunchTime {0} ;
  double          _crunchSigma {0.} ;
	std::size_t 		_nHist {0} ;
	std::size_t 		_nFills {0} ;
	std::size_t 		_nBins {0} ;
	std::chrono::duration<float> _tReal {0};
	std::chrono::duration<float> _tUser {0};
	std::chrono::duration<float> _tSys {0};
  double          _serialTime {0.} ;
  double          _parallelTime {0.} ;
  double          _speedup {0.} ;
	static char& checkDelim(char& c) {
		if (c != ',') {
			std::cerr << "wrong delimiter '" << c << "' used!";
			throw std::runtime_error("wrong delimiter") ;	
		}
		return c;
	}
	friend std::istream& operator>>(std::istream& is, Entry& entry) {
		char c;
		if(is>> entry._ncores >> c) {
			if(is	>> entry._crunchTime  >> checkDelim(c)) 
				if(is >> entry._crunchSigma  >> checkDelim(c))
					if(is >> entry._nHist >> checkDelim(c))
						if(is >> entry._nFills >> checkDelim(c))
							if(is >> entry._nBins >> checkDelim(c))
								if(is >> entry._tReal >> checkDelim(c))
									if(is >> entry._tUser >> checkDelim(c))
										if(is >> entry._tSys >> checkDelim(c))
											if(is >> entry._serialTime >> checkDelim(c))
												if(is >> entry._parallelTime  >> checkDelim(c))
													if(is >> entry._speedup) {
														checkDelim(c);
														return is;
													}
			throw std::runtime_error("Entry parsing error!");
		} else { return is; } 
	}
	friend std::ostream& operator<<(std::ostream& os, const Entry& entry) {
		os 
			<< entry._ncores << ", " 
			<< entry._crunchTime << ", "
			<< entry._crunchSigma << ", "
			<< entry._nHist<< ", "
			<< entry._nFills<< ", "
			<< entry._nBins << ", "
			<< entry._tReal.count() << ", "
			<< entry._tUser.count() << ", "
			<< entry._tSys.count() << ", "
			<< entry._serialTime << ", "
			<< entry._parallelTime << ", "
			<< entry._speedup ;
		return os;
	}
	static bool checkHeader(const std::string_view& sv) {
		static const char exp[] = "concurrency,crunchTime,crunchSigma,nHists,nFills,nBins,"
			"tReal,tUser,tSys,tSerial,tParallel,scaling";
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
	itr_t<MapFrom_t>  begin, itr_t<MapFrom_t> end)
{
	constexpr const char* const paramName = MapFrom_t::valName[I];
	if constexpr (I + 1 == MapFrom_t::dim) {
		int color = 0;
		TMultiGraph *mGraph = new TMultiGraph();
		float maxX = 0, maxY = 0;;
		for(auto itr = begin; itr != end; ++itr, ++color) {
			const auto& entries = itr->second;
			auto key = std::get<I>(itr->first.values);
			TGraph *graph = new TGraph(entries.size());
			graph->SetName(std::to_string(key).c_str());
			graph->SetTitle(std::to_string(key).c_str());
			mGraph->Add(graph);
			graph->SetLineWidth( 3 );
			graph->SetLineColor( getColor(color) );
			graph->SetMarkerStyle( 0 ) ;

			for(std::size_t i = 0; i < entries.size(); ++i) {
				const Entry& entry = entries[i];
				float x = MapFrom_t::getX(entry), y = MapFrom_t::getY(entry);
				graph->SetPoint(i, x, y); 
				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);
			}
		}

		canvas->cd();
		canvas->SetGridx();
		canvas->SetGridy();

		mGraph->SetTitle(MapFrom_t::title);

		maxX *= 1.1f;
		maxY *= 1.1f;
		mGraph->Draw("alp");
		TAxis *x = mGraph->GetXaxis(), *y = mGraph->GetYaxis();
		x->SetTitle(MapFrom_t::titleX);
		y->SetTitle(MapFrom_t::titleY);
		x->SetTitleSize( 0.05 );
		y->SetTitleSize( 0.05 );
		x->SetRangeUser(0, maxX);
		y->SetRangeUser(0, MapFrom_t::bLinear ? maxX : maxY);


		TLegend* legend = MapFrom_t::aLegend == Alignment::LEFT
			? canvas->BuildLegend(0.15, 0.47, 0.47, 0.89, "")
			: canvas->BuildLegend(0.7, 0.5, 1., 0.9, "");
		legend->SetTextSize(0.035);
		legend->SetHeader(paramName, "C");
		legend->SetBorderSize( 1 );

		if constexpr ( MapFrom_t::bLinear ) {
			TF1 *xyline = new TF1("y=x", "x", 0., maxX);
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
			auto key = std::get<I>(b->first.values);
			while(itr != end && std::get<I>((++itr)->first.values) == std::get<I>(b->first.values)); 	
			printForEach<MapFrom_t, I+1>(name + paramName + std::to_string(key).c_str(), canvas, b, itr);
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
  while( file >> entry ) {
		if (!MapFrom_t::filter(entry)) continue;
		tToEntries[MapFrom_t(entry)].push_back(entry);
		std::cout << "\ttime: "  << entry._tReal.count() << '\n';
  }

	if (tToEntries.empty()) {
		throw std::runtime_error("no entries left after filter!");
	}

	TCanvas *canvas = new TCanvas( MapFrom_t::name, MapFrom_t::title, 800, 800);
  canvas->SetMargin( 0.130326, 0.0538847, 0.130491, 0.0917313 ) ;

	printForEach<MapFrom_t>(name, canvas, tToEntries.begin(), tToEntries.end());
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



// output diagrams
// scaling from filling depending on NBins, NHists, NFills, Crunch = 0
struct ScalingToCores {
	static constexpr Alignment aLegend{Alignment::LEFT};
	static constexpr bool bLinear{true};
	enum struct Values { NBins, NHists, NFills, SIZE};
	static constexpr const char* valName[] = {"NBins", "NHists", "NFills"};
	std::tuple<std::size_t, std::size_t, std::size_t> values;
	template<std::size_t I>
	using value_t = std::tuple_element_t<I, decltype(values)>;
	static constexpr std::size_t dim = std::tuple_size_v<decltype(values)>;
	static constexpr char name[] = "ScalingToCores";
	static constexpr char title[] = "Scaling for #Cores";
	static constexpr char titleX[] = "# Cores" ;
	static constexpr char titleY[] = "Scaling";
	ScalingToCores() = default;
	ScalingToCores(const Entry& e) : values{e._nBins, e._nHist, e._nFills} {}
	static float getX(const Entry& e) { return e._ncores; }
	static float getY(const Entry& e) { return e._speedup; }
	struct Filter {
		bool operator()(const Entry& e) const {
			return e._crunchTime == 0;	
		}
	};
	static bool filter(const Entry& e) {
		constexpr Filter f{};
		return f(e);
	}

	struct Compare {
		bool operator()(const ScalingToCores& lh, const ScalingToCores& rh) const {
			if (auto res = compare(lh,rh,&ScalingToCores::values)) return res.value();
			return false;
		}
	};
};

// scaling plot with real time
struct ScalingToCoresRealTime {
	static constexpr Alignment aLegend{Alignment::RIGHT} ;
	static constexpr bool bLinear{false};	
	enum struct Values { NBins, NHists, NFills, SIZE};
	static constexpr const char* valName[] = {"NBins", "NHists", "NFills"};
	std::tuple<std::size_t, std::size_t, std::size_t> values;
	template<std::size_t I>
	using value_t = std::tuple_element_t<I, decltype(values)>;
	static constexpr std::size_t dim = std::tuple_size_v<decltype(values)>;
	static constexpr char name[] = "ScalingToCores";
	static constexpr char title[] = "time for #Cores";
	static constexpr char titleX[] = "# Cores";
	static constexpr char titleY[] = "t in sec";
	ScalingToCoresRealTime() = default;
	ScalingToCoresRealTime(const Entry& e) : values{e._nBins, e._nHist, e._nFills}{}
	static float getX(const Entry& e) { return e._ncores; }
	static float getY(const Entry& e) { return e._tReal.count();}
	struct Filter {
		bool operator()(const Entry& e) const {
			return e._crunchTime == 0;
		}
	};
	static bool filter(const Entry& e) {
		constexpr Filter f{};
		return f(e);
	}
	struct Compare {
		bool operator()(const ScalingToCoresRealTime& lh, const ScalingToCoresRealTime& rh) const {
			if (auto res = compare(lh, rh, &ScalingToCoresRealTime::values)) return res.value();
			return false;
		}
	};
};

// how much crunch is possible without losing time
struct CrunchToScale {
	static constexpr Alignment aLegend{Alignment::LEFT} ;
	static constexpr bool bLinear{false};
	enum struct Values { Cores, SIZE};
	static constexpr const char* valName[] = {"Cores"}; 
	std::tuple<std::size_t> values;
	template<std::size_t I>
	using value_t = std::tuple_element_t<I, decltype(values)>;
	static constexpr std::size_t dim = std::tuple_size_v<decltype(values)>;
	static constexpr char name[] = "CrunchToScale";
	static constexpr char title[] = "Crunch ms to Scale";
	static constexpr char titleX[] = "Scale" ;
	static constexpr char titleY[] = "Crunch ms";
	CrunchToScale() = default;
	CrunchToScale(const Entry& e) : values{e._ncores}{}
	static float getX(const Entry& e) { return e._speedup; }
	static float getY(const Entry& e) { return e._crunchTime; }
	struct Filter {
		bool operator()(const Entry& e) const {
			return e._nBins == 1000 
				&& e._nHist == 2020 
				&& e._nFills == 200000;
		}
	};
	static bool filter(const Entry& e) {
		constexpr Filter f{};
		return f(e);
	}
	struct Compare {
		bool operator()(const CrunchToScale& lh, const CrunchToScale& rh) const {
			if (auto res = compare(lh, rh, &CrunchToScale::values)) return res.value();
			return false;
		}
	};
};


auto PlotScalingToCore = PlotScalingBookStore<ScalingToCores>;
auto PlotCrunchToScale = PlotScalingBookStore<CrunchToScale>;
auto PlotTimeToCores = PlotScalingBookStore<ScalingToCoresRealTime>;
