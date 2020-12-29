#ifndef BOOST_SIMULATION_QSS1_HPP
#define BOOST_SIMULATION_QSS1_HPP

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** QSS1_t ****************/
/*******************************************/
struct QSS1_t{
  QSS1_t(){}
  QSS1_t(float i_y, float i_k)
   :y(i_y), k(i_k){}

  	float y;
  	float k;
};

istream& operator>> (istream& is, QSS1_t& msg);

ostream& operator<<(ostream& os, const QSS1_t& msg);


#endif // BOOST_SIMULATION_QSS1_HPP
