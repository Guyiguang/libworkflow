#include <core/controller.h>
#include <core/workflow.h>
#include <core/controller_spawn.h>
#include <core/state_machine.h>
#include <core/request.h>
#include <core/target.h>
#include <tools/error_report.h>
#include <service/controller_manager.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid.hpp>
#include <iostream>
#include <core/scheduler.h>

Controller::Controller(const std::string & name, uint32_t pool, bool delay_start) :
    ActiveObject(name, pool, delay_start), Logged("ctrl"){
}

Controller::~Controller() {
    workflows.clear();
}

void Controller::started() {
    scheduler.reset(new Scheduler(shared_from_this()));
}   

void Controller::setScheduler(SchedulerPtr schd) {
    scheduler->abortAll();
    scheduler = schd;
}

bool Controller::perform(RequestPtr request) {
    if(workflows.count(request->getTarget().workflow) > 0 ) {
        request->setControllerSpawn(spawnForRequest(request));
        return true;
    }
    return false;
}

ControllerPtr Controller::shared_from_this() {
    return boost::dynamic_pointer_cast<Controller>(ActiveObject::shared_from_this());
}

ControllerSpawnPtr Controller::spawnForRequest(RequestPtr) {
    return ControllerSpawnPtr(new ControllerSpawn());
}

bool Controller::addWorkflow(WorkflowPtr workflow) {
    BOOST_LOG_SEV(logger, Info) << getName() << " Add workflow : " << workflow;
    // check first if workflow is "addable"
    
    auto er = workflow->getStateMachine()->validate();
    if(er.isSet()) {
        BOOST_LOG_SEV(logger, Error) << getName() << " Can't add provided workflow " << workflow->getName() << " due to : " << er.getErrorMessage();
        return false;
    }
    
    workflow->setController(shared_from_this());
    workflows[workflow->getName()] = workflow;
    return true;
}

void Controller::dropWorkflow(const std::string & key) {
    workflows.erase(key);
}

WorkflowPtr Controller::getWorkflow(const std::string & name) const {
    if(workflows.count(name) > 0 ) {
        return workflows.at(name);
    }
    return WorkflowPtr();
}

void Controller::requestFinished(RequestPtr rq) {
    scheduler->requestPerformed(rq);
}

void Controller::executeRequest(RequestPtr rq) {
    getIOService()->post(boost::bind(&Workflow::perform, workflows[rq->getTarget().workflow],rq));
}

void Controller::interruptRequest(RequestPtr rq) {
    // ensure request is meant to interrupt
    rq->getTarget().target = ETargetAction::Interrupt;

    getIOService()->post(boost::bind(&Workflow::perform, workflows[rq->getTarget().workflow],rq));
}

const std::map<std::string, WorkflowPtr> & Controller::getWorkflows() const {
    return workflows;
}
OSTREAM_HELPER_IMPL(Controller, obj) {
    out << "[Controller] name: " << obj.getName() << ", pool: " << obj.getPoolSize();
    return out;
}
