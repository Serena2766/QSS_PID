/**
* Mengyao Wu
* Carleton University
*
* pidController:
* brings a system state variable to desired value using PID algorithm
*/

#ifndef BOOST_SIMULATION_PDEVS_PID_CONTROLLER_HPP
#define BOOST_SIMULATION_PDEVS_PID_CONTROLLER_HPP


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

//define the P,D tuning range, depends on the specific application
#define P_FULL 10
#define D_FULL 10

#define UPPER_BOUND 1.0
#define LOWER_BOUND -1.0
using namespace cadmium;
using namespace std;

//Port definition
    struct pidController_defs {
        struct out : public out_port<float> { };
        struct desired : public in_port<float> { };
        struct measured : public in_port<float> { };
        struct tuneP : public in_port<float> { };
        struct tuneD : public in_port<float> { };
    };

    template<typename TIME>
    class PidController {
        using defs=pidController_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            TIME    sigma;
            // default constructor
            PidController() noexcept{
              sigma = TIME("00:00:00:000");
              state.desired = 0;
              state.measured = 0;
              state.currentError = 0;
              state.previousError = 0;
              state.Kp = 0;
              state.Kd = 0;
              state.steps = 0;
              state.correctionValue = 0;
            }
            
            // state definition
            struct state_type{
              float desired;
              float measured;
              float currentError;
              float previousError;
              float Kp;
              float Kd;
              int steps;
              float correctionValue;
            }; 
            state_type state;
            // ports definition

            using input_ports=std::tuple<typename defs::desired,typename defs::measured,typename defs::tuneD,typename defs::tuneP>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
                
                //use time advance and steps to track local time
                //pid controller use this local time reference for elapsed time 
                sigma = TIME("00:00:00:1");
                state.steps ++;
                #ifdef RT_ARM_MBED
                    pc.printf("pidController Called: %d\n",state.steps);
                #endif
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                
                vector<float> y;
                y = get_messages<typename pidController_defs::desired>(mbs);
                
                if (y.size())
                {
                    state.desired = y[0];
                }
                
                
                vector<float> k;
                k = get_messages<typename pidController_defs::measured>(mbs);                
                
                if (k.size())
                {
                    state.measured = k[0];
                    state.previousError = state.currentError;
                    state.currentError = state.desired - state.measured;
                    
                    float deltaError = state.currentError - state.previousError;
                    
                    state.correctionValue = state.Kp*state.currentError;
                    
                    if (state.steps != 0)
                    {
                        state.correctionValue += state.Kd*deltaError/state.steps;
                    }
                    state.steps = 0;
                    
                    if (state.correctionValue >= UPPER_BOUND)
                        state.correctionValue = UPPER_BOUND;
                    
                    if (state.correctionValue <= LOWER_BOUND)
                        state.correctionValue = LOWER_BOUND;
                }

                vector<float> p;
                p = get_messages<typename pidController_defs::tuneP>(mbs);
                
                if (p.size())
                {
                    state.Kp = p[0]*float(P_FULL);
                }

                vector<float> d;
                d = get_messages<typename pidController_defs::tuneD>(mbs);
                
                if (d.size())
                {
                    state.Kd = d[0]*float(D_FULL);
                }                     
                
                sigma = sigma - e;
            }
            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              internal_transition();
              external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;

              get_messages<typename defs::out>(bags).push_back(state.correctionValue);
                
              return bags;
            }

            // time_advance function
            TIME time_advance() const {
                return sigma;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename PidController<TIME>::state_type& i) {
              os << "Output: " << (i.correctionValue); 
              return os;
            }
        };     


#endif