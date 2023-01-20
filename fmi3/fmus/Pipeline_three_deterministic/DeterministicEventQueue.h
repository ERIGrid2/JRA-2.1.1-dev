/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef DeterministicEventQueue_h
#define DeterministicEventQueue_h

#include <queue>
#include "PipeInterconnector.h"

namespace DeterministicEventQueue
{
	typedef fmi3Float64 TimeStamp;
	typedef fmi3Float64 Tolerance;
	typedef fmi3Int32 MessageID;
	//typedef fmi3Int32* Receiver;
	//typedef fmi3Clock* ReceiverClock;

	struct Event {

		TimeStamp timeStamp; // Each event is associated with a timestamp.
		MessageID msgId; // Each event is associated with a message ID.
		PipeInterconnector* interconnector; //The interconnector at the end of this pipe
		//Receiver receiver; // Each message ID is associated to an output variable.
		//ReceiverClock clock; // Each output variable is associated to an output clock.

		// Struct constructor.
		Event(
            TimeStamp t,
            MessageID m,
		    PipeInterconnector* i
            //Receiver r,
            //ReceiverClock c
        ) :
            timeStamp( t ),
            msgId( m ),
            interconnector ( i )
            //receiver( r ),
            //clock( c )
        {}
	};

    static Tolerance tolerance = 1e-2;

	// This functor defines that events are sorted in the event queue according to their timestamp.
	struct EventOrder {
		bool operator() (
            const Event* e1,
            const Event* e2
        ) const {
			return e1->timeStamp > e2->timeStamp;// - DeterministicEventQueue::tolerance;
		}
	};

	// This is the definition of the event queue.
	typedef std::priority_queue <Event*,std::vector <Event*>,EventOrder> EventQueue; //min-heap priority queue

    inline void clearpq(std::priority_queue<Event*,std::vector <Event*>,EventOrder>& q) {
        struct HackedQueue : private std::priority_queue<Event*,std::vector <Event*>,EventOrder> {
            static std::vector <Event*>& Container(std::priority_queue<Event*,std::vector <Event*>,EventOrder>& q) {
                return q.*&HackedQueue::c;
            }
        };
        HackedQueue::Container(q).clear();
    }

}

#endif // DeterministicEventQueue_h
