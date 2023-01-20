#include "PipeInterconnector.h"

PipeInterconnector::PipeInterconnector(
    const std::string name,
    bool isoutput
    ) :
    nodeName(name),
    isOutput(isoutput),
    hasmessage(false),
    messageID(0)
    {}
