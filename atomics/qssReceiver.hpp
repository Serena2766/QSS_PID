/**
* Mengyao Wu
* Carleton University
*
* QssReceiver:
* Receive events which contains y and k info, reconstruct the signal
*/

#ifndef BOOST_SIMULATION_PDEVS_QSS_RECEIVER_HPP
#define BOOST_SIMULATION_PDEVS_QSS_RECEIVER_HPP


//#include "../data_structures/QSS1.hpp"

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>


using namespace cadmium;
using namespace std;

//Port definition
    struct qssReceiver_defs {
        struct out : public out_port<float> { };
        struct eventY : public in_port<float> { };
        struct eventK : public in_port<float> { };
    };

    template<typename TIME>
    class QssReceiver {
        using defs=qssReceiver_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            TIME    sigma;
            // default constructor
            QssReceiver() noexcept{
              sigma = std::numeric_limits<TIME>::infinity();
              state.currentY = 0.0;
              state.currentK = 0.0;
              state.steps = 0;
            }
            
            // state definition
            struct state_type{
              float currentY;
              float currentK;
              int steps;
            }; 
            state_type state;
            // ports definition

            using input_ports=std::tuple<typename defs::eventY,typename defs::eventK>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
                
                state.currentY += state.currentK;
                sigma = TIME("00:00:00:1");
                state.steps ++;
                #ifdef RT_ARM_MBED
                    pc.printf("QssReceiver Called: %d\n",state.steps);
                #endif
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                
                vector<float> y;
                y = get_messages<typename qssReceiver_defs::eventY>(mbs);
                
                if (y.size())
                {
                    state.currentY = y[0];
                    state.steps = 0;
                }
                
                
                vector<float> k;
                k = get_messages<typename qssReceiver_defs::eventK>(mbs);                
                
                if (k.size())
                {
                    state.currentK = k[0];
                }               
                
                
                if (sigma == std::numeric_limits<TIME>::infinity())
                {
                    sigma = TIME("00:00:00:00");
                }
                else
                {
                    sigma = sigma - e;
                }

            }
            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              internal_transition();
              external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;

              get_messages<typename defs::out>(bags).push_back(state.currentY);
                
              return bags;
            }

            // time_advance function
            TIME time_advance() const {
                return sigma;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename QssReceiver<TIME>::state_type& i) {
              os << "Output: " << (i.currentY); 
              return os;
            }
        };     


#endif