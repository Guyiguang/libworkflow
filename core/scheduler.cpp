#include <core/scheduler.h>
#include <core/controller.h>
#include <boost/uuid/uuid.hpp>
#include <core/request.h>

Scheduler::Scheduler(ControllerPtr ctrl): Logged("ctrl.schd"), controller(ctrl) {
}

Scheduler::~Scheduler() {
}

bool Scheduler::addRequest(RequestPtr req) {
    getController()->executeRequest(req);
    boost::recursive_mutex::scoped_lock lock(mutex);
    runningRequests[req->getId()] = req;
    return true;
}

void Scheduler::interruptRequest(RequestPtr req) {
    getController()->interruptRequest(req);
}

void Scheduler::requestPerformed(RequestPtr req) {
    boost::recursive_mutex::scoped_lock lock(mutex);
    runningRequests.erase(req->getId());
}

void Scheduler::abortAll() {
    for(const auto & kv: runningRequests)
        interruptRequest(kv.second);
}

ControllerPtr Scheduler::getController() {
    return controller;
}

OSTREAM_HELPER_IMPL(Scheduler, obj) {
    out << "[Scheduler]";
    return out;
}

