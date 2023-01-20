/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#include "Pipeline_two_deterministic.h"
#include <ostream>
#include <iostream>
#include <limits>
#include <algorithm>
#include <stdexcept>

#define INSTANTIATION_TOKEN "{a67992a0-a385-11eb-aea4-00155d0bce5e}"

Pipeline_two_deterministic::Pipeline_two_deterministic(
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
    eventHappenedInternal1(fmi3False),
    eventHappenedInternal2(fmi3False),
    in_( 0 ),
    out_( 0 ),
    pipe1("pipe1"),
    pipe2("pipe2"),
    ic12("ic12",false),
    ic23("ic23",true)
    //pipe1(),
    //pipe2()

{
    std::cout << ">> Pipeline_two_deterministic::constructor" << std::endl << std::flush;
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
        "successfully initialized class %s", "Pipeline_two_deterministic"
    );
    std::cout << "<< Pipeline_two_deterministic::constructor" << std::endl << std::flush;
}

fmi3Status Pipeline_two_deterministic::enterInitializationMode(
    fmi3Boolean toleranceDefined,
    fmi3Float64 tolerance,
    fmi3Float64 startTime,
    fmi3Boolean stopTimeDefined,
    fmi3Float64 stopTime
) {
    std::cout << ">> Pipeline_two_deterministic::enterInitializationMode" << std::endl << std::flush;
    this->setMode( initializationMode );

    // Set internal time to simulation start time.
    this->syncTime_ = startTime;

    // Adjust tolerances for determining if two timestamps are the same.
    if ( fmi3True == toleranceDefined )
    {
        pipe1.setTolerance(tolerance);
        pipe2.setTolerance(tolerance);
        tolerance_=tolerance;
    }

    std::cout << "<< Pipeline_two_deterministic::enterInitializationMode" << std::endl << std::flush;
    return fmi3OK;
}

fmi3Status
Pipeline_two_deterministic::exitInitializationMode()
{
    std::cout << ">> Pipeline_two_deterministic::exitInitializationMode" << std::endl << std::flush;
    this->setMode( stepMode );

    pipe1.initialize();
    pipe2.initialize();

    std::cout << "<< Pipeline_two_deterministic::exitInitializationMode" << std::endl << std::flush;
    return fmi3OK;
}

/*
 * Process internal events - i.e. messages that have passed to the end of a pipe during the last step
 */
fmi3Status 
Pipeline_two_deterministic::enterEventMode(
    /*fmi3Boolean stepEvent,
    fmi3Boolean stateEvent,
    const fmi3Int32 rootsFound[],
    size_t nEventIndicators,
    fmi3Boolean timeEvent*/
) {
    std::cout << ">> Pipeline_two_deterministic::enterEventMode" << std::endl << std::flush;
    this->setMode( eventMode );
    
    // This is a time event that was previously signaled by function doStep.
    // This means that a new message is available - either to be received by the importer or to be pushed into the next pipe.
    std::cout << "  eventHappenedInternal1=" << this->eventHappenedInternal1 << ", eventHappenedInternal2=" << this->eventHappenedInternal2 << std::endl << std::flush;
    if ( fmi3True == this->eventHappenedInternal1 ) {
        pipe1.processInternalEvent();
    }
    if ( fmi3True == this->eventHappenedInternal2 ) {
        pipe2.processInternalEvent();
    }
    if (ic23.hasMessage()) {
        out_ = ic23.getMessage();
        outClock_ = fmi3ClockActive;
    }
    std::cout << "<< Pipeline_two_deterministic::!enterEventMode" << std::endl << std::flush;
    return fmi3OK;
}

fmi3Status
Pipeline_two_deterministic::reset()
{
    std::cout << ">> Pipeline_two_deterministic::reset" << std::endl << std::flush;
    pipe1.reset();
    pipe2.reset();

    std::cout << "<< Pipeline_two_deterministic::!reset" << std::endl << std::flush;
    return fmi3OK;
}

fmi3Status
Pipeline_two_deterministic::getInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int32 values[],
    size_t nValues
) {
    std::cout << ">> Pipeline_two_deterministic::getInt32" << std::endl << std::flush;
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        std::cout << "<< Pipeline_two_deterministic::getInt32" << std::endl << std::flush;
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

    std::cout << "<< Pipeline_two_deterministic::getInt32" << std::endl << std::flush;
    return status;
}

fmi3Status
Pipeline_two_deterministic::getClock(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock values[]
) {
    std::cout << ">> Pipeline_two_deterministic::getClock" << std::endl << std::flush;
    /*if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        std::cout << "<< Pipeline_two_deterministic::getClock" << std::endl << std::flush;
        return fmi3Error;
    }*/

    fmi3Status status = fmi3OK;
    const fmi3ValueReference* vr;
    fmi3Clock* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValueReferences, v != values + nValueReferences;
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

    std::cout << "<< Pipeline_two_deterministic::getClock" << std::endl << std::flush;
    return status;
}

