//
// Created by Kimi on 20/06/2015.
//

#ifndef PICODB_EVENTHANDLER_H
#define PICODB_EVENTHANDLER_H

class EventHandler {

public:
    virtual int handleSignal ( int signum ) = 0;
    virtual ~EventHandler () {};
};


#endif //PICODB_EVENTHANDLER_H
