
struct Entry {
  std::size_t     _ncores {0} ;
  std::size_t     _crunchTime {0} ;
  double          _crunchSigma {0.} ;
  double          _serialTime {0.} ;
  double          _parallelTime {0.} ;
  double          _speedup {0.} ;
};

static const std::vector<int> plotColors = { kGray, kBlue, kRed, kViolet, kOrange, kGreen, kOrange, kAzure } ; 

int getColor( int index ) {
  int trueIndex = index % plotColors.size() ;
  int colorShift = (index / plotColors.size() ) + 1 ;
  return plotColors[ trueIndex ] + colorShift ;
}


void PlotScaling( const std::string &fname, const std::string &title = "", bool save = false ) {


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

  TCanvas *speedupVsCoreCanvas = new TCanvas( "speedupVsCoreCanvas", "Speedup Vs #Core", 800, 800 ) ;
  speedupVsCoreCanvas->SetMargin( 0.130326, 0.0538847, 0.130491, 0.0917313 ) ;
  TMultiGraph *speedupVsCoreMultiGraph = new TMultiGraph() ;
  
  // loop over crunch times
  int color = 0 ;
  size_t maxCores = 0 ;
  for( auto ctIter : crunchTimeToEntryMap ) {
    
    auto crunchTime = ctIter.first ;
    std::stringstream title ; title << crunchTime ;
    if( ctIter.second.at(0)._crunchSigma != 0. ) {
      title << " #pm " << ctIter.second.at(0)._crunchSigma ;
    }
    title << " ms" ;

    TGraph *crunchTimeGraph = new TGraph( ctIter.second.size() ) ;
    crunchTimeGraph->SetName( title.str().c_str() ) ;
    crunchTimeGraph->SetTitle( title.str().c_str() ) ;
    speedupVsCoreMultiGraph->Add( crunchTimeGraph ) ;
    crunchTimeGraph->SetLineWidth( 3 ) ;
    crunchTimeGraph->SetLineColor( getColor(color) ) ;
    crunchTimeGraph->SetMarkerStyle( 0 ) ;

    // loop over ncores for a given crunch time
    for( unsigned int i=0 ; i<ctIter.second.size() ; i++ ) {
      auto &entry = ctIter.second.at( i ) ;
      crunchTimeGraph->SetPoint( i, entry._ncores, entry._speedup ) ;
      maxCores = std::max( maxCores, entry._ncores ) ;
    }
    color++ ;
  }

  speedupVsCoreCanvas->cd() ;
  speedupVsCoreCanvas->SetGridx();
  speedupVsCoreCanvas->SetGridy();  

  if( not title.empty() ) {
    speedupVsCoreMultiGraph->SetTitle( title.c_str() ) ;
  }

  speedupVsCoreMultiGraph->Draw( "alp" ) ;
  speedupVsCoreMultiGraph->GetXaxis()->SetTitle( "# Cores" ) ;
  speedupVsCoreMultiGraph->GetYaxis()->SetTitle( "Speedup" ) ;
  speedupVsCoreMultiGraph->GetXaxis()->SetTitleSize( 0.05 ) ;
  speedupVsCoreMultiGraph->GetYaxis()->SetTitleSize( 0.05 ) ;
  speedupVsCoreMultiGraph->GetXaxis()->SetRangeUser( 0, maxCores+1 ) ;
  speedupVsCoreMultiGraph->GetYaxis()->SetRangeUser( 0, maxCores+1 ) ;
  
  auto legend = speedupVsCoreCanvas->BuildLegend( 0.15, 0.47, 0.47, 0.89, "" ) ;
  legend->SetTextSize( 0.035 ) ;
  legend->SetHeader( "Crunch times", "C" ) ;
  legend->SetBorderSize( 1 ) ;
  
  TF1 *xyline = new TF1( "y=x", "x", 0., static_cast<float>( maxCores+1 ) ) ;
  xyline->SetLineColor( kBlack ) ;
  xyline->SetLineStyle( 7 ) ;
  xyline->SetLineWidth( 2 ) ;
  xyline->Draw( "same" ) ;
  
  TText *t = new TText( 0.7, 0.72, "Perfect scaling" ) ;
  t->SetNDC() ;
  t->SetTextAlign(22);
  t->SetTextColor(kGray+1);
  t->SetTextFont(43);
  t->SetTextSize(38);
  t->SetTextAngle(45);
  t->Draw();
  
  speedupVsCoreCanvas->GetFrame()->SetFillColor( 19 ) ;

  if( save ) {
    speedupVsCoreCanvas->SaveAs( (fname + "SpeedupVSNCores.pdf").c_str() ) ;
  }

}
