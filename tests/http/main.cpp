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

#include <http/http.h>
#include <crash/crash.h>
#include <logs/logs.hpp>

class HttpTest : public http::HttpRequest::Delegate {
public:
    void OnHttpRequestComplete(std::shared_ptr<http::HttpRequest> request) override
    {
        std::cout << "OnRequestComplete" << std::endl;
        std::cout << "res:" << request->response() << std::endl;
    }

    void Start()
    {
        LOG_INFO("request start");
        request_.reset(new http::HttpRequest(
            http::HttpRequest::HttpMode::POST,
            "http://www.baidu.com", 
            this));
        START_HTTP_REQUEST(request_);
    }

    std::shared_ptr<http::HttpRequest> request_;
};

int main() {
    INIT_CRASH_SIGNAL();
    HTTP_INIT();

    HttpTest http_test;
    http_test.Start();

    std::vector<HttpTest> http_test_list;
    for (size_t i = 0; i < 10000; i++) {
        http_test_list.push_back(http_test);
    }

    while (true) {
        for (size_t i = 0; i < http_test_list.size(); i++) {
            http_test_list[i].Start();
            usleep(100000);
        }       
    }

    return 0;
}