#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "QSS1.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const QSS1_t& msg) {
  os << msg.y << " " << msg.k;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, QSS1_t& msg) {
  is >> msg.y;
  is >> msg.k;
  return is;
}
