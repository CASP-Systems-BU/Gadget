//
// Created by fl on 12/25/20.
//

#include "plg_faster.h"
#include <sstream>

bool assert_opr_success(bool eql, double latency){
    if(!eql){
        throw latency;
    }
    return(eql);
}
//typedef FASTER::environment::QueueIoHandler handler_t;
//typedef FASTER::device::FileSystemDisk<handler_t, 1073741824ull> disk_t;
//using store_t = FasterKv<plg_faster::Key, plg_faster::Value, disk_t>;

int plg_faster::opendb(){
    store_guid = store.StartSession();

//    // Recover
//    uint32_t version;
//    std::vector<Guid> session_ids;
//    Guid token;
//    store.Recover(token, token, version, session_ids);

//    Guid session_id = session_ids[0];
//    // Register with thread
//    uint64_t sno = store.ContinueSession(session_id);

    printf("faster opened \n");
    return(0);
}

int plg_faster::closedb(){
//    auto callback = [](Status result, uint64_t persistent_serial_num) {
//        if(result != Status::Ok) {
//            printf("FASTER checkpoint failed\n");
//        } else {
//            printf("FASTER checkpoint successed\n");
//        }
//    };
//    Guid token;
//    bool success = store.Checkpoint(nullptr, callback, token);

//  disable WAL and checkpoint
//  FASTER does not have WAL:  https://microsoft.github.io/FASTER/docs/fasterkv-basics/#overall-summary
    store.CompletePending(true);
    store.StopSession();
    return(0);
}

double plg_faster::opr_insert(uint64_t ikey, const char* val){
    int vlen = strlen(val);
    auto tp0 = std::chrono::high_resolution_clock::now();

    auto callback = [](IAsyncContext* ctxt, Status result) {
        CallbackContext<UpsertContext> context{ ctxt };
    };
    UpsertContext context{ ikey, VALUESIZE};

    Status iresult = store.Upsert(context, callback, 1);
    if(iresult==Status::Pending) {                          //TODO: whether do pending?
        bool pendingres = store.CompletePending(true);
        if (pendingres)
            iresult = Status::Ok;
    }
    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //printf("Status== %d\n", iresult);
    //assert_opr_success(Status::Ok==iresult, tps);
    return(tps);
}

double plg_faster::opr_update(uint64_t ikey, const char* val){    //manually get then put
    int vlen = strlen(val);
    auto ucallback = [](IAsyncContext* ctxt, Status result) {
        CallbackContext<UpsertContext> context{ ctxt };
    };
    UpsertContext ucontext{ ikey, VALUESIZE};
    auto rcallback = [](IAsyncContext* ctxt, Status result) {
        CallbackContext<ReadContext> context{ ctxt };
    };
    ReadContext rcontext{ikey};

    auto tp0 = std::chrono::high_resolution_clock::now();
    Status rresult = store.Read(rcontext, rcallback, 1);
    if(rresult==Status::Pending) {                          //TODO: whether do pending?
        bool pendingres = store.CompletePending(true);
        if (pendingres)
            rresult = Status::Ok;
    }
    Status uresult = store.Upsert(ucontext, ucallback, 1);
    if(uresult==Status::Pending) {                          //TODO: whether do pending?
        bool pendingres = store.CompletePending(true);
        if (pendingres)
            uresult = Status::Ok;
    }
    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //printf("Status== %d\n", rresult);
    //printf("Status== %d\n", uresult);
    //assert_opr_success(Status::Ok==rresult, tps);
    //assert_opr_success(Status::Ok==uresult, tps);
    return(tps);
}

double plg_faster::opr_read(uint64_t ikey){
    auto tp0 = std::chrono::high_resolution_clock::now();

    auto callback = [](IAsyncContext* ctxt, Status result) {
        CallbackContext<ReadContext> context{ ctxt };
    };
    ReadContext context{ikey};

    Status _result = store.Read(context, callback, 1);
    if(_result==Status::Pending) {
        bool _pendingres = store.CompletePending(true);
        if (_pendingres)
            _result = Status::Ok;
    }
    // new version
    if(_result == Status::Ok) {
        uint32_t oldValueSize = context.output_length;
        char buffer[oldValueSize];
        std::memcpy(buffer, context.val, oldValueSize);
    }
    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //printf("Status== %d\n", _result);
    //assert_opr_success(Status::Ok==_result, tps);
        //std::cout<<"faster read "<<ikey<<"  "<<context.output_length<<"____"<<context.output_pt1<<","<<context.output_pt2<<"\n";              //DEBUG
    return(tps);
}

double plg_faster::opr_delete(uint64_t ikey){
    auto tp0 = std::chrono::high_resolution_clock::now();

    auto callback = [](IAsyncContext* ctxt, Status result) {
        CallbackContext<DeleteContext> context{ctxt};
        //ASSERT_TRUE(false);
    };
    DeleteContext context{ ikey };

    Status _dresult = store.Delete(context, callback, 1);
    if(_dresult==Status::Pending) {
        bool _pendingres = store.CompletePending(true);
        if (_pendingres)
            _dresult = Status::Ok;
    }

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //assert_opr_success(Status::Ok==_dresult, tps);
    return(tps);
}

double plg_faster::opr_merge(uint64_t ikey, const char* val){    //build-in RMW operation
    int vlen = strlen(val);
    auto tp0 = std::chrono::high_resolution_clock::now();

    auto callback = [](IAsyncContext* ctxt, Status result) {
        CallbackContext<RmwContext> context{ ctxt };
    };

    RmwContext mcontext{ ikey, 1, VALUESIZE};
    Status _mresult = store.Rmw(mcontext, callback, 1);
    if(_mresult==Status::Pending) {
        bool _pendingres = store.CompletePending(true);
        if (_pendingres)
            _mresult = Status::Ok;
    }

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //assert_opr_success(Status::Ok==_mresult, tps);
    return(tps);
}
