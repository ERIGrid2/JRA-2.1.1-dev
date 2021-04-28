/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#include "InstanceBase.h"

#include "fmi3FunctionTypes.h"

#include "FMUMode.h"

#define NOT_IMPLEMENTED \
    this->logError( "Function is not implemented. <InstanceBase>" ); \
    return fmi3Discard;

/* Creation and destruction of FMU instances and setting debug status */
InstanceBase::InstanceBase(
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
    instanceName_( instanceName ),
    instantiationToken_( instantiationToken ),
    resourceLocation_( resourceLocation ),
    visible_( visible ),
    loggingOn_( loggingOn ),
    eventModeUsed_( eventModeUsed ),
    earlyReturnAllowed_( earlyReturnAllowed ),
    requiredIntermediateVariables_(
        requiredIntermediateVariables,
        requiredIntermediateVariables + nRequiredIntermediateVariables
    ),
    instanceEnvironment_( instanceEnvironment ),
    logger_( logMessage ),
    intermediateUpdate_( intermediateUpdate ),
    mode_( instantiated )
{
    this->logDebug( "instantiationToken = %s", this->instantiationToken_.c_str() );
    this->logDebug( "resourceLocation = %s", this->resourceLocation_.c_str() );
    this->logDebug( "visible = %d", this->visible_ );
    this->logDebug( "eventModeUsed = %d", this->eventModeUsed_ );
    this->logDebug( "earlyReturnAllowed = %d", this->earlyReturnAllowed_ );
}

fmi3Status
InstanceBase::setDebugLogging(
    fmi3Boolean loggingOn,
    size_t nCategories,
    const fmi3String categories[]
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::enterInitializationMode(
    fmi3Boolean toleranceDefined,
    fmi3Float64 tolerance,
    fmi3Float64 startTime,
    fmi3Boolean stopTimeDefined,
    fmi3Float64 stopTime
) {
    this->mode_ = initializationMode;
    return fmi3OK;
}

fmi3Status
InstanceBase::exitInitializationMode()
{
    this->mode_ = stepMode;
    return fmi3OK;
}

fmi3Status
InstanceBase::enterEventMode(
    fmi3Boolean stepEvent,
    fmi3Boolean stateEvent,
    const fmi3Int32 rootsFound[],
    size_t nEventIndicators,
    fmi3Boolean timeEvent
) {
    this->mode_ = eventMode;
    return fmi3OK;
}

fmi3Status
InstanceBase::terminate()
{
    this->mode_ = terminated;
    return fmi3OK;
}

fmi3Status
InstanceBase::reset()
{
    this->mode_ = instantiated;
    return fmi3OK;
}

/* Getting and setting variable values */
fmi3Status
InstanceBase::getFloat32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float32 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getFloat64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getInt8(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int8 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getUInt8(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt8 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getInt16(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int16 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getUInt16(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt16 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int32 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getUInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt32 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getInt64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getUInt64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getBoolean(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Boolean values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getString (
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3String values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getBinary(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    size_t sizes[],
    fmi3Binary values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getClock(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setFloat32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float32 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setFloat64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setInt8(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int8 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setUInt8(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt8 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setInt16(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int16 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setUInt16(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt16 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int32 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setUInt32(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt32 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setInt64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setUInt64(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setBoolean(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Boolean values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setString(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3String values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setBinary(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const size_t sizes[],
    const fmi3Binary values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setClock(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Clock values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

/* Getting Variable Dependency Information */
fmi3Status
InstanceBase::getNumberOfVariableDependencies(
    fmi3ValueReference valueReference,
    size_t* nDependencies
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getVariableDependencies(
    fmi3ValueReference dependent,
    size_t elementIndicesOfDependent[],
    fmi3ValueReference independents[],
    size_t elementIndicesOfIndependents[],
    fmi3DependencyKind dependencyKinds[],
    size_t nDependencies
) {
    NOT_IMPLEMENTED
}

/* Getting and setting the internal FMU state */
fmi3Status
InstanceBase::getFMUState (
    fmi3FMUState* fmuState
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setFMUState (
    fmi3FMUState fmuState
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::freeFMUState(
    fmi3FMUState* fmuState
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::serializedFMUStateSize(
    fmi3FMUState  fmuState,
    size_t* size
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::serializeFMUState(
    fmi3FMUState fmuState,
    fmi3Byte serializedState[],
    size_t size
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::deSerializeFMUState(
    const fmi3Byte serializedState[],
    size_t size,
    fmi3FMUState* fmuState
) {
    NOT_IMPLEMENTED
}

/* Getting partial derivatives */
fmi3Status
InstanceBase::getDirectionalDerivative(
    const fmi3ValueReference unknowns[],
    size_t nUnknowns,
    const fmi3ValueReference knowns[],
    size_t nKnowns,
    const fmi3Float64 seed[],
    size_t nSeed,
    fmi3Float64 sensitivity[],
    size_t nSensitivity
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getAdjointDerivative(
    const fmi3ValueReference unknowns[],
    size_t nUnknowns,
    const fmi3ValueReference knowns[],
    size_t nKnowns,
    const fmi3Float64 seed[],
    size_t nSeed,
    fmi3Float64 sensitivity[],
    size_t nSensitivity
) {
    NOT_IMPLEMENTED
}

/* Entering and exiting the Configuration or Reconfiguration Mode */
fmi3Status
InstanceBase::enterConfigurationMode()
{
    this->mode_ = configurationMode;
    return fmi3OK;
}

fmi3Status
InstanceBase::exitConfigurationMode()
{
    this->mode_ = initializationMode;
    return fmi3OK;
}

fmi3Status
InstanceBase::getIntervalDecimal(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 interval[],
    fmi3IntervalQualifier qualifier[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getIntervalFraction(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 intervalCounter[],
    fmi3UInt64 resolution[],
    fmi3IntervalQualifier qualifier[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getShiftDecimal(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 shift[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::getShiftFraction(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 shiftCounter[],
    fmi3UInt64 resolution[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setIntervalDecimal(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 interval[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::setIntervalFraction(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3UInt64 intervalCounter[],
    const fmi3UInt64 resolution[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::updateDiscreteStates(
    fmi3Boolean *discreteStatesNeedUpdate,
    fmi3Boolean *terminateSimulation,
    fmi3Boolean *nominalsOfContinuousStatesChanged,
    fmi3Boolean *valuesOfContinuousStatesChanged,
    fmi3Boolean *nextEventTimeDefined,
    fmi3Float64 *nextEventTime
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::enterStepMode()
{
    this->mode_ = stepMode;
    return fmi3OK;
}

fmi3Status
InstanceBase::getOutputDerivatives(
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Int32 orders[],
    fmi3Float64 values[],
    size_t nValues
) {
    NOT_IMPLEMENTED
}

fmi3Status
InstanceBase::doStep(
    fmi3Float64 currentCommunicationPoint,
    fmi3Float64 communicationStepSize,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* eventEncountered,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* earlyReturn,
    fmi3Float64* lastSuccessfulTime
) {
    NOT_IMPLEMENTED
}

void
InstanceBase::logMessage(
    fmi3Status status,
    const char *category,
    const char *message,
    va_list args
) {
    va_list args1;
    size_t len = 0;
    char* buf = new char;

    va_copy( args1, args );
    len = vsnprintf( buf, len, message, args1 );
    va_end( args1 );

    va_copy( args1, args );
    buf = ( char* ) calloc( len + 1, sizeof( char ) );
    vsnprintf( buf, len + 1, message, args );
    va_end( args1 );

    this->logger_(
        this->instanceEnvironment_, this->instanceName_.c_str(),
        status, category, buf
    );
}

void
InstanceBase::logDebug(
    const char *message,
    ...
) {
    if ( fmi3True == this->getLoggingOn() ) {
        va_list args;
        va_start( args, message );
        this->logMessage( fmi3OK, "DEBUG", message, args );
        va_end( args );
    }
}

void
InstanceBase::logError(
    const char *message,
    ...
) {
    va_list args;
    va_start( args, message );
    this->logMessage( fmi3Error, "ERROR", message, args );
    va_end( args );
}
