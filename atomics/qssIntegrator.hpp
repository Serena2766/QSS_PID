/**
* Mengyao Wu && Xinrui Zhang
* Carleton University
*
*/

#ifndef BOOST_SIMULATION_PDEVS_QSS_INTEGRATOR_HPP
#define BOOST_SIMULATION_PDEVS_QSS_INTEGRATOR_HPP



#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>



using namespace cadmium;
using namespace std;


#define FLOAT_MIN 0.01
//Port definition
    struct qssIntegrator_defs {
        struct out : public out_port<float> { };
        struct eventU : public in_port<float> { };
    };

    template<typename TIME>
    class QssIntegrator {
        using defs=qssIntegrator_defs; // putting definitions in context
        public:
            TIME    sigma;
            //maximium quantum level, 0 to max
            // map to actual output range, ie: (0.0,1.0) for PWN or Analog
            const float numOfLevels = 50.0;
            //quantum is relative to numOfLevels
            //adjust numOfLevels is sufficient, keep quantum as 1
            const float quantum = 1.0; 

            int qssMode;// 1 for QSS1, 2 for QSS2
            // default constructor
            QssIntegrator(int mode = 1) noexcept{
              sigma = std::numeric_limits<TIME>::infinity();
              
              qssMode = mode;//should be passed as constructor arguement
              
              state.currentLevel = 0;
              state.integratorInput = 0;
              state.currentU = 0;
            }
            
            // state definition
            struct state_type{
              float currentLevel;
              float integratorInput;
              float currentU;
            }; 
            state_type state;
            // ports definition

            using input_ports=std::tuple<typename defs::eventU>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
                
                state.integratorInput = state.currentU - state.currentLevel;
                //float number, use extra FLOAT_MIN 
                assert(abs(state.integratorInput) < (numOfLevels + FLOAT_MIN) && \
                       "|integratorInput| should be < numOfLevels\n");
                
                //stable state, no update. FLOAT_MIN due to float number
                if (abs(state.integratorInput) < FLOAT_MIN)
                {
                    sigma = std::numeric_limits<TIME>::infinity();
                }
                else
                {
                    if (state.integratorInput < 0)
                    {
                        state.currentLevel -= quantum;
                        assert(state.currentLevel > -FLOAT_MIN && \
                               "currentLevel cannot below 0\n");
                    }
                    else
                    {
                        state.currentLevel += quantum;
                        assert(state.currentLevel < (numOfLevels + FLOAT_MIN) && \
                               "currentLevel cannot be above numOfLevels\n");
                    }
                    
                    float tempT = 0.0;
                    
                    if (qssMode == 1)
                    {
                        //solve for next t: integratorInput * t = quantum
                        //t = quantum/integratorInput, quantum = 1 relative to numOfLevels
                        // default reference unit time is 1 second
                        tempT = quantum/abs(state.integratorInput);                        
                    }
                    else if (qssMode == 2)
                    {
                        //0.5 * integratorInput * t^2 = quantum
                        // t = sqrt((2*quantum)/integratorInput)
                        tempT = sqrt((2*quantum)/abs(state.integratorInput)); 
                    }
                    else
                    {
                        assert("Order higher than 2 not implemented\n");
                    }
                    
                    std::string timeStr;
                    
                    if (tempT < 1.0)
                    {
                        int ms = (int)(1000*tempT + 0.5);
                        int sec = 0;
                        
                        if (ms == 1000)
                        {
                            sec += 1;
                            ms = 0;
                        }
                        
                        timeStr = "00:00:" + std::to_string(sec) + ":" + std::to_string(ms);
                    }
                    //the resolution of input U cannot be less than 1/10 of the quantum 1
                    // if it reaches stable state, time should be infinity set above
                    else if (tempT >= 1.0 && tempT < (10 + FLOAT_MIN))
                    {
                        
                        float intPart = 0, fracPart = 0;
                        
                        fracPart = modf(tempT, &intPart);
                        
                        int ms = (int)(1000*fracPart + 0.5);
                        int sec = (int)(intPart + 0.5);
                        
                        if (ms == 1000)
                        {
                            sec += 1;
                            ms = 0;
                        }
                        
                        timeStr = "00:00:" + std::to_string(sec) + ":" + std::to_string(ms);
                        
                    }
                    else
                    {
                        assert(("time advance out of range: %f\n",tempT));
                    }
                    
                    sigma = TIME(timeStr);
                }
                
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                
                vector<float> u;
                u = get_messages<typename qssIntegrator_defs::eventU>(mbs);
                
                if (u.size())
                {
                    state.currentU = u[0];
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

              get_messages<typename defs::out>(bags).push_back(state.currentLevel);
                
              return bags;
            }

            // time_advance function
            TIME time_advance() const {
                return sigma;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename QssIntegrator<TIME>::state_type& i) {
              os << "Output: " << (i.integratorInput); 
              return os;
            }
        };     


#endif