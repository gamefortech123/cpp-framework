/*
 *
 * Copyright 2018 Guolian Zhang.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <rpc/async_server.h>

#include <base/base.h>
#include <logs/logs.hpp>

namespace rpc {

AsyncRpcServer* AsyncRpcServer::rpc_server_ = NULL;

AsyncRpcServer::AsyncRpcServer() {

}
AsyncRpcServer::~AsyncRpcServer() {

}

AsyncRpcServer* AsyncRpcServer::GetInstance() {
    if (rpc_server_ == NULL) {
        rpc_server_ = new AsyncRpcServer();
    }

    return rpc_server_;
}
    
void AsyncRpcServer::Join() {
    if (thread_.get() != NULL) {
        thread_->Join();
    }
}

void AsyncRpcServer::Init(int thread_num) {
    if (thread_.get() == NULL) {
        thread_.reset(new async::Thread(thread_num));
    }
}

void AsyncRpcServer::Run(const std::string& host, int port) {
    std::string server_address = base::StringPrintf("%s:%d", host.c_str(), port);
    builder_.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    cq_ = builder_.AddCompletionQueue();
    server_ = builder_.BuildAndStart();

    for (size_t i = 0; i < init_call_data_list_.size(); i++) {
        init_call_data_list_[i]->Proceed();
    }
    init_call_data_list_.clear();

    LOG_INFO("AsyncRpcServer Run, listening on %s.", server_address.c_str());

    thread_->PostTask(boost::bind(&AsyncRpcServer::HandleRpcs, this));
}

void AsyncRpcServer::RegisterService(grpc::Service* service) {
    builder_.RegisterService(service);
}

void AsyncRpcServer::AddInitCallData(CallData* call_data) {
    init_call_data_list_.push_back(call_data);
}

grpc::ServerCompletionQueue* AsyncRpcServer::cq() {
    return cq_.get();
}

void AsyncRpcServer::HandleRpcs() {
    void* tag;  // uniquely identifies a request.
    bool ok;
    while (true) {
        // Block waiting to read the next event from the completion queue. The
        // event is uniquely identified by its tag, which in this case is the
        // memory address of a CallData instance.
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or cq_ is shutting down.
        GPR_ASSERT(cq_->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<CallData*>(tag)->Proceed();
    }
}

} // namespace rpc

