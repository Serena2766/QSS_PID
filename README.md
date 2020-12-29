### RT-Cadmium: QSS-PID ###

See QSS_PID_RT_DEVS_project_report.pdf SECTION 3 "Model Specification" for a complete description of the model.

### REAL TIME CADMIUM INSTALL ###

In current folder (RT-Cadmium-QSS-PID), make ensure you download the MBed submodules:

> git submodule update --init --recursive

Run this to install dependencies
> chmod +x install.sh

> ./install.sh


### ATOMIC MODELS ###
Atomic models can be found in folder "atomics"

atomics/
├── pidController.hpp
├── qssReceiver.hpp
└── qssSender.hpp

### RUN SIMULATIONS ###
Go to folder "TESTS", there are 2 simulations
First simulation is for PID, second one is for QSS.

TESTS
├── pid_top_model
│   ├── SVEC.py
│   ├── inputs
│   │   ├── DESIRED_In.txt
│   │   ├── MEASURED_In.txt
│   │   ├── TUNE_D_In.txt
│   │   └── TUNE_P_In.txt
│   ├── makefile
│   ├── outputs
│   │   └── PID_Out.txt
│   ├── pid.cpp
│   └── pid_test_output.txt
└── qss_top_model
    ├── SVEC.py
    ├── inputs
    │   └── ANALOG_In.txt
    ├── makefile
    ├── outputs
    │   ├── LED1_Out.txt
    │   └── qssReceiver_Out.txt
    ├── qssSenderReceiver.cpp
    ├── qssVisualizer.py
    └── qss_test_output.txt

    ### PID SIMULATIONS ###
    > cd RT-Cadmium-QSS-PID/TESTS/pid_top_model
    > make clean; make all
    > ./PID_TOP

    This will run the standard Cadmium simulator. Cadmium logs will be generated in RT-Cadmium-QSS-PID/TESTS/pid_top_model/outputs/PID_Out.txt
    Input can be changed in the files as below:
    
        inputs/
        ├── DESIRED_In.txt
        ├── MEASURED_In.txt
        ├── TUNE_D_In.txt
        └── TUNE_P_In.txt
    
    At this moment, there is no visualization tool applicable to this simulation.


    ### QSS SIMULATIONS ###
    > cd RT-Cadmium-QSS-PID/TESTS/qss_top_model
    > make clean;make all
    
    Optional step: modify qssVisualizer.py function inputGenerator to generate desired sample points into ANALOG_In.txt file.
                   run "python(or python2) qssVisualizer.py" and the input file will be auto-filled with desired signal.
    
    > ./QSS_TOP
    
    check output files for simulation results
    
        outputs/
        ├── LED1_Out.txt
        └── qssReceiver_Out.txt   
    
    LED1_Out.txt record events generation.
    qssReceiver_Out.txt is the QSS reconstructed signal.
    
    Optional step: run qssVisualizer.py to visualize and compare input/output signal.
    
        ### QSS ORDER SELECTION (QSS1 or QSS2)
        Curently the choice of QSS1 or QSS2 is hard-coded insdie file atomics/qssSender.hpp.
        Locate function call inside internal transition: QSS(state.currentY,QSS2_MODE,THRESHOLD_QSS2), modify accordingly.
        

### RUN MODELS ON TARGET PLATFORM ###

If your target platform *is not* the Nucleo-STM32F401, you will need to change the COMPILE_TARGET / FLASH_TARGET in the make file.

> cd RT-Cadmium-QSS-PID/qss_top_model

> make clean; make embedded; 
> make flash; (not necessary, you can simply locate the bin file in BUILD directory and drag it to the target board folder)

For hardware setup, refer to QSS_PID_RT_DEVS_project_report.pdf section 5 On-Target Testing of QSS Quantizer.

