#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include <tools/active_object.h>
#include <tools/defines.h>
#include <core/workflow.h>

#pragma GCC visibility push(default)
SHARED_PTR(Controller);
SHARED_PTR(ControllerSpawn);
SHARED_PTR(Workflow);
SHARED_PTR(Scheduler);

/**
    Stores workflows, can add some meta data to requests.
    You may add here some scheduling mecanism or request perform control
 */
class Controller: public ActiveObject{
    std::map<std::string, WorkflowPtr> workflows;
    SchedulerPtr scheduler;
    //! allow not to expose unnecessary informations ... 
    friend class Scheduler;
    friend class Workflow;

public:
    Controller(const std::string & name, uint32_t pool, bool delay_start = false);
    virtual ~Controller();
    
    //! cast helper :)
    ControllerPtr shared_from_this();
    //! try to find an appropriate workflow to execute this request.
    virtual bool perform(RequestPtr);

    //! by default simply add an empty struct, but can be used to add some meta data.
    //! Note, default add only data to DefaultAction kind of request.
    virtual ControllerSpawnPtr spawnForRequest(RequestPtr);
    
    
    //! Well retrieve requested workflow.
    WorkflowPtr getWorkflow(const std::string &) const;
    
    //! register a workflow in this controller
    bool addWorkflow(WorkflowPtr);
    
    //! drop @a key workflow
    void dropWorkflow(const std::string & key);
    
    const std::map<std::string, WorkflowPtr> & getWorkflows() const;

    void setScheduler(SchedulerPtr);

protected:
    //! this is used for a back propagation of request execution.
    //! Called by workflow only.
    virtual void requestFinished(RequestPtr);

    //! Execute a request.
    //! Called by Scheduler
    virtual void executeRequest(RequestPtr);

    //! Order request interruption
    //! Called by Scheduler.
    virtual void interruptRequest(RequestPtr);

    //! ensure appropriate init of scheduler.
    void started() override;
};

OSTREAM_HELPER_DECL(Controller);

#pragma GCC visibility pop

#endif // __CONTROLLER_H_
