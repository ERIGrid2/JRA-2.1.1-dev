/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef UnpredictableEventStack_h
#define UnpredictableEventStack_h

#include <queue>

namespace UnpredictableEventStack
{
	typedef fmi3Int32 MessageID;
	typedef fmi3Int32* Receiver;
	typedef fmi3Clock* ReceiverClock;

	struct Event {

		MessageID msgId; // Each event is associated with a message ID.
		Receiver receiver; // Each message ID is associated to an output variable.
		ReceiverClock clock; // Each output variable is associated to an output clock.

		// Struct constructor.
		Event(
            MessageID m,
            Receiver r,
            ReceiverClock c
        ) :
            msgId( m ),
            receiver( r ),
            clock( c )
        {}
	};

	// This is the definition of the event queue.
	typedef std::queue<Event*> EventStack;
}

#endif // UnpredictableEventStack_h
