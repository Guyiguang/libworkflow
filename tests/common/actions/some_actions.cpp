#include <tests/common/actions/some_actions.h>
#include <tests/common/contexts/some_context.h>


SomeProducer::SomeProducer(): Action("SomeProducer") {
    defineOutput("some_data", CHECKER(SomeContext));
}

Result SomeProducer::perform(SessionPtr session) const {
    auto ctx = SomeContextPtr(new SomeContext());
    ctx->some_value = 42;
    setOutput(session, "some_data", ctx);
    return done();
}


SomeConsummer::SomeConsummer(): Action("SomeConsummer") {
    defineInput("some_data", CHECKER(SomeContext));
}

Result SomeConsummer::perform(SessionPtr session) const {
    // note: we don't need to check for existence, as it's a mandatory input.
    auto input = getCastedInput<SomeContext>(session, "some_data");
    BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " Found target number: " << input->some_value;
    return done();
}


SomeAllowed::SomeAllowed(): Action("SomeAllowed") {
    defineInput("some_data", CHECKER(SomeContext), false);
}

Result SomeAllowed::perform(SessionPtr session) const {
    // note: we don't need to check for existence, as it's a mandatory input.
    auto input = getCastedInput<SomeContext>(session, "some_data");
    if(input) {
    BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " Found Optionnal target number: " << input->some_value;
    } else {
        BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " No data provided continuing." ;
    }
    return done();
}
