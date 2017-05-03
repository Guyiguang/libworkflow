#ifndef __ONE_BY_ONE_H_
#define __ONE_BY_ONE_H_

#include <tools/defines.h>
#include <core/scheduler.h>
#include <list>
#include <map>

SHARED_PTR(Controller);

#pragma GCC visibility push(default)


namespace Schedulers{
    class OneByOne;
};

typedef boost::shared_ptr<Schedulers::OneByOne> OneByOnePtr;

namespace Schedulers {
    class OneByOne: public Scheduler {
        std::list<boost::uuids::uuid> programmed;
        std::map<boost::uuids::uuid, RequestPtr> known_requests;
    public:
        OneByOne(ControllerPtr);
        virtual ~OneByOne();

        bool addRequest(RequestPtr) override;
        void interruptRequest(RequestPtr) override;
        void requestPerformed(RequestPtr) override;

    protected:
        void checkCanExecute();
    };

};

std::ostream& operator<<(std::ostream&, const Schedulers::OneByOne &); 
std::ostream& operator<<(std::ostream&, Schedulers::OneByOne *); 
std::ostream& operator<<(std::ostream&, OneByOnePtr); 

#pragma GCC visibility pop

#endif // __ONE_BY_ONE_H_
