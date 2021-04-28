/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef InstanceBase_h
#define InstanceBase_h

#include <cstdarg>
#include <string>
#include <vector>

#include "fmi3FunctionTypes.h"
#include "fmi3Functions.h"

#include "FMUMode.h"


class InstanceBase {

public:

    /* Creation and destruction of FMU instances and setting debug status */
    InstanceBase(
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

    /* Inquire version numbers and setting logging status */
    static const char* getVersion() { 
        return fmi3Version; 
    }

    virtual fmi3Status setDebugLogging(
        fmi3Boolean loggingOn,
        size_t nCategories,
        const fmi3String categories[]
    );

    /* Enter and exit initialization mode, enter event mode, terminate and reset */
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

    virtual fmi3Status terminate();

    virtual fmi3Status reset();

    /* Getting and setting variable values */
    virtual fmi3Status getFloat32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float32 values[],
        size_t nValues
    );

    virtual fmi3Status getFloat64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float64 values[],
        size_t nValues
    );

    virtual fmi3Status getInt8(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Int8 values[],
        size_t nValues
    );

    virtual fmi3Status getUInt8(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt8 values[],
        size_t nValues
    );

    virtual fmi3Status getInt16(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Int16 values[],
        size_t nValues
    );

    virtual fmi3Status getUInt16(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt16 values[],
        size_t nValues
    );

    virtual fmi3Status getInt32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Int32 values[],
        size_t nValues
    );

    virtual fmi3Status getUInt32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt32 values[],
        size_t nValues
    );

    virtual fmi3Status getInt64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Int64 values[],
        size_t nValues
    );

    virtual fmi3Status getUInt64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt64 values[],
        size_t nValues
    );

    virtual fmi3Status getBoolean(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Boolean values[],
        size_t nValues
    );

    virtual fmi3Status getString (
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3String values[],
        size_t nValues
    );

    virtual fmi3Status getBinary(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        size_t sizes[],
        fmi3Binary values[],
        size_t nValues
    );

    virtual fmi3Status getClock(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Clock values[],
        size_t nValues
    );

    virtual fmi3Status setFloat32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float32 values[],
        size_t nValues
    );

    virtual fmi3Status setFloat64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float64 values[],
        size_t nValues
    );

    virtual fmi3Status setInt8(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Int8 values[],
        size_t nValues
    );

    virtual fmi3Status setUInt8(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt8 values[],
        size_t nValues
    );

    virtual fmi3Status setInt16(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Int16 values[],
        size_t nValues
    );

    virtual fmi3Status setUInt16(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt16 values[],
        size_t nValues
    );

    virtual fmi3Status setInt32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Int32 values[],
        size_t nValues
    );

    virtual fmi3Status setUInt32(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt32 values[],
        size_t nValues
    );

    virtual fmi3Status setInt64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Int64 values[],
        size_t nValues
    );

    virtual fmi3Status setUInt64(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt64 values[],
        size_t nValues
    );

    virtual fmi3Status setBoolean(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Boolean values[],
        size_t nValues
    );

    virtual fmi3Status setString(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3String values[],
        size_t nValues
    );

    virtual fmi3Status setBinary(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const size_t sizes[],
        const fmi3Binary values[],
        size_t nValues
    );

    virtual fmi3Status setClock(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Clock values[],
        size_t nValues
    );

    /* Getting Variable Dependency Information */
    virtual fmi3Status getNumberOfVariableDependencies(
        fmi3ValueReference valueReference,
        size_t* nDependencies
    );

    virtual fmi3Status getVariableDependencies(
        fmi3ValueReference dependent,
        size_t elementIndicesOfDependent[],
        fmi3ValueReference independents[],
        size_t elementIndicesOfIndependents[],
        fmi3DependencyKind dependencyKinds[],
        size_t nDependencies
    );

    /* Getting and setting the internal FMU state */
    virtual fmi3Status getFMUState (
        fmi3FMUState* fmuState
    );

    virtual fmi3Status setFMUState (
        fmi3FMUState fmuState
    );

    virtual fmi3Status freeFMUState(
        fmi3FMUState* fmuState
    );

    virtual fmi3Status serializedFMUStateSize(
        fmi3FMUState  fmuState,
        size_t* size
    );

    virtual fmi3Status serializeFMUState(
        fmi3FMUState fmuState,
        fmi3Byte serializedState[],
        size_t size
    );

    virtual fmi3Status deSerializeFMUState(
        const fmi3Byte serializedState[],
        size_t size,
        fmi3FMUState* fmuState
    );

    /* Getting partial derivatives */
    virtual fmi3Status getDirectionalDerivative(
        const fmi3ValueReference unknowns[],
        size_t nUnknowns,
        const fmi3ValueReference knowns[],
        size_t nKnowns,
        const fmi3Float64 seed[],
        size_t nSeed,
        fmi3Float64 sensitivity[],
        size_t nSensitivity
    );

    virtual fmi3Status getAdjointDerivative(
        const fmi3ValueReference unknowns[],
        size_t nUnknowns,
        const fmi3ValueReference knowns[],
        size_t nKnowns,
        const fmi3Float64 seed[],
        size_t nSeed,
        fmi3Float64 sensitivity[],
        size_t nSensitivity
    );

    /* Entering and exiting the Configuration or Reconfiguration Mode */
    virtual fmi3Status enterConfigurationMode();

    virtual fmi3Status exitConfigurationMode();

    virtual fmi3Status getIntervalDecimal(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float64 interval[],
        fmi3IntervalQualifier qualifier[],
        size_t nValues
    );

    virtual fmi3Status getIntervalFraction(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt64 intervalCounter[],
        fmi3UInt64 resolution[],
        fmi3IntervalQualifier qualifier[],
        size_t nValues
    );

    virtual fmi3Status getShiftDecimal(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float64 shift[],
        size_t nValues
    );

    virtual fmi3Status getShiftFraction(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt64 shiftCounter[],
        fmi3UInt64 resolution[],
        size_t nValues
    );

    virtual fmi3Status setIntervalDecimal(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float64 interval[],
        size_t nValues
    );

    virtual fmi3Status setIntervalFraction(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt64 intervalCounter[],
        const fmi3UInt64 resolution[],
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

    virtual fmi3Status enterStepMode();

    virtual fmi3Status getOutputDerivatives(
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Int32 orders[],
        fmi3Float64 values[],
        size_t nValues
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

    FMUMode getMode() { return this->mode_; }

    void logMessage(
        fmi3Status status,
        const char *category,
        const char *message,
    va_list args
    );

    void logDebug(
        const char *message,
        ...
    );

    void logError(
        const char *message,
        ...
    );

protected:

    const std::string& getInstanceName() { 
        return this->instanceName_; 
    }

    const std::string& getInstantiationToken() { 
        return this->instantiationToken_;
    }

    const std::string& getResourceLocation() { 
        return this->resourceLocation_;
    }

    bool getVisible() { 
        return this->visible_;
    }

    bool getLoggingOn() { 
        return this->loggingOn_;
    }

    bool getEventModeUsed() { 
        return this->eventModeUsed_;
    }

    bool getEarlyReturnAllowed() { 
        return this->earlyReturnAllowed_;
    }

    const std::vector<fmi3ValueReference>& getRequiredIntermediateVariables() { 
        return this->requiredIntermediateVariables_ ;
    }

    const fmi3InstanceEnvironment getInstanceEnvironment() {
        return this->instanceEnvironment_;
    }

    const fmi3CallbackLogMessage getLogger() {  
        return this->logger_; 
    }

    const fmi3CallbackIntermediateUpdate getIntermediateUpdate() { 
        return this->intermediateUpdate_; 
    }

    void setMode( FMUMode mode ) { 
        this->mode_ = mode; 
    }

private:

    const std::string instanceName_;
    const std::string instantiationToken_;
    const std::string resourceLocation_;
    const bool visible_;
    const bool loggingOn_;
    const bool eventModeUsed_;
    const bool earlyReturnAllowed_;
    const std::vector<fmi3ValueReference> requiredIntermediateVariables_;

    const fmi3InstanceEnvironment instanceEnvironment_;

    const fmi3CallbackLogMessage logger_;
    const fmi3CallbackIntermediateUpdate intermediateUpdate_;

    FMUMode mode_;
};

#endif // InstanceBase_h
