/**
* Mengyao Wu && Xinrui Zhang
* Carleton University
*
* Top model for qssIntegrator
*/



#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupling.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include <NDTime.hpp>
#include <cadmium/io/iestream.hpp>


#include <cadmium/real_time/arm_mbed/io/digitalOutput.hpp>
#include <cadmium/real_time/arm_mbed/io/analogInput.hpp>
#include <cadmium/real_time/arm_mbed/io/pwmOutput.hpp>



#ifdef RT_ARM_MBED
  #include "../mbed.h"
  Serial pc(SERIAL_TX,SERIAL_RX);
#else
  // When simulating the model it will use these files as IO in place of the pins specified.
  const char* A0 = "./inputs/EVENT_In.txt"; //use A0 analog for U event in, should be 
  const char* D3    = "./outputs/qssIntegrator_Out.txt";
#endif


#include "../../atomics/qssIntegrator.hpp"

using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;


int main(int argc, char ** argv) {

  #ifdef RT_ARM_MBED
      //Logging is done over cout in RT_ARM_MBED
      struct oss_sink_provider{
        static std::ostream& sink(){
          return cout;
        }
      };
  #else
    // all simulation timing and I/O streams are ommited when running embedded
    auto start = hclock::now(); //to measure simulation execution time

    static std::ofstream out_data("qssIntegrator_test_output.txt");
    struct oss_sink_provider{
      static std::ostream& sink(){
        return out_data;
      }
    };
  #endif

  /*************** Loggers *******************/
  using info=cadmium::logger::logger<cadmium::logger::logger_info, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using debug=cadmium::logger::logger<cadmium::logger::logger_debug, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using state=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using log_all=cadmium::logger::multilogger<info, debug, state, log_messages, routing, global_time, local_time>;

  #ifdef RT_ARM_MBED
    using logger_top=cadmium::logger::multilogger<global_time>;
  #else
    using logger_top=cadmium::logger::multilogger<log_messages,global_time>;
  #endif

  /*******************************************/

  using AtomicModelPtr=std::shared_ptr<cadmium::dynamic::modeling::model>;
  using CoupledModelPtr=std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>>;

  /********************************************/
  /***************** qssIntegrator *******************/
  /********************************************/
  #define QSS_ORDER 1
  AtomicModelPtr qssIntegrator1 = cadmium::dynamic::translate::make_dynamic_atomic_model<QssIntegrator, TIME>("qssIntegrator1",QSS_ORDER);


  /********************************************/
  /********** EVENT in *******************/
  /********************************************/
  AtomicModelPtr IntegratorIn1 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("IntegratorIn1", A0);

  /********************************************/
  /********* IntegratorOut *******************/
  /********************************************/
  AtomicModelPtr IntegratorOut1 = cadmium::dynamic::translate::make_dynamic_atomic_model<PwmOutput, TIME>("IntegratorOut1", D3);


  /************************/
  /*******TOP MODEL********/
  /************************/
  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  cadmium::dynamic::modeling::Models submodels_TOP =  {qssIntegrator1, IntegratorIn1, IntegratorOut1};
  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {};
  cadmium::dynamic::modeling::ICs ics_TOP = {
    cadmium::dynamic::translate::make_IC<analogInput_defs::out, qssIntegrator_defs::eventU>("IntegratorIn1", "qssIntegrator1"),
    cadmium::dynamic::translate::make_IC<qssIntegrator_defs::out, pwmOutput_defs::in>("qssIntegrator1", "IntegratorOut1"),
  };
  CoupledModelPtr TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
    "TOP",
    submodels_TOP,
    iports_TOP,
    oports_TOP,
    eics_TOP,
    eocs_TOP,
    ics_TOP
  );

  ///****************////
  cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
  #ifdef RT_ARM_MBED
    r.run_until(NDTime("00:10:00:000"));
  #else
    r.run_until(NDTime("00:01:00:000"));
    return 0;
  #endif
}
