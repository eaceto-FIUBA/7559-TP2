//
// Created by Kimi on 20/06/2015.
//

#ifndef PICODB_SIGNALHANDLER_H
#define PICODB_SIGNALHANDLER_H

#include <signal.h>
#include <stdio.h>
#include <memory.h>

#include "EventHandler.h"

class SignalHandler {

private:
    static SignalHandler *instance;
    static EventHandler *signal_handlers[NSIG];

    SignalHandler(void);

    static void dispatcher(int signum);

public:
    static SignalHandler *getInstance();

    static void destruir();

    EventHandler *registrarHandler(int signum, EventHandler *eh);

    int removerHandler(int signum);

};


#endif //PICODB_SIGNALHANDLER_H
