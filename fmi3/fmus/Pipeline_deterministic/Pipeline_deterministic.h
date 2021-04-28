/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef Pipeline_deterministic_h
#define Pipeline_deterministic_h

#include <random>

#include "InstanceBase.h"
#include "DeterministicEventQueue.h"

class Pipeline_deterministic : public InstanceBase {

public:

    Pipeline_deterministic(
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
        fmi3CallbackLogMessage logMessage,
        fmi3CallbackIntermediateUpdate intermediateUpdate
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
        fmi3Boolean stepEvent,
        fmi3Boolean stateEvent,
        const fmi3Int32 rootsFound[],
        size_t nEventIndicators,
        fmi3Boolean timeEvent
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
        fmi3Clock values[],
        size_t nValues
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
        const fmi3Clock values[],
        size_t nValues
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

	// This function adds new events to the event queue.
	bool addNewEvent( 
        const DeterministicEventQueue::TimeStamp& msgReceiveTime,
        const DeterministicEventQueue::MessageID& msgId,
        const DeterministicEventQueue::Receiver& receiver,
        const DeterministicEventQueue::ReceiverClock& clock
    );

    fmi3Float64 calculateDelay();

    void deactivateAllClocks();

    // Input variable "in" (value reference 1001).
    fmi3Int32 in_;
    static const fmi3ValueReference vrIn_ = 1001;

    // Input clock "in_clock" (value reference 1002).
    fmi3Clock inClock_;
    static const fmi3ValueReference vrInClock_ = 1002;

    // Output variable "out" (value reference 2001).
    fmi3Int32 out_;
    static const fmi3ValueReference vrOut_ = 2001;

    // Output clock "out_clock" (value reference 2002).
    fmi3Clock outClock_;
    static const fmi3ValueReference vrOutClock_ = 2002;

	// Random number generator seed (parameter, value reference 3001).
	fmi3Int32 randomSeed_;
    static const fmi3ValueReference vrRandomSeed_ = 3001;

    // Random number distribution mean (parameter, value reference 3002).
    fmi3Float64 randomMean_;
    static const fmi3ValueReference vrRandomMean_ = 3002;

    // Random number distribution standard deviation (parameter, value reference 3003).
    fmi3Float64 randomStdDev_;
    static const fmi3ValueReference vrRandomStdDev_ = 3003;

    // Random number distribution minimum value (parameter, value reference 3004).
    fmi3Float64 randomMin_;
    static const fmi3ValueReference vrRandomMin_ = 3004;

    // Current internal synchronization point.
    fmi3Float64 syncTime_;

	// Time of the next scheduled event.
	fmi3Float64 nextEventTime_;

	// Precision for detecting events.
	fmi3Float64 tolerance_;

	// Event queue.
	DeterministicEventQueue::EventQueue eventQueue_;
	DeterministicEventQueue::EventQueue::iterator currentEvent_;

    // Random generator (Gaussian);
    std::default_random_engine generator_;
    std::normal_distribution<fmi3Float64> distribution_;

};

#endif // Pipeline_deterministic_h
