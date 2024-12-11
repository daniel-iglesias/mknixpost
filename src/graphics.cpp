#include "graphics.h"

namespace cpplot{

Graphics::Graphics( std::string oFileName_in )
{
  oFileName = oFileName_in ;
  setEncoding( "iso_8859_15" );
  setGrid( "on" );
  setTitle( "MkniX simulation" );
  setXtitle( "X-title" );
  setYtitle( "Y-title" );
  setXrange( 0.0 , 0.0 );
  setYrange( 0.0 , 0.0 );
}

Graphics::~Graphics()
{

}

void Graphics::setEncoding( std::string encoding_in )
{
  encoding = "set encoding " + encoding_in ;
}

void Graphics::setTitle( std::string title_in )
{
  title = "set title \"" + title_in + "\"";
}

void Graphics::setXtitle( std::string title_in )
{
  xTitle = "set xlabel \"" + title_in + "\"";
}

void Graphics::setYtitle( std::string title_in )
{
  yTitle = "set ylabel \"" + title_in + "\"";
}

void Graphics::setGrid( std::string grid_in )
{
  if( grid_in == "on" )
  {
    grid = "set grid" ;
  }
  else
  {
    grid = "unset grid" ;
  }
}

void Graphics::setXrange( double xStart_in , double xEnd_in  )
{
  xStart = xStart_in ;
  xEnd   = xEnd_in  ;

  if( xStart < xEnd )
  {
     std::string sXstart;
     std::string sXend;
     
     std::stringstream cStream_xStart;
     std::stringstream cStream_xEnd;
     
     cStream_xStart << xStart;
     cStream_xEnd   << xEnd;
     
     cStream_xStart >> sXstart;
     cStream_xEnd   >> sXend;
     
     xRange = "set xrange ["+ sXstart +":"+ sXend +"]";
   }
   else
   {
      xRange = "set xrange [*:*]";
    }

}

void Graphics::setYrange( double yStart_in , double yEnd_in  )
{
  yStart = yStart_in ;
  yEnd   = yEnd_in  ;

  if( yStart < yEnd )
  {
     std::string sYstart;
     std::string sYend;
     
     std::stringstream cStream_yStart;
     std::stringstream cStream_yEnd;
     
     cStream_yStart << yStart;
     cStream_yEnd   << yEnd;
     
     cStream_yStart >> sYstart;
     cStream_yEnd   >> sYend;
     
     yRange = "set yrange ["+ sYstart +":"+ sYend +"]";
   }
   else
   {
      yRange = "set yrange [*:*]";
    }

}

void Graphics::setOutput( std::string output_in )
{
  output = output_in ;
}

void Graphics::addPlot( Plot2D p2D_in )
{
  p2D_in.setLineType( p2D.size()+1 );
  p2D_in.setPointType( p2D.size()+1 );
  p2D.push_back( p2D_in );
  
}

void Graphics::getPlot()
{
  int i;
  
//   std::cout << oFileName << std::endl;
  oFile.open( oFileName.c_str() );
  
  oFile << "#!/usr/bin/gnuplot -persist" << std::endl;
  oFile << "#       G N U P L O T" << std::endl;
  oFile << std::endl;
  oFile << getEncoding() << std::endl;
  oFile << getTitle() << std::endl;
  oFile << getXtitle() << std::endl;
  oFile << getYtitle() << std::endl;
  oFile << std::endl;
  oFile << getGrid() << std::endl;
  oFile << getXrange() << std::endl;
  oFile << getYrange() << std::endl;
  oFile << std::endl;
  oFile << "set key outside horizontal center bottom" << std::endl ;
  oFile << "set term png" << std::endl ;
  oFile << "set output \"" << getOutput() << ".png\"" << std::endl ;
  oFile << std::endl;
  oFile << "plot \\" << std::endl;
  for( i=0 ; i < p2D.size() ; ++i)
  {
    oFile << "\"" << p2D[i].getDataFile() << "\" ";
    oFile << p2D[i].getWith() << " ";
    oFile << "lt " << p2D[i].getLineType()  << " ";
    oFile << "pt " << p2D[i].getPointType() << " ";
    oFile << "lw " << p2D[i].getLineWidth() << " ";
    oFile << "ps " << p2D[i].getPointSize() << " ";
    oFile << "title \"" << p2D[i].getTitle() << "\" ";
    if( i < p2D.size()-1 )
    {
      oFile << ",\\";
    }
    oFile << std::endl;
  }
  
}

}
