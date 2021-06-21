/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef ConfigurableEventQueue_h
#define ConfigurableEventQueue_h

#include <set>

namespace ConfigurableEventQueue
{
	typedef fmi3Float64 TimeStamp;
	typedef fmi3Float64 Tolerance;
	typedef fmi3Int32 MessageID;
	typedef fmi3Int32* Receiver;
	typedef fmi3Clock* ReceiverClock;
    
	struct Event {

		TimeStamp timeStamp; // Each event is associated with a timestamp.
		MessageID msgId; // Each event is associated with a message ID.
		Receiver receiver; //TODO: to be removed
        ReceiverClock clock; //TODO: to be removed

		// Struct constructor.
		Event(
            TimeStamp t,
            MessageID m,
            Receiver r,
            ReceiverClock c
        ) :
            timeStamp( t ),
            msgId( m ),
            receiver( r ),
            clock( c )
        {}
	};

    static Tolerance tolerance = 1e-9;

	// This functor defines that events are sorted in the event queue according to their timestamp.
	struct EventOrder {
		bool operator() (
            const Event* e1,
            const Event* e2
        ) const {
			return e1->timeStamp < e2->timeStamp - ConfigurableEventQueue::tolerance;
		}
	};

	// This is the definition of the event queue.
	typedef std::set<Event*, EventOrder> EventQueue;
}

#endif // ConfigurableEventQueue_h
