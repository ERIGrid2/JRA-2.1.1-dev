/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef AllowedState_h
#define AllowedState_h

#include "FMUMode.h"

class InstanceBase;


// ---------------------------------------------------------------------------
// Function calls allowed state masks for both Model-exchange and Co-simulation
// ---------------------------------------------------------------------------
struct AllowedFMUMode {

    /* Inquire version numbers and set debug logging */
    static const int getVersion               = setable;
    static const int setDebugLogging          = setable;

    /* Creation and destruction of FMU instances */
    static const int instantiateInstantiateModelExchange = setable;
    static const int instantiateCoSimulation             = setable;
    static const int instantiateScheduledExectuion       = setable;
    static const int freeInstance                        = setable;

    /* Enter and exit initialization mode, terminate and reset */
    static const int enterInitializationMode  = instantiated;
    static const int exitInitializationMode   = initializationMode;
    static const int enterEventMode           = ( continuousTimeMode | stepMode );
    static const int terminate                = ( continuousTimeMode | stepMode | stepDiscarded | eventMode | clockActivationMode | reconfigurationMode );
    static const int reset                    = setable;

    /* Common Functions */

    /* Getting and setting variable values */
    static const int getFloat32               = ( initializationMode | configurationMode | reconfigurationMode | eventMode | continuousTimeMode | stepMode | clockActivationMode | intermediateUpdateMode | terminated );
    static const int getFloat64               = getFloat32;
    static const int getInt8                  = getFloat32;
    static const int getUInt8                 = getFloat32;
    static const int getInt16                 = getFloat32;
    static const int getUInt16                = getFloat32;
    static const int getInt32                 = getFloat32;
    static const int getUInt32                = getFloat32;
    static const int getInt64                 = getFloat32;
    static const int getUInt64                = getFloat32;
    static const int getBoolean               = getFloat32;
    static const int getString                = getFloat32;
    static const int getBinary                = getFloat32;

    static const int setFloat32               = ( instantiated | initializationMode | configurationMode | reconfigurationMode | eventMode | continuousTimeMode | stepMode | clockActivationMode | intermediateUpdateMode | terminated );
    static const int setFloat64               = setFloat32;
    static const int setInt8                  = ( instantiated | configurationMode | reconfigurationMode | initializationMode | eventMode | stepMode | clockActivationMode | terminated );
    static const int setUInt8                 = setInt8;
    static const int setInt16                 = setInt8;
    static const int setUInt16                = setInt8;
    static const int setInt32                 = setInt8;
    static const int setUInt32                = setInt8;
    static const int setInt64                 = setInt8;
    static const int setUInt64                = setInt8;
    static const int setBoolean               = setInt8;
    static const int setString                = setInt8;
    static const int setBinary                = setInt8;

    /* Getting Variable Dependency Information */
    static const int getNumberOfVariableDependencies  = setable;
    static const int getVariableDependencies          = setable;

    /* Getting and setting the internal FMU state */
    static const int getFMUState              = setable;
    static const int setFMUState              = setable;
    static const int freeFMUState             = setable;
    static const int serializedFMUStateSize   = setable;
    static const int serializedFMUState       = setable;
    static const int serializeFMUState        = setable;
    static const int deSerializeFMUState      = setable;

    /* Getting partial derivatives */
    static const int getDirectionalDerivative = ( initializationMode | eventMode | continuousTimeMode | terminated );
    static const int getAdjointDerivative     = getDirectionalDerivative;

    /* Entering and exiting the Configuration or Reconfiguration Mode */
    static const int enterConfigurationMode   = ( instantiated | stepMode | eventMode | clockActivationMode );
    static const int exitConfigurationMode    = ( configurationMode | reconfigurationMode );

    /* Clock related functions */
    // TODO: fix masks
    static const int getClock                  = setable;
    static const int setClock                  = setable;
    static const int getIntervalDecimal        = setable;
    static const int getIntervalFraction       = setable;
    static const int setIntervalDecimal        = setable;
    static const int setIntervalFraction       = setable;
    static const int updateDiscreteStates      = eventMode;

    /* Functions for Model Exchange */

    static const int entercontinuousTimeMode       = eventMode;
    static const int completedIntegratorStep       = continuousTimeMode;

    /* Providing independent variables and re-initialization of caching */
    static const int setContinuousStates           = continuousTimeMode;
    static const int setTime                       = ( eventMode | continuousTimeMode );

    /* Evaluation of the model equations */
    static const int getDerivatives                = ( initializationMode | eventMode | continuousTimeMode | terminated );
    static const int getEventIndicators            = getDerivatives;
    static const int getContinuousStates           = getDerivatives;
    static const int getNominalsOfContinuousStates = getDerivatives;
    static const int getNumberOfEventIndicators    = getDerivatives;
    static const int getNumberOfContinuousStates   = getDerivatives;

    /* Functions for Co-Simulation */

    static const int enterStepMode            = ( initializationMode | eventMode );
    static const int setInputDerivatives      = ( instantiated | initializationMode | stepMode );
    static const int getOutputDerivatives     = ( stepMode | stepDiscarded | terminated | error );
    static const int doStep                   = stepMode;
    static const int activateModelPartition   = clockActivationMode;
    static const int doEarlyReturn            = intermediateUpdateMode;
    static const int getDostepDiscardedStatus = stepMode;


    static bool check(
        InstanceBase* impl,
        int mode,
        const char *name
    );
};

#endif // AllowedState_h
