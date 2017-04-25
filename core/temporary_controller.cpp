#include <core/temporary_controller.h>
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

TemporaryController::TemporaryController(uint32_t pool, bool delayed_start) : Controller("temporary", pool, delayed_start) {}
TemporaryController::~TemporaryController() {
}

bool TemporaryController::perform(RequestPtr req) {
    if(req->getWorkflowJson().empty()) {
        // either it's a reply, or it's an error.
        
        if(req->getTarget().target == ETargetAction::Reply) {
            return Controller::perform(req);
        }
        
        BOOST_LOG_SEV(logger, Error) << req->logRequest() << getName() << " controller can't execute this request, as it doesn't provide a workflow.";
        
        auto reply = Request::createReply(req);
        ErrorReportPtr er = ErrorReportPtr(new ErrorReport());
        er->setError("temporary.empty.workflow", "Empty workflow provided, can't execute request.");
        reply->setErrorReport(er);
        ControllerManager::getInstance()->perform(reply);
        return false;
    }
    WorkflowPtr wkf(new Workflow(""));
    wkf->str_load(req->getWorkflowJson());
    auto er = wkf->getStateMachine()->validate();
    if(er.isSet()) {
        BOOST_LOG_SEV(logger, Error) << req->logRequest() << getName() << " provided workflow isn't valid: " << er.getErrorMessage();
        auto reply = Request::createReply(req);
        ErrorReportPtr erp = ErrorReportPtr(new ErrorReport(er));
        reply->setErrorReport(erp);
        ControllerManager::getInstance()->perform(reply);
    }
    
    auto wkf_prefix = shortId(boost::uuids::random_generator()(), 3);
    wkf->setName(wkf_prefix + wkf->getName());
    req->getTarget().workflow = wkf->getName();
    req->getTarget().controller = "temporary";
    addWorkflow(wkf);
    BOOST_LOG_SEV(logger, Trace) << req->logRequest() << getName() << " adding temporary workflow: " << wkf->getName();
    
    auto res = Controller::perform(req);
    if(not res) {
        BOOST_LOG_SEV(logger, Warn) << req->logRequest() << getName() << " " << wkf->getName() << " Failed to perform request.";
    }
    
    return res;
}

void TemporaryController::requestFinished(RequestPtr req) {
    dropWorkflow(req->getTarget().workflow);
    BOOST_LOG_SEV(logger, Trace) << req->logRequest() << getName() << " dropping temporary workflow: " << req->getTarget().workflow;
}



