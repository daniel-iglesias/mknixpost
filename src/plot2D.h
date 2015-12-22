#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace cpplot{

class Plot2D{

public:
  
  Plot2D();
  ~Plot2D();

  void setDataFile( std::string );
  
  void setUsing( int , int );
  
  void setWith( std::string );
  
  void setLineType( int );
  
  void setPointType( int );
  
  void setLineWidth( int );
  
  void setPointSize( int );
  
  void setTitle( std::string );
  
  inline std::string getDataFile(){ return sDataFile; }
  
  inline std::string getUsing(){ return sUsing; }
  
  inline std::string getWith(){ return sWith; } 
  
  inline int getLineType(){ return lt; }
  
  inline int getPointType(){ return pt; }
  
  inline int getLineWidth(){ return lw; }
  
  inline int getPointSize(){ return ps; }
  
  inline std::string getTitle(){ return sTitle; }
  
private:

  std::string sDataFile;
  std::string sUsing;
  std::string sWith;
  std::string sTitle;
  int lt;
  int pt;
  int lw;
  int ps;
  int xCol, yCol ;
};
}
