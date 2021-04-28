/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef Pipeline_unpredictable_h
#define Pipeline_unpredictable_h

#include <random>

#include "InstanceBase.h"
#include "UnpredictableEventStack.h"

class Pipeline_unpredictable : public InstanceBase {

public:

    Pipeline_unpredictable(
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
	void addNewEvent( 
        const UnpredictableEventStack::MessageID& msgId,
        const UnpredictableEventStack::Receiver& receiver,
        const UnpredictableEventStack::ReceiverClock& clock
    );

    bool applyCurrentEvent();

    bool removeCurrentEvent();

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

    // Current internal synchronization point.
    fmi3Float64 syncTime_;

	// Precision for detecting events.
	fmi3Float64 tolerance_;

	// Event queue.
	UnpredictableEventStack::EventStack eventStack_;

    // Random generator (Gaussian);
    std::default_random_engine generator_;
    std::uniform_real_distribution<fmi3Float64> distribution_;
};

#endif // Pipeline_unpredictable_h