fmi3Status
Pipeline_two_deterministic::setInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int32 values[],
    size_t nValues
) {
    std::cout << ">> Pipeline_two_deterministic::setInt32" << std::endl << std::flush;
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        std::cout << "<< Pipeline_two_deterministic::!setInt32" << std::endl << std::flush;
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
                pipe1.setRandomSeed(*v);
                pipe2.setRandomSeed(*v);
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }

        this->logDebug( "Value reference %d => set to: %d (fmi3Int32)", *vr, *v );
    }
    std::cout << "<< Pipeline_two_deterministic::!setInt32" << std::endl << std::flush;
    return status;
}

fmi3Status
Pipeline_two_deterministic::setFloat64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 values[],
    size_t nValues
) {
    std::cout << ">> Pipeline_two_deterministic::setFloat64" << std::endl << std::flush;
    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        std::cout << "<< Pipeline_two_deterministic::setFloat64" << std::endl << std::flush;
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
                pipe1.setRandomMean(*v);
                pipe2.setRandomMean(*v);
                break;
            case this->vrRandomStdDev_:
                pipe1.setRandomStdDev(*v);
                pipe2.setRandomStdDev(*v);
                break;
            case this->vrRandomMin_:
                pipe1.setRandomMin(*v);
                pipe2.setRandomMin(*v);
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }

        this->logDebug( 
            "Value reference %d => set to: %f (fmi3Float64)", *vr, *v 
        );
    }

    std::cout << "<< Pipeline_two_deterministic::setFloat64" << std::endl << std::flush;
    return status;
}

fmi3Status
Pipeline_two_deterministic::setClock(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Clock values[])
{
    std::cout << ">> Pipeline_two_deterministic::setClock" << std::endl << std::flush;
/*    if ( nValueReferences != nValues ) {
        this->logError(
            "%s %s",
            "This FMU only supports scalar variables.",
            "The number of value references and values must match!"
        );
        std::cout << "<< Pipeline_two_deterministic::setClock" << std::endl << std::flush;
        return fmi3Error;
    }*/

    fmi3Status status = fmi3OK;
    const fmi3ValueReference* vr;
    const fmi3Clock* v;

    for (
        vr = valueReferences, v = values;
        vr != valueReferences + nValueReferences, v != values + nValueReferences;
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
                this->logDebug( "%d => set clock %d", *vr, this->inClock_ );
                break;
            default:
                this->logError( "Invalid value reference: %d", *vr );
                status = fmi3Error;
        }

        this->logDebug( 
            "%d => set clock %d", *vr, *v
        );
    }

    std::cout << "<< Pipeline_two_deterministic::setClock" << std::endl << std::flush;
    return status;
}

fmi3Status
Pipeline_two_deterministic::updateDiscreteStates(
    fmi3Boolean *discreteStatesNeedUpdate,
    fmi3Boolean *terminateSimulation,
    fmi3Boolean *nominalsOfContinuousStatesChanged,
    fmi3Boolean *valuesOfContinuousStatesChanged,
    fmi3Boolean *nextEventTimeDefined,
    fmi3Float64 *nextEventTime
) {
    std::cout << ">> Pipeline_two_deterministic::updateDiscreteStates" << std::endl << std::flush;
    // Input clock is active --> add message as output using the calculated delay.
    if ( fmi3ClockActive == this->inClock_ ) {
        pipe1.insertMessage(syncTime_,in_,&ic12/*&out_,&outClock_*/);
    }
    if (ic12.hasMessage()) {
        fmi3Int32 msg=ic12.getMessage();
        std::cout << "### Message from pipe1: " << msg << ", inserting into pipe2" << std::endl << std::flush;
        pipe2.insertMessage(syncTime_,msg,&ic23);
    }

    *nextEventTimeDefined = fmi3False;
    fmi3Float64 netime = std::numeric_limits<fmi3Float64>::max();
    fmi3Float64 net1=pipe1.processQueue(syncTime_, nextEventTimeDefined);
    netime=std::min(netime,net1);
    fmi3Float64 net2=pipe2.processQueue(syncTime_, nextEventTimeDefined);
    netime=std::min(netime,net2);
    std::cout << "%%%%%%% Next Event times: pipe 1=" << net1 << ", pipe2=" << net2 << ", combined=" << netime << std::endl << std::flush;

    *discreteStatesNeedUpdate = fmi3False;
    *terminateSimulation = fmi3False;
    *nominalsOfContinuousStatesChanged = fmi3False;
    *valuesOfContinuousStatesChanged = fmi3False;
    *nextEventTime = netime;

    // We have finished processing internal events --> deactivate all active clocks.
    this->deactivateAllClocks();

    std::cout << "<< Pipeline_two_deterministic::updateDiscreteStates" << std::endl << std::flush;
    return fmi3OK;
}

