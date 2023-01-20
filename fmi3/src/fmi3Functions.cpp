/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

/**
 * Compile this file in order to generate an FMU CS (Version 3.0) that uses 
 * an instance derived from class InstanceBase.
 *
 * When compiling, define the following macros accordingly:
 *  - INSTANCE_TYPE: class name of the derived instance
 *  - INSTANCE_TYPE_INCLUDE: header file of the class of the derived instance
 *
 * Example (for GCC): -DINSTANCE_TYPE=Pipeline_deterministic -DINSTANCE_TYPE_INCLUDE="Pipeline_deterministic.h"
 */ 
 
#include <cstdarg>
#include <stdexcept>

#include "fmi3Functions.h"

#include "FMUMode.h"
#include "AllowedFMUMode.h"
#include INSTANCE_TYPE_INCLUDE

#define NOT_IMPLEMENTED \
    InstanceBase* impl = static_cast<InstanceBase*>( instance ); \
    impl->logError( "Function is not implemented." ); \
    return fmi3Error;

#define CHECK_STATE_AND_CALL_METHOD( METHOD_STUB, ... ) \
    InstanceBase* impl = static_cast<InstanceBase*>( instance ); \
    if( !AllowedFMUMode::check( impl, AllowedFMUMode::METHOD_STUB, #METHOD_STUB ) ) { return fmi3Error; } \
    return impl->METHOD_STUB( __VA_ARGS__ );


const char* fmi3GetVersion()
{
    return InstanceBase::getVersion();
}

fmi3Status fmi3SetDebugLogging(
    fmi3Instance instance,
    fmi3Boolean loggingOn,
    size_t nCategories,
    const fmi3String categories[]
) {
    CHECK_STATE_AND_CALL_METHOD(
        setDebugLogging,
        loggingOn,
        nCategories,
        categories
    )
}

fmi3Instance fmi3InstantiateModelExchange(
    fmi3String instanceName,
    fmi3String instantiationToken,
    fmi3String resourcePath,
    fmi3Boolean visible,
    fmi3Boolean loggingOn,
    fmi3InstanceEnvironment instanceEnvironment,
    fmi3LogMessageCallback logMessage
) {
    return NULL;
}

fmi3Instance fmi3InstantiateCoSimulation(
    fmi3String instanceName,
    fmi3String instantiationToken,
    fmi3String resourcePath,
    fmi3Boolean visible,
    fmi3Boolean loggingOn,
    fmi3Boolean eventModeUsed,
    fmi3Boolean earlyReturnAllowed,
    const fmi3ValueReference requiredIntermediateVariables[],
    size_t nRequiredIntermediateVariables,
    fmi3InstanceEnvironment instanceEnvironment,
    fmi3LogMessageCallback logMessage,
    fmi3IntermediateUpdateCallback intermediateUpdate
) {
    try 
    {
        InstanceBase *instance = new INSTANCE_TYPE(
            instanceName,
            instantiationToken,
            resourcePath,
            visible,
            loggingOn,
            eventModeUsed,
            earlyReturnAllowed,
            requiredIntermediateVariables,
            nRequiredIntermediateVariables,
            instanceEnvironment,
            logMessage,
            intermediateUpdate
        );

        return instance;
    }
    catch ( std::runtime_error& err ) 
    {
        logMessage(
            instanceEnvironment,
            fmi3Fatal,
            "ERROR",
            err.what()
        );
    }

    return NULL;
}

/*fmi3Instance fmi3InstantiateScheduledExecution(
    fmi3String instanceName,
    fmi3String instantiationToken,
    fmi3String resourcePath,
    fmi3Boolean visible,
    fmi3Boolean loggingOn,
    fmi3InstanceEnvironment instanceEnvironment,
    fmi3LogMessageCallback logMessage,
    fmi3ClockUpdateCallback clockUpdate,
    fmi3LockPreemptionCallback lockPreemption,
    fmi3UnlockPreemptionCallback unlockPreemption
) {
    NOT_IMPLEMENTED
}*/


void fmi3FreeInstance(
    fmi3Instance instance
) {
    INSTANCE_TYPE* impl = static_cast<INSTANCE_TYPE*>( instance );
    delete impl;
}

fmi3Status fmi3EnterInitializationMode(
    fmi3Instance instance,
    fmi3Boolean toleranceDefined,
    fmi3Float64 tolerance,
    fmi3Float64 startTime,
    fmi3Boolean stopTimeDefined,
    fmi3Float64 stopTime
) {
    CHECK_STATE_AND_CALL_METHOD(
        enterInitializationMode,
        toleranceDefined,
        tolerance,
        startTime,
        stopTimeDefined,
        stopTime
    )
}

fmi3Status fmi3ExitInitializationMode(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        exitInitializationMode
    )
}

fmi3Status fmi3EnterEventMode(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        enterEventMode
    )
}

fmi3Status fmi3Terminate(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        terminate
    )
}

fmi3Status fmi3Reset(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        reset
    )
}

