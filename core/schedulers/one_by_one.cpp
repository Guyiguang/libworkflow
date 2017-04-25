#include <core/schedulers/one_by_one.h>
#include <core/request.h>

namespace Schedulers {

    OneByOne::OneByOne(ControllerPtr c): Scheduler(c) {
    }
    OneByOne::~OneByOne() {
    }

    bool OneByOne::addRequest(RequestPtr request) {
        boost::recursive_mutex::scoped_lock lock(mutex);
        programmed.push_back(request->getId());
        known_requests[request->getId()] = request;
        checkCanExecute();
    }

    void OneByOne::interruptRequest(RequestPtr request) {
        boost::recursive_mutex::scoped_lock lock(mutex);
        if(known_requests.count(request->getId())) {
            programmed.remove_if([request](const boost::uuids::uuid & id) {
                return id == request->getId();
            });
            known_requests.erase(request->getId());
            if(runningRequests.count(request->getId()))
                Scheduler::interruptRequest(request);
        }
    }

    void OneByOne::requestPerformed(RequestPtr request) {
        Scheduler::requestPerformed(request);
        boost::recursive_mutex::scoped_lock lock(mutex);
        if(known_requests.count(request->getId())) {
            programmed.remove_if([request](const boost::uuids::uuid & id) {
                return id == request->getId();
            });
            known_requests.erase(request->getId());
        }
    }

    void OneByOne::checkCanExecute() {
        boost::recursive_mutex::scoped_lock lock(mutex);
        if(runningRequests.size() == 0) {
            auto req = known_requests[programmed.front()];
            Scheduler::addRequest(req);
            programmed.pop_front();
        }
    }
};

std::ostream& operator<<(std::ostream& out, OneByOnePtr obj) { 
out << *obj; 
return out; 
} 

std::ostream& operator<<(std::ostream& out, Schedulers::OneByOne * obj) { 
    out << *obj; 
    return out; 
} 

std::ostream& operator<<(std::ostream& out, const Schedulers::OneByOne & obj) {
    out << "[Scheduler OneByOne]"; 
    return out;
}

