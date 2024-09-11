#include "dbppguiappl15.h"
#include <iostream>
// Qt includes
#include <QApplication>
// DamBreak package include
#include "Utility/dbpp_CommandLineArgs.h"

// =================================================================
//
//                    Main Entry Point
//
// =================================================================

int main(int argc, char *argv[]) {
  // Initialize the command line args for use in the DamBreak++ Simulator
  dbpp::CmdLineArgs::init(argc, const_cast<const char **>(argv));
  if (dbpp::CmdLineArgs::hasSwitch("-h")) {
    std::cout << "usage:" << argv[0]
              << " [-h] [-images] [-func N] [-grid <grid>]" << endl;
    std::cout << "\t-h      : display this text" << endl;
    std::cout << "\t-phi    : dambreak parameters" << endl;
    std::cout << "\t-func N : specify function to use (default is 1)" << endl;
    std::cout << "\t-grid \"[x1,y1]x[x2,y2] [size1xsize2]\" : define the grid"
              << endl;

    exit(EXIT_FAILURE); // should never do such a thing
  }
  QApplication a(argc, argv);
  DBppGuiAppl15 w;
  w.show();
  return a.exec();
}
