/**
* Mengyao Wu
* Carleton University
*
* Qss_sender:
* Attached to a signal source, quantize the signal, generates events to be sent to qssReceiver
*/

#ifndef BOOST_SIMULATION_PDEVS_QSS_SENDER_HPP
#define BOOST_SIMULATION_PDEVS_QSS_SENDER_HPP

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

#define THRESHOLD_QSS1 0.05
#define THRESHOLD_QSS2 0.05

#define QSS1_MODE 1
#define QSS2_MODE 2

using namespace cadmium;
using namespace std;

//Port definition
    struct qssSender_defs {
        struct eventLed : public out_port<bool> { };
        
        
        struct eventY : public out_port<float> { };
        
        struct eventK : public out_port<float> { };
        
        //struct pwmOut : public out_port<float> { };
        struct in : public in_port<float> { };
    };

    template<typename TIME>
    class QssSender {
        using defs=qssSender_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            TIME    sigma;
            bool toggle;
            bool timeToggle;
            bool firstLoop;
            // default constructor
            QssSender() noexcept{
              toggle = 0;
              timeToggle = false;
              firstLoop = true;
              sigma = std::numeric_limits<TIME>::infinity();
              state.event = false;
              state.deltaY = 0.0;
              state.deltaK = 0.0;
              state.lastEventY = 0.0;
              state.lastEventK = 0.0;
              state.lastY = 0.0;
              state.currentY = 0.0;
              state.estimatedY = 0.0;
              state.steps = 0;
            }
            
            // state definition
            struct state_type{
              bool event; 
              float deltaY;
              float deltaK;
              float lastEventY; //This is the last y value sent to PID, NOT the last y value received.
              float lastEventK;
              float lastY;
              float currentY;
              float estimatedY;
              int steps;
            }; 
            state_type state;
            // ports definition

            using input_ports=std::tuple<typename defs::in>;
            using output_ports=std::tuple<typename defs::eventLed,typename defs::eventY,typename defs::eventK>;
            // internal transition
                        
            void QSS(float y, int QSS_MODE, float Threshold)
            {   
                state.deltaY = y - state.lastY;
            
                state.estimatedY = state.lastEventY + state.steps*state.lastEventK;
                
                //pc.printf("Show Show: %f, %f, %f\n",y,state.estimatedY,state.estimatedY - y);
                
                if (abs(state.estimatedY - y) > Threshold)
                {
                    if (QSS_MODE == QSS1_MODE)
                    {
                        state.lastEventK = 0;
                    }
                    else if (QSS_MODE == QSS2_MODE)
                    {
                        state.lastEventK = state.deltaY;
                    }
                    else
                    {
                        assert("Order higher than QSS2 Not implemented yet\n");
                    }
                    
                    state.lastEventY = y;
                    state.steps = 0;
                    state.event = true;
                    toggle = !toggle;  
                }
                
                state.lastY = y;
            }
            
            void internal_transition() {
                
                state.steps ++;
                //sigma = std::numeric_limits<TIME>::infinity();
                #ifdef RT_ARM_MBED
                    pc.printf("QssSender Called:%d\n",state.steps);
                #endif

                state.event = false;
                
                QSS(state.currentY,QSS2_MODE,THRESHOLD_QSS2);
                
                
                if (firstLoop)
                {
                    sigma = TIME("00:00:00:1");
                    firstLoop = false;
                }
                else
                {
                    if (timeToggle)
                    {
                        sigma = TIME("00:00:00:1");
                    }
                    else
                    {
                        sigma = TIME("00:00:00:1");
                    }
                    timeToggle = !timeToggle;                  
                }
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
              
                vector<float> y;
                y = get_messages<typename qssSender_defs::in>(mbs);
                
                state.currentY = y[0];
                //state.event = false;
                
                if (sigma == std::numeric_limits<TIME>::infinity())
                {
                    //This is the first sampling, generate an event
                    state.event = true;
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

              if (state.event)
              {   
                  get_messages<typename defs::eventY>(bags).push_back(state.lastEventY);
                  get_messages<typename defs::eventK>(bags).push_back(state.lastEventK);
                  get_messages<typename defs::eventLed>(bags).push_back(toggle);
              }
              
              return bags;
            }

            // time_advance function
            TIME time_advance() const {
                return sigma;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename QssSender<TIME>::state_type& i) {
              os << "Output: " << (i.event ? 1 : 0); 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_QSS_SENDER_HPP