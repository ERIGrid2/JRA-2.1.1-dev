/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#include <iostream>
#include <cmath>

#include "fmi3Functions.h"

static void cb_logMessage(fmi3InstanceEnvironment instanceEnvironment, fmi3String instanceName, fmi3Status status, fmi3String category, fmi3String message) {
    std::cout << instanceName << " [" << category << "] STATUS = " << status << ", " << message << std::endl;
}

int main( int argc, char* argv[] ) 
{
    fmi3Instance m = fmi3InstantiateCoSimulation(
        "instance1",             // instance name
        INSTANTIATION_TOKEN,     // instantiation token (from XML)
        "file:///",              // resource location (extracted FMU)
        fmi3False,               // visible
        fmi3True,                // debug logging disabled
        fmi3True,                // event mode used
        fmi3True,                // early return allowed
        NULL,                    // required intermediate variables
        0,                       // number of required intermediate variables
        NULL,                    // instance environment
        cb_logMessage,           // logger callback
        NULL                     // intermediate update callback
    );

    if ( m == NULL ) return EXIT_FAILURE;

    fmi3Status status;

    fmi3Float64 tolerance = 1e-8;

    fmi3Float64 startTime = 0.;
    fmi3Float64 stopTime = 4.;

    // Communication point.
    fmi3Float64 time = startTime;

    // Communication step size.
    fmi3Float64 stepSize = 1.;

    status = fmi3EnterInitializationMode(
        m,
        fmi3True,  // tolerance defined
        tolerance, // tolerance
        startTime, // start time
        fmi3True,  // stop time defined
        stopTime   // stop time
    );

    if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

    // Arrays for passing the random number generator seed.
    fmi3ValueReference valRefSeed[] = { 3001 };
    fmi3Int32 valSeed[] = { 12345 };

    status = fmi3SetInt32(
        m,
        valRefSeed,
        sizeof( valRefSeed ) / sizeof( fmi3ValueReference ),
        valSeed,
        sizeof( valSeed ) / sizeof( fmi3Int32 )
    );

    if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

    status = fmi3ExitInitializationMode( m );

    if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

    // Message ID.
    fmi3Int32 msgId = 0;

    // Arrays for passing value references.
    fmi3ValueReference valRefIn[] = { 1001 };
    fmi3ValueReference valRefInClock[] = { 1002 };
    fmi3ValueReference valRefOut[] = { 2001 };
    fmi3ValueReference valRefOutClock[] = { 2002 };

    // Arrays for getting / setting values.
    fmi3Int32 valIn[] = { 0 };
    fmi3Clock valInClock[] = { fmi3ClockActive };
    fmi3Int32 valOut[] = { 0 };
    fmi3Clock valOutClock[] = { fmi3False };

    fmi3Float64 sendStepSize = 1.;
    fmi3Float64 nextSendTime = startTime + sendStepSize;
    fmi3Float64 nextEventTime = stopTime;

    // Variables for fmi3DoStep.
    fmi3Boolean eventEncountered = fmi3False;
    fmi3Boolean terminateSimulation = fmi3False;
    fmi3Boolean earlyReturn = fmi3False;
    fmi3Float64 lastSuccessfulTime = startTime;

    // Variables for fmi3UpdateDiscreteStates.
    fmi3Boolean discreteStatesNeedUpdate = fmi3False;
    fmi3Boolean nominalsOfContinuousStatesChanged = fmi3False;
    fmi3Boolean valuesOfContinuousStatesChanged = fmi3False;
    fmi3Boolean nextEventTimeDefined = fmi3False;


    while ( time <= stopTime ) 
    {
        std::cout << "================================================" << std::endl;

        // Calculate step size for next simulation step.
        stepSize = std::min( nextEventTime - time, nextSendTime - time );

        if ( time + stepSize >= stopTime ) break;

        // Advance internal time of FMU.
        fmi3DoStep(
            m,
            time,
            stepSize,
            fmi3True,
            &eventEncountered,
            &terminateSimulation,
            &earlyReturn,
            &lastSuccessfulTime
        );

        if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

        // Advance simulation time.
        time = earlyReturn ? lastSuccessfulTime : time + stepSize;

        std::cout << "simulation time = " << time << std::endl;
        std::cout << "next event time = " << nextEventTime << std::endl;
        std::cout << "next send time = " << nextSendTime << std::endl;

        // External event: send messages at regular time intervals.
        if ( fmi3False == eventEncountered ) 
        {
            // Increment message ID.
            ++msgId;
            
            std::cout << "At time " << time << ": SEND message with ID = " << msgId << std::endl;

            status = fmi3EnterEventMode(
               m,
               fmi3False, // step event --> ME only
               fmi3False, // state event --> ME only
               NULL,      // roots found --> ME only
               0,         // number of event indicators --> ME only
               fmi3False  // signals that this event is an external event scheduled by the importer
            );

            if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

            // Set message ID as new input.
            valIn[0] = msgId;

            status = fmi3SetInt32(
                m,
                valRefIn,
                sizeof( valRefIn ) / sizeof( fmi3ValueReference ),
                valIn,
                sizeof( valIn ) / sizeof( fmi3Int32 )
            );

            if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

            valInClock[0] = fmi3ClockActive;

            status = fmi3SetClock(
                m,
                valRefInClock,
                sizeof( valRefInClock ) / sizeof( fmi3ValueReference ),
                valInClock,
                sizeof( valInClock ) / sizeof( fmi3Clock )
            );

            if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

            nextSendTime += sendStepSize;

        }   
        else // Internal event: message has arrived at output.
        {
            std::cout << "At time " << time << ": RECEIVE message" << std::endl;

            status = fmi3EnterEventMode(
               m,
               fmi3False, // step event --> ME only
               fmi3False, // state event --> ME only
               NULL,      // roots found --> ME only
               0,         // number of event indicators --> ME only
               fmi3True   // signals that this event is an internal event which has been previously scheduled by the FMU
            );

            if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

            status = fmi3GetClock(
                m,
                valRefOutClock,
                sizeof( valRefOutClock ) / sizeof( fmi3ValueReference ),
                valOutClock,
                sizeof( valOutClock ) / sizeof( fmi3Clock )
            );

            if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

            if ( valOutClock[0] == fmi3ClockActive ) {

                status = fmi3GetInt32(
                    m,
                    valRefOut,
                    sizeof( valRefOut ) / sizeof( fmi3ValueReference ),
                    valOut,
                    sizeof( valOut ) / sizeof( fmi3Int32 )
                );

                if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

                std::cout << "Received message with ID = " << valOut[0] << std::endl;
            } else {
                std::cout << "ERROR: Something went wrong, should have received a message!!!" << std::endl;
                break;
            }
        }

        // Finish event handling.
        status = fmi3UpdateDiscreteStates(
            m,
            &discreteStatesNeedUpdate,
            &terminateSimulation,
            &nominalsOfContinuousStatesChanged,
            &valuesOfContinuousStatesChanged,
            &nextEventTimeDefined,
            &nextEventTime
        );

        if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;

        if ( fmi3True == discreteStatesNeedUpdate ) {
            std::cout << "ERROR: Something went wrong, should not need more than 1 iteration!!!" << std::endl;
            break;
        }

        if ( fmi3True == terminateSimulation ) {
            std::cout << "ERROR: Something went wrong, simulation termination requested!!!" << std::endl;
            break;
        }

        if ( fmi3True == nominalsOfContinuousStatesChanged ) {
            std::cout << "ERROR: Something went wrong, 'nominalsOfContinuousStatesChanged' only supported in ME!!!" << std::endl;
            break;
        }

        if ( fmi3True == valuesOfContinuousStatesChanged ) {
            std::cout << "ERROR: Something went wrong, 'valuesOfContinuousStatesChanged' only supported in ME!!!" << std::endl;
            break;
        }

        if ( fmi3True == nextEventTimeDefined ) {
            std::cout << "NEXT event time = " << nextEventTime << std::endl;
        }

        // Go back to step mode.
        fmi3EnterStepMode( m );

        if ( status != fmi3OK ) std::cout << "WARNING: status = " << status << std::endl;
    }

    // Done.
    fmi3Terminate( m );
    fmi3FreeInstance( m );

    return m ? EXIT_SUCCESS : EXIT_FAILURE;
}