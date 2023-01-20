/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef PipeInterconnector_h
#define PipeInterconnector_h

#include <random>
#include <cstdarg>
#include "fmi3FunctionTypes.h"
#include <string>
#include <iostream>

class PipeInterconnector {

public:
    PipeInterconnector(
		const std::string name,
        bool isoutput
    );

	void reset() {
	}

	void setMessage(fmi3Int32 mid) {
        messageID=mid;
        hasmessage=true;
        std::cout << "Interconnector " << nodeName << ": Setting message=" << mid << std::endl;
    }

    fmi3Clock hasMessage() {
        return hasmessage;
    }

    fmi3Int32 getMessage() {
        if (hasmessage==false) {
            std::cout << "ERROR: Trying to get message but there isn't one." << std::endl;
            return 0;
        }
        hasmessage=false;
        return messageID;
    }

private:

	const std::string nodeName;

    bool isOutput;

    fmi3Clock hasmessage;
    fmi3Int32 messageID;

};

#endif // PipeInterconnector_h
