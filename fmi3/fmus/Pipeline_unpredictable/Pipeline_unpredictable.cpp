/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#include "Pipeline_unpredictable.h"

#include <limits>
#include <algorithm>
#include <stdexcept>

#define INSTANTIATION_TOKEN "{58210e20-a83b-11eb-82ba-00155d0450ce}"

using namespace UnpredictableEventStack;

Pipeline_unpredictable::Pipeline_unpredictable(
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
) :
    InstanceBase(
        instanceName,
        instantiationToken,
        resourceLocation,
        visible,
        loggingOn,
        eventModeUsed,
        earlyReturnAllowed,
        requiredIntermediateVariables,
        nRequiredIntermediateVariables,
        instanceEnvironment,
        logMessage,
        intermediateUpdate
    ),
    in_( 0 ),
    out_( 0 ),
    randomSeed_( 1 ),
    tolerance_( 1e-9 )
{
    if ( fmi3False == this->getEventModeUsed() )
    {
        throw std::runtime_error( "Importer must support event mode." );
    }

    if ( fmi3False == this->getEarlyReturnAllowed() )
    {
        throw std::runtime_error( "Importer must support early return." );
    }

    if ( this->getInstantiationToken() != std::string( INSTANTIATION_TOKEN ) )
    {
        throw std::runtime_error( "Wrong GUID (instantiation token)." );
    }

    this->logDebug(
        "successfully initialized class %s", "Pipeline_unpredictable"
    );
}

fmi3Status
Pipeline_unpredictable::enterInitializationMode(
    fmi3Boolean toleranceDefined,
    fmi3Float64 tolerance,
    fmi3Float64 startTime,
    fmi3Boolean stopTimeDefined,
    fmi3Float64 stopTime
) {
    this->setMode( initializationMode );

    // Set internal time to simulation start time.
    this->syncTime_ = startTime;

    // Adjust tolerances for determining if two timestamps are the same.
    if ( fmi3True == toleranceDefined )
    {
        this->tolerance_ = tolerance;
    }

    return fmi3OK;
}

fmi3Status
Pipeline_unpredictable::exitInitializationMode()
{
    this->setMode( stepMode );

	// Random generator seed has to be a positive non-zero integer.
	if ( 1 > this->randomSeed_ )
    {
        this->randomSeed_ = 1;
    }

    // Set random generator seed.
    this->generator_.seed( this->randomSeed_ );

    // Initialize random number distribution.
    this->distribution_ = std::uniform_real_distribution<fmi3Float64>( 0., 1. );

    return fmi3OK;
}

fmi3Status
Pipeline_unpredictable::enterEventMode(
    fmi3Boolean stepEvent,
    fmi3Boolean stateEvent,
    const fmi3Int32 rootsFound[],
    size_t nEventIndicators,
    fmi3Boolean timeEvent
) {
    this->setMode( eventMode );

    // This is a time event that was previously signaled by function doStep.
    // This means that a new message is available to be received by the importer.
    if ( fmi3True == timeEvent )
    {
        if ( true == this->applyCurrentEvent() )
        {
            this->removeCurrentEvent();
        }
        else
        {
            return fmi3Error;
        }

    }

    return fmi3OK;
}

fmi3Status
Pipeline_unpredictable::reset()
{
    while ( false == this->eventStack_.empty() )
    {
        delete this->eventStack_.front();
        this->eventStack_.pop();
    }

    return fmi3OK;
}

fmi3Status
Pipeline_unpredictable::getInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int32 values[],
    size_t nValues
) {
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        return fmi3Error;
    }

    fmi3Status status = fmi3OK;
    const fmi3ValueReference* vr;
    fmi3Int32* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValues, v != values + nValues;
        ++vr, ++v
    ) {
        switch ( *vr ) {
            case this->vrOut_:
                *v = this->out_;
                this->logDebug( "%d => get %d", *vr, this->out_ );
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }
    }

    return status;
}

fmi3Status
Pipeline_unpredictable::getClock(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock values[],
    size_t nValues
) {
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        return fmi3Error;
    }

    fmi3Status status = fmi3OK;
    const fmi3ValueReference* vr;
    fmi3Clock* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValues, v != values + nValues;
        ++vr, ++v
    ) {
        switch ( *vr ) {
            case this->vrOutClock_:
                *v = this->outClock_;
                this->logDebug( "%d => get clock %d", *vr, this->outClock_ );
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }
    }

    return status;
}

fmi3Status
Pipeline_unpredictable::setInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int32 values[],
    size_t nValues
) {
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        return fmi3Error;
    }

    fmi3Status status = fmi3OK;
    const fmi3ValueReference* vr;
    const fmi3Int32* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValues, v != values + nValues;
        ++vr, ++v
    ) {
        switch ( *vr ) {
            case this->vrIn_:
                this->in_ = *v;
                break;
            case this->vrRandomSeed_:
                this->randomSeed_ = *v;
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }

        this->logDebug( "Value reference %d => set to: %d (fmi3Int32)", *vr, *v );
    }

    return status;
}

