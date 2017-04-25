#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_

#include <tools/defines.h>
#include <tools/logged.h>
#include <boost/thread/recursive_mutex.hpp>

SHARED_PTR(Scheduler);
SHARED_PTR(Request);
SHARED_PTR(Controller);

#pragma GCC visibility push(default)

/**
    Scheduler handle complexity of choosing if a controller can execute a request or not, but also when it should execute a request. 

    This scheduler is the most basic one, every request coming that way are allowed to be executed right away. 

    Scheduler MUST be thread safe, as they're going to be executed from multiples threads. 

    Note, it's still the controller's pool that does the execution !
*/
class Scheduler : public Logged {
    ControllerPtr controller;

    std::map<boost::uuids::uuid,RequestPtr> runningRequests;

protected:
    boost::recursive_mutex mutex;

public:
    Scheduler(ControllerPtr);
    virtual ~Scheduler();

    //! @return true if @a request was added to the stack, doesn't means it's going to be executed right away.
    virtual bool addRequest(RequestPtr);
    //! attempt to interrupt any request with provided @a id
    virtual void interruptRequest(RequestPtr);
    //! notification from Controller that a request has completed it's execution. 
    virtual void requestPerformed(RequestPtr);

    //! drop all running requests. 
    virtual void abortAll();

protected:
    
    ControllerPtr getController();

};

#pragma GCC visibility pop

#endif // __SCHEDULER_H_
