#include <core/scheduler.h>
#include <core/controller.h>
#include <boost/uuid/uuid.hpp>
#include <core/request.h>
#include <boost/interprocess/sync/scoped_lock.hpp>

Scheduler::Scheduler(ControllerPtr ctrl): Logged("ctrl.schd"), controller(ctrl) {
}

Scheduler::~Scheduler() {
}

bool Scheduler::addRequest(RequestPtr req) {
    getController()->executeRequest(req);
    runningRequests[req->getId()] = req;
    return true;
}

void Scheduler::interruptRequest(RequestPtr req) {
    getController()->interruptRequest(req);
}

void Scheduler::requestPerformed(RequestPtr req) {
    runningRequests.erase(req->getId());

}

void Scheduler::abortAll() {
    for(const auto & kv: runningRequests)
        interruptRequest(kv.second);
}

ControllerPtr Scheduler::getController() {
    return controller;
}
