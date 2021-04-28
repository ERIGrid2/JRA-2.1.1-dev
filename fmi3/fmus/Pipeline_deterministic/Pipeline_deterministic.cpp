/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#include "Pipeline_deterministic.h"

#include <limits>
#include <algorithm>
#include <stdexcept>

#define INSTANTIATION_TOKEN "{a67992a0-a385-11eb-aea4-00155d0bce5e}"

using namespace DeterministicEventQueue;

Pipeline_deterministic::Pipeline_deterministic(
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
    randomMean_( 0.5 ),
    randomStdDev_( 0.15 ),
    randomMin_( 0.1 ),
    tolerance_( DeterministicEventQueue::tolerance ),
    nextEventTime_( std::numeric_limits<fmi3Float64>::max() ),
    eventQueue_(),
    currentEvent_( eventQueue_.end() )
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
        "successfully initialized class %s", "Pipeline_deterministic"
    );
}

fmi3Status
Pipeline_deterministic::enterInitializationMode(
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
        DeterministicEventQueue::tolerance = tolerance;
        this->tolerance_ = tolerance;
    }

    return fmi3OK;
}

fmi3Status
Pipeline_deterministic::exitInitializationMode()
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
    this->distribution_ = std::normal_distribution<fmi3Float64>(
        this->randomMean_,
        this->randomStdDev_
    );

    return fmi3OK;
}

fmi3Status 
Pipeline_deterministic::enterEventMode(
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
        *( *this->currentEvent_ )->receiver = ( *this->currentEvent_ )->msgId;
        *( *this->currentEvent_ )->clock = fmi3ClockActive;
    }

    return fmi3OK;
}

fmi3Status
Pipeline_deterministic::reset()
{
    this->eventQueue_.clear();
    this->currentEvent_ = this->eventQueue_.end();
    this->nextEventTime_ = std::numeric_limits<fmi3Float64>::max();

    return fmi3OK;
}

