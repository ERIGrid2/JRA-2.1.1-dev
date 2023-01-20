#include "MessagePipeline.h"

using namespace DeterministicEventQueue;

MessagePipeline::MessagePipeline(
    const std::string name
    ) :
    pipeName(name),
    randomSeed_( 1 ),
    randomMean_( 0.5 ),
    randomStdDev_( 0.15 ),
    randomMin_( 0.1 ),
    tolerance_( DeterministicEventQueue::tolerance ),
    nextEventTime_( std::numeric_limits<fmi3Float64>::max() ),
    eventQueue_()//,
    /*currentEvent_( eventQueue_.end() )*/
{
}


fmi3Float64 MessagePipeline::calculateDelay() {
    std::cout << ">><< MessagePipeline::calculateDelay" << std::endl << std::flush;
    // No negative delays!
    return std::max(
        this->distribution_( this->generator_ ),
        this->randomMin_
    );
}

bool MessagePipeline::addNewEvent(
    const TimeStamp& msgReceiveTime,
    const MessageID& msgId,
    PipeInterconnector* interconn
    //const Receiver& receiver,
    //const ReceiverClock& clock
) {
    std::cout << ">> MessagePipeline::addNewEvent" << std::endl << std::flush;
    bool firstEvent = this->eventQueue_.empty();

    // Create new event.
    Event* evt = new Event(
        msgReceiveTime,
        msgId,
        interconn
        //receiver,
        //clock
    );

    this->logDebug(
        "add new event at t = %f. ID = %d", msgReceiveTime, msgId
    );

    // Insert event into queue.
    this->eventQueue_.push(evt);
    //std::pair<EventQueue::iterator ,bool> insertResult = this->eventQueue_.insert( evt );
    bool validEvent = true; //PriQueue is not a Set, so this will always be valid (?)
    //bool validEvent = insertResult.second;

    // Delete the event, because it has not been inserted successfully (another event with
    // the same timestamp alreay exists).
    /*if ( false == validEvent )
    {
        this->logDebug(
            "veto for event at t = %f", msgReceiveTime
        );

        delete evt;
        std::cout << "<< MessagePipeline::addNewEvent" << std::endl << std::flush;
        return false;
    }*/

    if ( firstEvent || ( msgReceiveTime < this->nextEventTime_ ) )
    {
        //this->currentEvent_ = insertResult.first;
        this->nextEventTime_ = msgReceiveTime;
    }

    std::cout << "<< MessagePipeline::addNewEvent" << std::endl << std::flush;
    return true;
}
