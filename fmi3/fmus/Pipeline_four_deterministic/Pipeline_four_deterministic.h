/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef Pipeline_four_deterministic_h
#define Pipeline_four_deterministic_h

//#include <string>
#include "InstanceBase.h"
#include "MessagePipeline.h"
#include "PipeInterconnector.h"

class Pipeline_four_deterministic : public InstanceBase {

public:

    fmi3Boolean eventHappenedInternal1;
    fmi3Boolean eventHappenedInternal2;
    fmi3Boolean eventHappenedInternal3;
    fmi3Boolean eventHappenedInternal4;

    Pipeline_four_deterministic(
        fmi3String instanceName,
        fmi3String instantiationToken,
        fmi3String resourceLocation,
        fmi3Boolean visible,
        fmi3Boolean loggingOn,
        fmi3Boolean eventModeUsed,
        fmi3Boolean earlyReturnAllowed,
        const fmi3ValueReference requiredIntermediateVariables[],
        size_t nRequiredIntermediateVariables,
        fmi3InstanceEnvironment instanceEnvironment,
        fmi3LogMessageCallback logMessage,
        fmi3IntermediateUpdateCallback intermediateUpdate
    );

    virtual fmi3Status enterInitializationMode(
        fmi3Boolean toleranceDefined,
        fmi3Float64 tolerance,
        fmi3Float64 startTime,
        fmi3Boolean stopTimeDefined,
        fmi3Float64 stopTime
    );

    virtual fmi3Status exitInitializationMode();

    virtual fmi3Status enterEventMode(
        /*fmi3Boolean stepEvent,
        fmi3Boolean stateEvent,
        const fmi3Int32 rootsFound[],
        size_t nEventIndicators,
        fmi3Boolean timeEvent*/
    );

    virtual fmi3Status reset();

    virtual fmi3Status getInt32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Int32 values[],
        size_t nValues
    );

    virtual fmi3Status getClock(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Clock values[]
    );

    virtual fmi3Status setFloat64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float64 values[],
        size_t nValues
    );

    virtual fmi3Status setInt32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Int32 values[],
        size_t nValues
    );

    virtual fmi3Status setClock(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Clock values[]
    );

    virtual fmi3Status updateDiscreteStates(
        fmi3Boolean *discreteStatesNeedUpdate,
        fmi3Boolean *terminateSimulation,
        fmi3Boolean *nominalsOfContinuousStatesChanged,
        fmi3Boolean *valuesOfContinuousStatesChanged,
        fmi3Boolean *nextEventTimeDefined,
        fmi3Float64 *nextEventTime
    );

    virtual fmi3Status doStep(
        fmi3Float64 currentCommunicationPoint,
        fmi3Float64 communicationStepSize,
        fmi3Boolean noSetFMUStatePriorToCurrentPoint,
        fmi3Boolean* eventEncountered,
        fmi3Boolean* terminateSimulation,
        fmi3Boolean* earlyReturn,
        fmi3Float64* lastSuccessfulTime
    );

private:
    MessagePipeline pipe1;
    MessagePipeline pipe2;
    MessagePipeline pipe3;
    MessagePipeline pipe4;
    PipeInterconnector ic12;
    PipeInterconnector ic23;
    PipeInterconnector ic34;

    void deactivateAllClocks();

    fmi3Float64 tolerance_; //precision for detecting events

    // Input variable "in1" (value reference 1001).
    fmi3Int32 in1_;
    static const fmi3ValueReference vrIn1_ = 1001;

    // Input clock "in_clock1" (value reference 1002).
    fmi3Clock inClock1_;
    static const fmi3ValueReference vrInClock1_ = 1002;

    // Input variable "in2" (value reference 1003).
    fmi3Int32 in2_;
    static const fmi3ValueReference vrIn2_ = 1003;

    // Input clock "in_clock2" (value reference 1004).
    fmi3Clock inClock2_;
    static const fmi3ValueReference vrInClock2_ = 1004;

    // Output variable "out1" (value reference 2001).
    fmi3Int32 out1_;
    static const fmi3ValueReference vrOut1_ = 2001;

    // Output clock "out1_clock" (value reference 2002).
    fmi3Clock outClock1_;
    static const fmi3ValueReference vrOutClock1_ = 2002;

    // Output variable "out2" (value reference 2003).
    fmi3Int32 out2_;
    static const fmi3ValueReference vrOut2_ = 2003;

    // Output clock "out_clock2" (value reference 2004).
    fmi3Clock outClock2_;
    static const fmi3ValueReference vrOutClock2_ = 2004;

    // Current internal synchronization point.
    fmi3Float64 syncTime_;

    	// Random number generator seed (parameter, value reference 3001).
    static const fmi3ValueReference vrRandomSeed_ = 3001;

    // Random number distribution mean (parameter, value reference 3002).
    static const fmi3ValueReference vrRandomMean_ = 3002;

    // Random number distribution standard deviation (parameter, value reference 3003).
    static const fmi3ValueReference vrRandomStdDev_ = 3003;

    // Random number distribution minimum value (parameter, value reference 3004).
    static const fmi3ValueReference vrRandomMin_ = 3004;


};

#endif // Pipeline_four_deterministic_h