fmi3Status fmi3GetFloat32(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float32 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getFloat32,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetFloat64(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getFloat64,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetInt8(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int8 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getInt8,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetUInt8(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt8 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getUInt8,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetInt16(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int16 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getInt16,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetUInt16(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt16 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getUInt16,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetInt32(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int32 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getInt32,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetUInt32(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt32 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getUInt32,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetInt64(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getInt64,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetUInt64(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getUInt64,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetBoolean(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Boolean values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getBoolean,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetString(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3String values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getString,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3GetBinary(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    size_t sizes[],
    fmi3Binary values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        getBinary,
        valueReferences,
        nValueReferences,
        sizes,
        values,
        nValues
    )
}

fmi3Status fmi3SetFloat32(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float32 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setFloat32,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetFloat64(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setFloat64,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetInt8(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int8 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setInt8,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetUInt8(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt8 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setUInt8,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetInt16(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int16 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setInt16,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetUInt16(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt16 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setUInt16,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetInt32(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int32 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setInt32,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetUInt32(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt32 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setUInt32,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetInt64(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setInt64,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetUInt64(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setUInt64,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetBoolean(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Boolean values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setBoolean,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetString(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3String values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setString,
        valueReferences,
        nValueReferences,
        values,
        nValues
    )
}

fmi3Status fmi3SetBinary(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const size_t valueSizes[],
    const fmi3Binary values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD(
        setBinary,
        valueReferences,
        nValueReferences,
        valueSizes,
        values,
        nValues
    )
}

fmi3Status fmi3GetNumberOfVariableDependencies(
    fmi3Instance instance,
    fmi3ValueReference valueReference,
    size_t* nDependencies
) {
    CHECK_STATE_AND_CALL_METHOD(
        getNumberOfVariableDependencies,
        valueReference,
        nDependencies
    )
}

fmi3Status fmi3GetVariableDependencies(
    fmi3Instance instance,
    fmi3ValueReference dependent,
    size_t elementIndicesOfDependent[],
    fmi3ValueReference independents[],
    size_t elementIndicesOfIndependents[],
    fmi3DependencyKind dependencyKinds[],
    size_t nDependencies
) {
    CHECK_STATE_AND_CALL_METHOD(
        getVariableDependencies,
        dependent,
        elementIndicesOfDependent,
        independents,
        elementIndicesOfIndependents,
        dependencyKinds,
        nDependencies
    )
}

fmi3Status fmi3GetFMUState(
    fmi3Instance instance,
    fmi3FMUState* fmuState
) {
    CHECK_STATE_AND_CALL_METHOD(
        getFMUState,
        fmuState
    )
}

fmi3Status fmi3SetFMUState(
    fmi3Instance instance,
    fmi3FMUState fmuState
) {
    CHECK_STATE_AND_CALL_METHOD(
        setFMUState,
        fmuState
    )
}

fmi3Status fmi3FreeFMUState(
    fmi3Instance instance,
    fmi3FMUState* fmuState
) {
    CHECK_STATE_AND_CALL_METHOD(
        freeFMUState,
        fmuState
    )
}

fmi3Status fmi3SerializedFMUStateSize(
    fmi3Instance instance,
    fmi3FMUState fmuState,
    size_t *size
) {
    CHECK_STATE_AND_CALL_METHOD(
        serializedFMUStateSize,
        fmuState,
        size
    )
}

fmi3Status fmi3SerializeFMUState(
    fmi3Instance instance,
    fmi3FMUState fmuState,
    fmi3Byte serializedState[],
    size_t size
) {
    CHECK_STATE_AND_CALL_METHOD(
        serializeFMUState,
        fmuState,
        serializedState,
        size
    )
}

fmi3Status fmi3DeSerializeFMUState (
    fmi3Instance instance,
    const fmi3Byte serializedState[],
    size_t size,
    fmi3FMUState* fmuState
) {
    CHECK_STATE_AND_CALL_METHOD(
        deSerializeFMUState,
        serializedState,
        size,
        fmuState
    )
}

fmi3Status fmi3GetDirectionalDerivative(
    fmi3Instance instance,
    const fmi3ValueReference unknowns[],
    size_t nUnknowns,
    const fmi3ValueReference knowns[],
    size_t nKnowns,
    const fmi3Float64 seed[],
    size_t nSeed,
    fmi3Float64 sensitivity[],
    size_t nSensitivity
) {
    CHECK_STATE_AND_CALL_METHOD(
        getDirectionalDerivative,
        unknowns,
        nUnknowns,
        knowns,
        nKnowns,
        seed,
        nSeed,
        sensitivity,
        nSensitivity
    )
}

fmi3Status fmi3GetAdjointDerivative(
    fmi3Instance instance,
    const fmi3ValueReference unknowns[],
    size_t nUnknowns,
    const fmi3ValueReference knowns[],
    size_t nKnowns,
    const fmi3Float64 seed[],
    size_t nSeed,
    fmi3Float64 sensitivity[],
    size_t nSensitivity
) {
    CHECK_STATE_AND_CALL_METHOD(
        getAdjointDerivative,
        unknowns,
        nUnknowns,
        knowns,
        nKnowns,
        seed,
        nSeed,
        sensitivity,
        nSensitivity
    )
}

fmi3Status fmi3EnterConfigurationMode(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        enterConfigurationMode
    )
}

fmi3Status fmi3ExitConfigurationMode(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        exitConfigurationMode
    )
}

fmi3Status fmi3SetClock(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Clock values[]
) {
    CHECK_STATE_AND_CALL_METHOD(
        setClock,
        valueReferences,
        nValueReferences,
        values
    )
}

fmi3Status fmi3GetClock(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock values[]
) {
    CHECK_STATE_AND_CALL_METHOD(
        getClock,
        valueReferences,
        nValueReferences,
        values
    )
}

fmi3Status fmi3GetIntervalDecimal(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 interval[],
    fmi3IntervalQualifier qualifiers[]
) {
    CHECK_STATE_AND_CALL_METHOD(
        getIntervalDecimal,
        valueReferences,
        nValueReferences,
        interval,
        qualifiers
    )
}

fmi3Status fmi3SetIntervalDecimal(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 intervals[]
) {
    CHECK_STATE_AND_CALL_METHOD( 
        setIntervalDecimal,
        valueReferences,
        nValueReferences,
        intervals
    )
}

fmi3Status fmi3GetIntervalFraction(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 counters[],
    fmi3UInt64 resolutions[],
    fmi3IntervalQualifier qualifiers[]
) {
    CHECK_STATE_AND_CALL_METHOD( 
        getIntervalFraction,
        valueReferences,
        nValueReferences,
        counters,
        resolutions,
        qualifiers
    )
}

fmi3Status fmi3SetIntervalFraction(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt64 counters[],
    const fmi3UInt64 resolutions[]
) {
    CHECK_STATE_AND_CALL_METHOD( 
        setIntervalFraction,
        valueReferences,
        nValueReferences,
        counters,
        resolutions
    )
}

fmi3Status fmi3UpdateDiscreteStates(
    fmi3Instance instance,
    fmi3Boolean* discreteStatesNeedUpdate,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* nominalsOfContinuousStatesChanged,
    fmi3Boolean* valuesOfContinuousStatesChanged,
    fmi3Boolean* nextEventTimeDefined,
    fmi3Float64* nextEventTime
) {
    CHECK_STATE_AND_CALL_METHOD(
        updateDiscreteStates,
        discreteStatesNeedUpdate,
        terminateSimulation,
        nominalsOfContinuousStatesChanged,
        valuesOfContinuousStatesChanged,
        nextEventTimeDefined,
        nextEventTime
    )
}

fmi3Status fmi3EnterContinuousTimeMode(
    fmi3Instance instance
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetShiftDecimal(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 shifts[]
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetShiftFraction(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 counters[],
    const fmi3Float64 resolutions[]
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3CompletedIntegratorStep(
    fmi3Instance instance, 
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean *enterEventMode, 
    fmi3Boolean *terminateSimulation
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetTime(
    fmi3Instance instance, 
    fmi3Float64 time
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetContinuousStates(
    fmi3Instance instance, 
    const fmi3Float64 continuousStates[],
    size_t nContinuousStates
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetContinuousStateDerivatives(
    fmi3Instance instance, 
    fmi3Float64 derivatives[], 
    size_t nContinuousStates
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetEventIndicators(
    fmi3Instance instance, 
    fmi3Float64 eventIndicators[], 
    size_t nEventIndicators
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetContinuousStates(
    fmi3Instance instance, 
    fmi3Float64 continuousStates[],
    size_t nContinuousStates
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetNominalsOfContinuousStates(
    fmi3Instance instance, 
    fmi3Float64 nominals[],
    size_t nContinuousStates
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetNumberOfEventIndicators(
    fmi3Instance instance, 
    size_t* nEventIndicators
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetNumberOfContinuousStates(
    fmi3Instance instance, 
    size_t* nContinuousStates
) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3EnterStepMode(
    fmi3Instance instance
) {
    CHECK_STATE_AND_CALL_METHOD(
        enterStepMode
    )
}

fmi3Status fmi3GetOutputDerivatives(
    fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int32 orders[],
    fmi3Float64 values[],
    size_t nValues
) {
    CHECK_STATE_AND_CALL_METHOD( 
        getOutputDerivatives,
        valueReferences,
        nValueReferences,
        orders,
        values,
        nValues
    )
}

fmi3Status fmi3DoStep(
    fmi3Instance instance,
    fmi3Float64 currentCommunicationPoint,
    fmi3Float64 communicationStepSize,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* eventHandlingNeeded,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* earlyReturn,
    fmi3Float64* lastSuccessfulTime
) {
    CHECK_STATE_AND_CALL_METHOD( 
        doStep,
        currentCommunicationPoint,
        communicationStepSize,
        noSetFMUStatePriorToCurrentPoint,
        eventHandlingNeeded,
        terminateSimulation,
        earlyReturn,
        lastSuccessfulTime
    )
}

fmi3Status fmi3ActivateModelPartition(
    fmi3Instance instance,
    fmi3ValueReference clockReference,
    fmi3Float64 activationTime
) {
    NOT_IMPLEMENTED
}
