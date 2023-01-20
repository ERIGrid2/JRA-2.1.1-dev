/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef MessagePipeline_h
#define MessagePipeline_h

#include <random>
#include <cstdarg>
#include "fmi3FunctionTypes.h"
#include "DeterministicEventQueue.h"
#include "PipeInterconnector.h"
#include <string>
#include <iostream>

class MessagePipeline {

public:
    MessagePipeline(
		const std::string name
    );

    void setTolerance(fmi3Float64 tolerance) {
		tolerance_=tolerance;
	}

	void initialize() {
	    // Random generator seed has to be a positive non-zero integer.
	    if ( 1 > randomSeed_ ) {
            randomSeed_ = 1;
        }
        // Set random generator seed.
        generator_.seed( randomSeed_ );
        // Initialize random number distribution.
        distribution_ = std::normal_distribution<fmi3Float64>(randomMean_,randomStdDev_);
	}

	void reset() {
		DeterministicEventQueue::clearpq(eventQueue_);
	    //eventQueue_.clear();
        //currentEvent_ = eventQueue_.end();
        nextEventTime_ = std::numeric_limits<fmi3Float64>::max();
	}

	void insertMessage(fmi3Float64 synctime, fmi3Int32 inchannel, PipeInterconnector* interconnector /*fmi3Int32* outchannel, fmi3Clock* outclock*/) {
		fmi3Float64 delay;
        // The event queue can only contain one event per timestamp. Since there is
        // a (very small) chance that we generate a new random event with an already
        // existing timestamp, insertion of new event is repeated until a new event
        // has been inserted successfully.
        do
        {
            delay = calculateDelay();
        }
        while( false == addNewEvent(synctime + delay,inchannel, interconnector /*outchannel, outclock*/));
	}

	void setRandomSeed(fmi3Int32 s) {randomSeed_=s;}
	void setRandomMean(fmi3Float64 s) {randomMean_=s;}
	void setRandomStdDev(fmi3Float64 s) {randomStdDev_=s;}
	void setRandomMin(fmi3Float64 s) {randomMin_=s;}

	bool didWeOverstep(fmi3Float64 syncTime) {
		return (syncTime > (nextEventTime_-tolerance_));
	}

	bool didWeReach(fmi3Float64 syncTime) {
		return ( fabs(syncTime-nextEventTime_) <= tolerance_);
	}

	bool isQueueCorrupt() {
		 //return (currentEvent_ == eventQueue_.end());
		return false; //TODO: What's the equivalent of the previous check?
	}

	fmi3Float64 processQueue(fmi3Float64 synctime, fmi3Boolean* nextEventTimeDefined) {
		std::cout << ">> MessagePipeline::processQueue (pipeline " << pipeName << ")" << std::endl << std::flush;
		/* This uses a weird mix of std::set methods and pointer acrobatics. It should be rewritten to use std container access only! */
	    //if ( currentEvent_ != std::prev( eventQueue_.end())) {
		if ( eventQueue_.empty()) {
			std::cout << "   event queue is empty" << std::endl << std::flush;
            nextEventTime_ = std::numeric_limits<fmi3Float64>::max();
        }
		// We have not yet reached the next event, return the next event's time.
        else if ( synctime < nextEventTime_-tolerance_ ){
			//std::cout << "   not yet reached next event, time=" << (*currentEvent_) ->timeStamp << std::endl << std::flush;
			nextEventTime_ = eventQueue_.top()->timeStamp;
			std::cout << "   not yet reached next event, time=" << nextEventTime_ << std::endl << std::flush;
            //nextEventTime_ = (*currentEvent_) ->timeStamp;
            *nextEventTimeDefined = fmi3True;
        }
        // We are processing the current event, increment event if possible.
        else if ( fabs( synctime-nextEventTime_ ) <= tolerance_ ) {
            // There is a next event in the schedule --> increment current event and set
            // time of this event as next event time.
			if ( !eventQueue_.empty()) {
            //if ( currentEvent_ != std::prev( eventQueue_.end())) {
				std::cout << "   processing next event. empty=" << ( eventQueue_.empty()) << std::endl << std::flush;
				//eventQueue_.pop();
                //++( currentEvent_ );
				std::cout << "   processing next event. empty=" << ( eventQueue_.empty()) << std::endl << std::flush;
                //nextEventTime_ = (*currentEvent_)->timeStamp;
				nextEventTime_ = eventQueue_.top()->timeStamp;
                *nextEventTimeDefined = fmi3True;
                logDebug("set next event time to t = %f",nextEventTime_);
            }
            else { // There is NO next event in the schedule, next event time is undefined.
	            nextEventTime_ = std::numeric_limits<fmi3Float64>::max();
				*nextEventTimeDefined = fmi3False;
				std::cout << "   no next event defined. empty=" << ( eventQueue_.empty()) << std::endl << std::flush;
				logDebug("no next event defined");
            }
        }
        std::cout << "<< MessagePipeline::processQueue (pipeline " << pipeName << ")" << std::endl << std::flush;
        return nextEventTime_;
	}

	//TODO: This pushes the event directly out to the receiver. How do we intercept it
	void processInternalEvent() {
		eventQueue_.top()->interconnector->setMessage(eventQueue_.top()->msgId);
		eventQueue_.pop();
		//(*currentEvent_)->interconnector->setMessage(( *currentEvent_ )->msgId);
        /**( *currentEvent_ )->receiver = ( *currentEvent_ )->msgId;
        *( *currentEvent_ )->clock = fmi3ClockActive;*/
    }

	fmi3Float64 getNextEventTime() {return nextEventTime_;}

private:

	const std::string pipeName;

	fmi3Float64 calculateDelay();

	bool addNewEvent(  // adds new events to the event queue.
        const DeterministicEventQueue::TimeStamp& msgReceiveTime,
        const DeterministicEventQueue::MessageID& msgId,
		PipeInterconnector* interconn
        //const DeterministicEventQueue::Receiver& receiver,
        //const DeterministicEventQueue::ReceiverClock& clock
    );

    // Event queue.
	DeterministicEventQueue::EventQueue eventQueue_;
	//DeterministicEventQueue::EventQueue::iterator currentEvent_;

	// Random generator (Gaussian);
    std::default_random_engine generator_;
    std::normal_distribution<fmi3Float64> distribution_;

	fmi3Float64 tolerance_; //precision for detecting events

	// Time of the next scheduled event.
	fmi3Float64 nextEventTime_;


	fmi3Int32 randomSeed_;
	fmi3Float64 randomMean_;
	fmi3Float64 randomStdDev_;
    fmi3Float64 randomMin_;

	void logMessage (fmi3Status status,const char *category,const char *message,va_list args) {
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

        std::cout << "=== [" << category << "] " << pipeName << "/" << buf << std::endl;
    }

    void logDebug(const char *message,...) {
        va_list args;
        va_start( args, message );
        logMessage( fmi3OK, "DEBUG", message, args );
        va_end( args );
    }

};

#endif // MessagePipeline_h