fmi3Status
Pipeline_two_deterministic::doStep(
    fmi3Float64 currentCommunicationPoint,
    fmi3Float64 communicationStepSize,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* eventEncountered,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* earlyReturn,
    fmi3Float64* lastSuccessfulTime
) {
    std::cout << ">> Pipeline_two_deterministic::doStep" << std::endl << std::flush;
    std::cout << "Current communication point (" << currentCommunicationPoint << "), internal time (" << this->syncTime_ << "), comm step size (" << communicationStepSize << ")" << std::endl << std::flush;
    // Sanity check: Do the importer's current communication point and the internal
    // synchronization time coincide?
    if ( fabs( this->syncTime_ - currentCommunicationPoint ) > this->tolerance_ )
    {
        std::cout << "Current communication point (" << currentCommunicationPoint << ") does not coincide with the internal time (" << this->syncTime_ << ") " << std::endl;
        //this->logError(
        //    "Current communication point (%f) does not coincide with the internal time (%f)",
        //    currentCommunicationPoint, this->syncTime_
        //);

        std::cout << "<< Pipeline_two_deterministic::doStep" << std::endl << std::flush;
        return fmi3Discard;
    }
    std::cout << "xxb" << std::endl;

    // Update internal synchronization time to new requested communication point.
    this->syncTime_ = currentCommunicationPoint + communicationStepSize;
    this->logDebug(
        "Attempt to step from %f to %f",
        currentCommunicationPoint,
        this->syncTime_
    );
    std::cout << "xxc" << std::endl;
    eventHappenedInternal1 = fmi3False; //Default
    eventHappenedInternal2 = fmi3False; //Default
    // The importer stepped over an event.
    if (pipe1.didWeOverstep(syncTime_)) {
        fmi3Float64 next=pipe1.getNextEventTime();
        std::cout << "xxd1" << std::endl;
        this->logDebug(
            "%s %s %f",
            "The importer stepped over an event in pipe 1.",
            "The current internal time (lastSuccessfulTime) is: ",next);
        *eventEncountered = fmi3True;
        eventHappenedInternal1 = fmi3True;
        *earlyReturn = fmi3True;
        *lastSuccessfulTime = next;
    }
    else if (pipe2.didWeOverstep(syncTime_)) {
        fmi3Float64 next=pipe2.getNextEventTime();
        std::cout << "xxd2" << std::endl;
        this->logDebug(
            "%s %s %f",
            "The importer stepped over an event in pipe 2.",
            "The current internal time (lastSuccessfulTime) is: ",next);
        *eventEncountered = fmi3True;
        eventHappenedInternal2 = fmi3True;
        *earlyReturn = fmi3True;
        *lastSuccessfulTime = next;
    }
    // The importer has reached the next event.
    else if (pipe1.didWeReach(syncTime_)) {
        std::cout << "xxe1" << std::endl;
        this->logDebug(
            "The importer has reached the next pipe1 event at the new synchronization point."
        );
        if (pipe1.isQueueCorrupt()) {
            this->logError("corrupted event queue in pipe1");
            *terminateSimulation = fmi3True;
            return fmi3Fatal;
        }
        *eventEncountered = fmi3True;
        eventHappenedInternal1 = fmi3True;
        *earlyReturn = fmi3False;
        *lastSuccessfulTime = this->syncTime_;
    }
    else if (pipe2.didWeReach(syncTime_)) {
        std::cout << "xxe2" << std::endl;
        this->logDebug(
            "The importer has reached the next pipe2 event at the new synchronization point."
        );
        if (pipe2.isQueueCorrupt()) {
            this->logError("corrupted event queue in pipe2");
            *terminateSimulation = fmi3True;
            return fmi3Fatal;
        }
        *eventEncountered = fmi3True;
        eventHappenedInternal2 = fmi3True;
        *earlyReturn = fmi3False;
        *lastSuccessfulTime = this->syncTime_;
    }
    else // The importer has not yet reached the next event.
    {
    std::cout << "xxf" << std::endl;

        this->logDebug(
            "The importer has not yet reached the next event."
        );

        *eventEncountered = fmi3False;
        *earlyReturn = fmi3False;
        *lastSuccessfulTime = this->syncTime_;
    }

    *terminateSimulation = fmi3False;

    std::cout << "<< Pipeline_two_deterministic::doStep" << std::endl << std::flush;
    return fmi3OK;
}

void
Pipeline_two_deterministic::deactivateAllClocks()
{
    std::cout << ">> Pipeline_two_deterministic::deactivateAllClocks" << std::endl << std::flush;
    inClock_ = fmi3ClockInactive;
    outClock_ = fmi3ClockInactive;
    std::cout << "<< Pipeline_two_deterministic::deactivateAllClocks" << std::endl << std::flush;
}
