#!/usr/bin/python3

from FMI3Wrapper import *
from FMI3ModelDescription import *
from extractFMU import *
from tempfile import *
import os.path
from pathlib import Path
import math, sys
import ctypes as c

@c.CFUNCTYPE(None,c_void_p,c_char_p,c_int,c_char_p,c_char_p)
def cb_logMessage(instance_environment, instance_name, status, category, message):
    print( '{} [{}] STATUS = {}, {}'.format(
        instance_name.decode( 'utf-8' ),
        category.decode( 'utf-8' ),
        status,
        message.decode( 'utf-8' )
        ) , flush=True)

def testFMU(model_path, model_name, instance_name, visible, logging_on, event_mode, early_return):
    with TemporaryDirectory() as tmpdir:
        extractFMU(
            os.path.join( model_path, model_name + '.fmu' ),
            tmpdir)
    
        model = FMI3ModelDescription()
        model.parse_model_description(model_name, tmpdir)
        
        assert (model_name == model.model_name)
        
        fmu = FMI3Wrapper(model, tmpdir)

    # Instantiate FMU.
    fmu.fmi3InstantiateCoSimulation (
        instance_name,             # instance name
        model.instantiation_token,     # instantiation token (from XML)
        "file:///",              # resource location (extracted FMU)
        visible,                 # visible
        logging_on,              # debug logging
        event_mode,              # event mode
        early_return,            # early return allowed
        None,                    # required intermediate variables
        None,                    # instance environment
        cb_logMessage,           # logger callback
        None                     # intermediate update callback
        )
    val_in = [ 0 ]
    val_in_clock = [ fmu.fmi3_clock_active ]
    val_out = [ 0 ]
    val_out_clock = [ fmu.fmi3_false ];

    tolerance = 1e-8
    start_time = 0
    stop_time = 4
    #communication point
    time = start_time
    #communication step size
    step_size = 1

    # Initialize FMU.
    fmu.fmi3EnterInitializationMode(tolerance,start_time,stop_time,True,True)
    fmu.fmi3SetInt32(['randomSeed'],[12345])
    fmu.fmi3ExitInitializationMode()

    msg_id = 0

    last_successful_time = start_time;
    send_step_size = 1
    next_send_time = start_time + send_step_size
    next_event_time = stop_time

    while ( time < stop_time ):

        print( '================================================', flush=True )

        # Calculate step size for next simulation step.
        step_size = min( next_event_time - time, next_send_time - time )
    
        if ( time + step_size >= stop_time ): break

        # Advance internal time of FMU.
        step_result=fmu.fmi3DoStep(time,step_size,True,last_successful_time)
        last_successful_time=step_result.last_successful_time
    
        #Advance simulation time.
        time = last_successful_time if step_result.early_return else time + step_size

        print("simulation time = "+str(time), flush=True)
        print("next event time = "+str(next_event_time), flush=True)
        print("next send time =  "+str(next_send_time), flush=True)
    
        #External event: send messages at regular time intervals.
        if not step_result.event_encountered:
            msg_id+=1
            print("At time "+str(time)+": SEND message with ID = "+str(msg_id), flush=True)
            status = fmu.fmi3EnterEventMode(
                False, # step event --> ME only
                False, # state event --> ME only
                [],  # roots found --> ME only
                False  # signals that this event is an external event scheduled by the importer
                )

            val_in[0] = msg_id
            status = fmu.fmi3SetInt32(['in'],val_in)
            val_in_clock[0] = fmu.fmi3_clock_active
            status = fmu.fmi3SetClock(['inClock'],val_in_clock)

            next_send_time += send_step_size
        
        # Internal event: message has arrived at output.
        else:
            status = fmu.fmi3EnterEventMode(
                False, # step event --> ME only
                False, # state event --> ME only
                [],  # roots found --> ME only
                True  # signals that this event is an internal event which has been previously scheduled by the FMU
                )
        
            val_out_clock = fmu.fmi3GetClock(['outClock'])
        
            if val_out_clock[0] == fmu.fmi3_clock_active:
                val_out = fmu.fmi3GetInt32(['out'])
                print("At time "+str(time)+": RECEIVE message with ID = "+str(val_out[0]), flush=True)
            else:
                print("At time "+str(time)+": Event but did not receive message.", flush=True)
                #print("ERROR: Something went wrong, should have received a message!!!")
                #break;
    
        # Finish event handling.
        discrete_update_result = fmu.fmi3UpdateDiscreteStates(next_event_time)
        next_event_time=discrete_update_result.next_event_time
        next_event_time_defined=discrete_update_result.next_event_time_defined
    
        if discrete_update_result.discrete_states_need_update:
            print("ERROR: Something went wrong, should not need more than 1 iteration!!!", flush=True)
            break

        if discrete_update_result.terminate_simulation:
            print("ERROR: Something went wrong, simulation termination requested!!!", flush=True)
            break

        if discrete_update_result.nominals_of_continuous_states_changed:
            print("ERROR: Something went wrong, 'nominalsOfContinuousStatesChanged' only supported in ME!!!", flush=True)
            break

        if discrete_update_result.values_of_continuous_states_changed:
            print("ERROR: Something went wrong, 'valuesOfContinuousStatesChanged' only supported in ME!!!", flush=True)
            break

        if discrete_update_result.next_event_time_defined:
            print("NEXT event time = "+str(next_event_time), flush=True)
    
        # Go back to step mode.
        fmu.fmi3EnterStepMode()

    # Done.
    fmu.fmi3Terminate()
    fmu.fmi3FreeInstance()
