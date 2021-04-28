/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef FMUMode_h
#define FMUMode_h

enum FMUMode : int {
    startAndEnd            = 1 << 0,
    configurationMode      = 1 << 1,
    instantiated           = 1 << 2,
    initializationMode     = 1 << 3,
    eventMode              = 1 << 4,
    continuousTimeMode     = 1 << 5, // --> ME only
    stepMode               = 1 << 6,
    clockActivationMode    = 1 << 7, // --> SE only
    stepDiscarded          = 1 << 8,
    reconfigurationMode    = 1 << 9,
    intermediateUpdateMode = 1 << 10,
    terminated             = 1 << 11,
    error                  = 1 << 12,
    fatal                  = 1 << 13,
    setable                = (~0)
};

#endif // FMUMode_h