fmi3Status
Pipeline_unpredictable::setClock(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Clock values[],
    size_t nValues
) {
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        return fmi3Error;
    }

    fmi3Status status = fmi3OK;
    const fmi3ValueReference* vr;
    const fmi3Clock* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValues, v != values + nValues;
        ++vr, ++v
    ) {
        switch ( *vr ) {
            case this->vrInClock_:
                if ( fmi3ClockInactive == *v )
                {
                    this->logError(
                        "clocks may not be deactivated by the importer"
                    );
                    return fmi3Error;
                }
                this->inClock_ = *v;
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }

        this->logDebug(
            "%d => set clock %d", *vr, *v
        );
    }

    return status;
}

fmi3Status
Pipeline_unpredictable::updateDiscreteStates(
    fmi3Boolean *discreteStatesNeedUpdate,
    fmi3Boolean *terminateSimulation,
    fmi3Boolean *nominalsOfContinuousStatesChanged,
    fmi3Boolean *valuesOfContinuousStatesChanged,
    fmi3Boolean *nextEventTimeDefined,
    fmi3Float64 *nextEventTime
) {
    // Input clock is active --> add message as output using the calculated delay.
    if ( fmi3ClockActive == this->inClock_ ) 
    {
        this->addNewEvent(
            this->in_,
            &this->out_,
            &this->outClock_
        );
    }

    *nextEventTimeDefined = fmi3False;
    *discreteStatesNeedUpdate = fmi3False;
    *terminateSimulation = fmi3False;
    *nominalsOfContinuousStatesChanged = fmi3False;
    *valuesOfContinuousStatesChanged = fmi3False;
    *nextEventTime = std::numeric_limits<fmi3Float64>::max();

    // We have finished processing internal events --> deactivate all active clocks.
    this->deactivateAllClocks();

    return fmi3OK;
}

fmi3Status
Pipeline_unpredictable::doStep(
    fmi3Float64 currentCommunicationPoint,
    fmi3Float64 communicationStepSize,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* eventEncountered,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* earlyReturn,
    fmi3Float64* lastSuccessfulTime
) {
    // Sanity check: Do the importer's current communication point and the internal
    // synchronization time coincide?
    if ( fabs( this->syncTime_ - currentCommunicationPoint ) > this->tolerance_ )
    {
        this->logError(
            "Current communication point (%f) does not coincide with the internal time (%f)",
            currentCommunicationPoint, this->syncTime_
        );

        return fmi3Discard;
    }

    // Update internal synchronization time to new requested communication point.
    this->syncTime_ = currentCommunicationPoint + communicationStepSize;
    this->logDebug(
        "Attempt to step from %f to %f",
        currentCommunicationPoint,
        this->syncTime_
    );

    // We have at least one event that can be applied.
    if ( false == this->eventStack_.empty() ) 
    {
        // Emulate an unpredictable event by signalling an internal event during doStep.
        fmi3Float64 fraction = this->distribution_( this->generator_ );
        fmi3Float64 nextEventTime = currentCommunicationPoint + fraction * communicationStepSize;

        this->logDebug(
            "An internal event occured at time = %f",
            nextEventTime
        );

        this->syncTime_ = nextEventTime;

        *eventEncountered = fmi3True;
        *earlyReturn = fmi3True;
        *lastSuccessfulTime = nextEventTime;
    } 
    else 
    {
        *eventEncountered = fmi3False;
        *earlyReturn = fmi3False;
        *lastSuccessfulTime = this->syncTime_;
    }

    *terminateSimulation = fmi3False;

    return fmi3OK;
}

void
Pipeline_unpredictable::addNewEvent(
    const MessageID& msgId,
    const Receiver& receiver,
    const ReceiverClock& clock
) {
    // Create new event.
    Event* evt = new Event(
        msgId,
        receiver,
        clock
    );

    this->eventStack_.push( evt );

    this->logDebug(
        "add new event with id = %d", msgId
    );
}

bool
Pipeline_unpredictable::applyCurrentEvent()
{
    if ( true == this->eventStack_.empty() ) return false;

    Event* evt = this->eventStack_.front();
    *( evt->receiver ) = evt->msgId;
    *( evt->clock ) = fmi3ClockActive;

    return true;
}

bool
Pipeline_unpredictable::removeCurrentEvent()
{
    if ( true == this->eventStack_.empty() ) return false;

    delete this->eventStack_.front();
    this->eventStack_.pop();

    return true;
}

void
Pipeline_unpredictable::deactivateAllClocks()
{
    inClock_ = fmi3ClockInactive;
    outClock_ = fmi3ClockInactive;
}
