#include "plot2D.h"

namespace cpplot{

Plot2D::Plot2D()
{

}

Plot2D::~Plot2D()
{

}

void Plot2D::setDataFile( std::string sDataFile_in )
{
  sDataFile = sDataFile_in ;
  setUsing(1,2);
  setWith("lp");
  setLineType( 1 );
  setPointType( 1 );
  setLineWidth( 1 );
  setPointSize( 1 );
  setTitle( sDataFile );
}
  
void Plot2D::setUsing( int xCol_in , int yCol_in )
{
  xCol = xCol_in ;
  yCol = yCol_in ;
  
  std::string sXcol;
  std::string sYcol;
     
  std::stringstream cStream_Xcol;
  std::stringstream cStream_Ycol;
     
  cStream_Xcol << xCol;
  cStream_Ycol << yCol;
     
  cStream_Xcol >> sXcol;
  cStream_Ycol >> sYcol;
     
  sUsing = "using "+ sXcol +":"+ sYcol;
}
  
void Plot2D::setWith( std::string sWith_in )
{
  sWith = sWith_in ;
  
  if( sWith == "l" )
  {
    sWith = "w lines";
  }
  else if( sWith == "p" )
  {
    sWith = "w points";
  }
  else if( sWith == "lp" )
  {
    sWith = "w linespoints";
  }
  else
  {
    sWith = "w linespoints";
  }
}
  
void Plot2D::setLineType( int lt_in )
{
  lt = lt_in ;
}
  
void Plot2D::setPointType( int pt_in )
{
  pt = pt_in ;
}
  
void Plot2D::setLineWidth( int lw_in )
{
  lw = lw_in ;
}
  
void Plot2D::setPointSize( int ps_in )
{
  ps = ps_in ;
}

void Plot2D::setTitle( std::string sTitle_in )
{
  sTitle = sTitle_in ;
}

}
