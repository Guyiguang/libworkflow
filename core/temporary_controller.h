#ifndef __TEMPORARY_CONTROLLER_H_
#define __TEMPORARY_CONTROLLER_H_
#include <core/controller.h>

#pragma GCC visibility push(default)
SHARED_PTR(TemporaryController);
/**
 This kind of specific controller will simply get the workflow from the request and store it temporarily.
 */
class TemporaryController : public Controller {
public:
    TemporaryController(uint32_t pool, bool delayed_start);
    virtual ~TemporaryController();
    
    bool perform(RequestPtr) override;
    void requestFinished(RequestPtr) override;
};

#pragma GCC visibility pop


#endif // __TEMPORARY_CONTROLLER_H_
