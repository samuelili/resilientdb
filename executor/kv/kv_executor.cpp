/*
 * Copyright (c) 2019-2022 ExpoLab, UC Davis
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "executor/kv/kv_executor.h"

#include <glog/logging.h>

#include "proto/kv/kv.pb.h"

namespace resdb {

KVExecutor::KVExecutor(std::unique_ptr<ChainState> state)
    : state_(std::move(state)) {}

std::unique_ptr<std::string> KVExecutor::ExecuteData(
    const std::string& request) {
  KVRequest kv_request;
  KVResponse kv_response;

  if (!kv_request.ParseFromString(request)) {
    LOG(ERROR) << "parse data fail";
    return nullptr;
  }

  if (kv_request.cmd() == KVRequest::SET) {
    Set(kv_request.key(), kv_request.value());
  } else if (kv_request.cmd() == KVRequest::GET) {
    kv_response.set_value(Get(kv_request.key()));
  } else if (kv_request.cmd() == KVRequest::GETALLVALUES) {
    kv_response.set_value(GetAllValues());
  } else if (kv_request.cmd() == KVRequest::GETRANGE) {
    kv_response.set_value(GetRange(kv_request.key(), kv_request.value()));
  }

  std::unique_ptr<std::string> resp_str = std::make_unique<std::string>();
  if (!kv_response.SerializeToString(resp_str.get())) {
    return nullptr;
  }

  return resp_str;
}

void KVExecutor::Set(const std::string& key, const std::string& value) {
  state_->SetValue(key, value);
}

std::string KVExecutor::Get(const std::string& key) {
  return state_->GetValue(key);
}

std::string KVExecutor::GetAllValues() { return state_->GetAllValues(); }

// Get values on a range of keys
std::string KVExecutor::GetRange(const std::string& min_key,
                                 const std::string& max_key) {
  return state_->GetRange(min_key, max_key);
}

}  // namespace resdb