fmi3Status
Pipeline_deterministic::getInt32(
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
Pipeline_deterministic::getClock(
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
Pipeline_deterministic::setInt32(
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
Pipeline_deterministic::setFloat64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 values[],
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
    const fmi3Float64* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValues, v != values + nValues;
        ++vr, ++v
    ) {
        switch ( *vr ) {
            case this->vrRandomMean_:
                this->randomMean_ = *v;
                break;
            case this->vrRandomStdDev_:
                this->randomStdDev_ = *v;
                break;
            case this->vrRandomMin_:
                this->randomMin_ = *v;
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }

        this->logDebug( 
            "Value reference %d => set to: %f (fmi3Float64)", *vr, *v 
        );
    }

    return status;
}

fmi3Status
Pipeline_deterministic::setClock(
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
Pipeline_deterministic::updateDiscreteStates(
    fmi3Boolean *discreteStatesNeedUpdate,
    fmi3Boolean *terminateSimulation,
    fmi3Boolean *nominalsOfContinuousStatesChanged,
    fmi3Boolean *valuesOfContinuousStatesChanged,
    fmi3Boolean *nextEventTimeDefined,
    fmi3Float64 *nextEventTime
) {
    // Input clock is active --> add message as output using the calculated delay.
    if ( fmi3ClockActive == this->inClock_ ) {

        fmi3Float64 delay;
        
        // The event queue can only contain one event per timestamp. Since there is
        // a (very small) chance that we generate a new random event with an already
        // existing timestamp, insertion of new event is repeated until a new event
        // has been inserted successfully.
        do
        {
            delay = this->calculateDelay();
        } 
        while( false == this->addNewEvent(
            this->syncTime_ + delay,
            this->in_,
            &this->out_,
            &this->outClock_
        ) );
    }

    // Event queue is empty, next event time is undefined.
    if ( this->eventQueue_.empty() )
    {
        this->nextEventTime_ = std::numeric_limits<fmi3Float64>::max();
        *nextEventTimeDefined = fmi3False;
    }
    // We have not yet reached the next event, return the next event's time.
    else if ( this->syncTime_ < this->nextEventTime_ - this->tolerance_ )
    {
        this->nextEventTime_ = ( *this->currentEvent_ )->timeStamp;
        *nextEventTimeDefined = fmi3True;
    }
    // We are processing the current event, increment event if possible.
    else if ( fabs( this->syncTime_ - this->nextEventTime_ ) <= this->tolerance_ )
    {
        // There is a next event in the schedule --> increment current event and set
        // time of this event as next event time.
        if ( this->currentEvent_ != std::prev( this->eventQueue_.end() ) )
        {
            ++( this->currentEvent_ );

            this->nextEventTime_ = ( *this->currentEvent_ )->timeStamp;
            *nextEventTimeDefined = fmi3True;

            this->logDebug(
                "set next event time to t = %f",
                this->nextEventTime_
            );
        }
        else // There is NO next event in the schedule, next event time is undefined.
        {
            this->nextEventTime_ = std::numeric_limits<fmi3Float64>::max();
            *nextEventTimeDefined = fmi3False;

            this->logDebug(
                "no next event defined"
            );
        }
    }

    *discreteStatesNeedUpdate = fmi3False;
    *terminateSimulation = fmi3False;
    *nominalsOfContinuousStatesChanged = fmi3False;
    *valuesOfContinuousStatesChanged = fmi3False;
    *nextEventTime = this->nextEventTime_;

    // We have finished processing internal events --> deactivate all active clocks.
    this->deactivateAllClocks();

    return fmi3OK;
}

fmi3Status
Pipeline_deterministic::doStep(
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

    // The importer stepped over an event.
    if ( this->syncTime_ > this->nextEventTime_ )
    {
        this->logDebug(
            "%s %s %f",
            "The importer stepped over an event.",
            "The current internal time (lastSuccessfulTime) is: ",
            this->nextEventTime_
        );

        *eventEncountered = fmi3True;
        *earlyReturn = fmi3True;
        *lastSuccessfulTime = this->nextEventTime_;
    }
    // The importer has reached the next event.
    else if ( fabs( this->syncTime_ - this->nextEventTime_ ) <= this->tolerance_ )
    {
        this->logDebug(
            "The importer has reached the next event at the new synchronization point."
        );

        if ( this->currentEvent_ == this->eventQueue_.end() )
        {
            this->logError(
                "corrupted event queue"
            );

            *terminateSimulation = fmi3True;
            return fmi3Fatal;
        }

        *eventEncountered = fmi3True;
        *earlyReturn = fmi3False;
        *lastSuccessfulTime = this->syncTime_;
    }
    else // The importer has not yet reached the next event.
    {
        this->logDebug(
            "The importer has not yet reached the next event."
        );

        *eventEncountered = fmi3False;
        *earlyReturn = fmi3False;
        *lastSuccessfulTime = this->syncTime_;
    }

    *terminateSimulation = fmi3False;

    return fmi3OK;
}

bool
Pipeline_deterministic::addNewEvent(
    const TimeStamp& msgReceiveTime,
    const MessageID& msgId,
    const Receiver& receiver,
    const ReceiverClock& clock
) {
    bool firstEvent = this->eventQueue_.empty();

    // Create new event.
    Event* evt = new Event(
        msgReceiveTime,
        msgId,
        receiver,
        clock
    );

    this->logDebug(
        "add new event at t = %f - id = %d", msgReceiveTime, msgId
    );

    // Insert event into queue.
    std::pair<EventQueue::iterator ,bool> insertResult = this->eventQueue_.insert( evt );
    bool validEvent = insertResult.second;

    // Delete the event, because it has not been inserted successfully (another event with
    // the same timestamp alreay exists).
    if ( false == validEvent )
    {
        this->logDebug(
            "veto for event at t = %f", msgReceiveTime
        );

        delete evt;
        return false;
    }

    if ( firstEvent || ( msgReceiveTime < this->nextEventTime_ ) )
    {
        this->currentEvent_ = insertResult.first;
        this->nextEventTime_ = msgReceiveTime;
    }

    return true;
}

fmi3Float64
Pipeline_deterministic::calculateDelay()
{
    // No negative delays!
    return std::max(
        this->distribution_( this->generator_ ),
        this->randomMin_
    );
}

void
Pipeline_deterministic::deactivateAllClocks()
{
    inClock_ = fmi3ClockInactive;
    outClock_ = fmi3ClockInactive;
}
