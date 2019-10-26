
struct Entry {
  std::size_t     _ncores {0} ;
  std::size_t     _crunchTime {0} ;
  double          _crunchSigma {0.} ;
  double          _serialTime {0.} ;
  double          _parallelTime {0.} ;
  double          _speedup {0.} ;
};


void PlotScaling( const std::string &fname, const std::string &title = "", bool save = true ) {


  std::ifstream file ( fname ) ;

  if( not file ) {
    throw std::runtime_error( "Input file is invalid" ) ;
  }


  std::map<std::size_t, std::vector<Entry>>   coreToEntryMap ;
  std::map<std::size_t, std::vector<Entry>>   crunchTimeToEntryMap ;


  while( not file.eof() ) {

    std::string line ;
    std::getline( file, line ) ;

    if( line.empty() ) {
      break ;
    }

    std::stringstream ssline ( line ) ;
    Entry entry ;
    
    ssline >> entry._ncores >> entry._crunchTime >> entry._crunchSigma >> entry._serialTime >> entry._parallelTime >> entry._speedup ;
    std::cout << "Parsed entry: " << line << std::endl ;

    coreToEntryMap[ entry._ncores ].push_back( entry ) ;
    crunchTimeToEntryMap[ entry._crunchTime ].push_back( entry ) ;
  }

  TCanvas *speedupVsCoreCanvas = TCanvas::MakeDefCanvas() ;
  // TCanvas *speedupVsCrunchTimeCanvas = TCanvas::MakeDefCanvas() ;
  
  TMultiGraph *speedupVsCoreMultiGraph = new TMultiGraph() ;
  // TMultiGraph *speedupVsCrunchTimeMultiGraph = new TMultiGraph() ;
  
  // loop over crunch times
  int color = 1 ;
  size_t maxCores = 0 ;
  for( auto ctIter : crunchTimeToEntryMap ) {
    
    auto crunchTime = ctIter.first ;
    std::stringstream title ; title << "Crunch time = " << crunchTime << " ms" ;

    TGraph *crunchTimeGraph = new TGraph( ctIter.second.size() ) ;
    crunchTimeGraph->SetName( title.str().c_str() ) ;
    crunchTimeGraph->SetTitle( title.str().c_str() ) ;
    speedupVsCoreMultiGraph->Add( crunchTimeGraph ) ;
    crunchTimeGraph->SetLineWidth( 2 ) ;
    crunchTimeGraph->SetLineColor( color ) ;
    crunchTimeGraph->SetMarkerStyle( 0 ) ;

    // loop over ncores for a given crunch time
    for( unsigned int i=0 ; i<ctIter.second.size() ; i++ ) {
      auto &entry = ctIter.second.at( i ) ;
      crunchTimeGraph->SetPoint( i, entry._ncores, entry._speedup ) ;
      maxCores = std::max( maxCores, entry._ncores ) ;
    }
    color++ ;
  }

  // // loop over ncores
  // color = 1 ;
  // size_t maxCrunchTime = 0 ;
  // for( auto coreIter : coreToEntryMap ) {
    
  //   auto ncores = coreIter.first ;
  //   std::stringstream title ; title << "# Cores = " << ncores ;

  //   TGraph *coreGraph = new TGraph( coreIter.second.size() ) ;
  //   coreGraph->SetName( title.str().c_str() ) ;
  //   coreGraph->SetTitle( title.str().c_str() ) ;
  //   coreGraph->SetLineWidth( 2 ) ;
  //   coreGraph->SetLineColor( color ) ;
  //   coreGraph->SetMarkerStyle( 0 ) ;
  //   speedupVsCrunchTimeMultiGraph->Add( coreGraph ) ;

  //   // loop over crunch time for a given number of core in use
  //   for( unsigned int i=0 ; i<coreIter.second.size() ; i++ ) {
  //     auto &entry = coreIter.second.at( i ) ;
  //     coreGraph->SetPoint( i, entry._crunchTime, entry._speedup ) ;
  //     maxCrunchTime = std::max( maxCrunchTime, entry._crunchTime ) ;
  //   }
  //   color++ ;
  // }

  speedupVsCoreCanvas->cd() ;
  speedupVsCoreCanvas->SetGridx();
  speedupVsCoreCanvas->SetGridy();  

  if( not title.empty() ) {
    speedupVsCoreMultiGraph->SetTitle( title.c_str() ) ;
  }

  speedupVsCoreMultiGraph->Draw( "alp" ) ;
  speedupVsCoreMultiGraph->GetXaxis()->SetTitle( "# Cores" ) ;
  speedupVsCoreMultiGraph->GetYaxis()->SetTitle( "Speedup" ) ;
  
  auto legend = speedupVsCoreCanvas->BuildLegend() ;
  legend->SetBorderSize( 0 ) ;
  
  TF1 *xyline = new TF1( "y=x", "x", 0., static_cast<float>( maxCores ) ) ;
  xyline->SetLineColor( kBlack ) ;
  xyline->SetLineStyle( 7 ) ;
  xyline->SetLineWidth( 1 ) ;
  xyline->Draw( "same" ) ;

  // speedupVsCrunchTimeCanvas->cd() ;
  // speedupVsCrunchTimeCanvas->SetGridx();
  // speedupVsCrunchTimeCanvas->SetGridy();  
  // speedupVsCrunchTimeMultiGraph->Draw( "alp" ) ;
  // speedupVsCrunchTimeMultiGraph->GetXaxis()->SetTitle( "Crunch time [ms]" ) ;
  // speedupVsCrunchTimeMultiGraph->GetYaxis()->SetTitle( "Speedup" ) ;
  
  // legend = speedupVsCrunchTimeCanvas->BuildLegend() ;
  // legend->SetBorderSize( 0 ) ;


  if( save ) {
    speedupVsCoreCanvas->SaveAs( (fname + "SpeedupVSNCores.pdf").c_str() ) ;
    // speedupVsCrunchTimeCanvas->SaveAs( (fname + "SpeedupVSCrunchTime.pdf").c_str() ) ;

  }

}
