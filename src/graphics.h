#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

#include "plot2D.h"

namespace cpplot{

class Graphics{

public:
  
  Graphics( std::string );
  ~Graphics();
  
  void setEncoding( std::string );
  
  void setTitle( std::string );
  
  void setXtitle( std::string );
  
  void setYtitle( std::string );
  
  void setGrid( std::string );
  
  void setXrange( double , double );
  
  void setYrange( double , double );
  
  void addPlot( Plot2D );
  
  void setOutput( std::string );
  
  void getPlot();
  
  inline std::string getEncoding(){ return encoding; }
  
  inline std::string getGrid(){ return grid; }
  
  inline std::string getTitle(){ return title; }

  inline std::string getXtitle(){ return xTitle; }
  
  inline std::string getYtitle(){ return yTitle; }
  
  inline std::string getXrange(){ return xRange; }
  
  inline std::string getYrange(){ return yRange; }
  
  inline std::string getOutput(){ return output; }

private:

  std::string oFileName;
  std::string encoding;
  std::string grid;
  std::string title;
  std::string xTitle;
  std::string yTitle;
  std::string xRange; 
  std::string yRange;  
  double xStart , xEnd ;
  double yStart , yEnd ;
  std::string output;
  
  std::ofstream oFile;
  std::ifstream iFile;
  
  std::vector< Plot2D > p2D;
};
}
