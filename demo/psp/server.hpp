/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_state.hpp>
#include <lbsr_share/lbsr_packet.hpp>
#include <lbsr_psp/lbsr_psp_callback_function.hpp>
#include <lbsr_psp/lbsr_psp_callback_param.hpp>
#include <lbsr_psp/lbsr_psp_thread.hpp>
#include <share/define.hpp>

namespace lbsr_demo
{

struct Server
{
    Server(lbsr_psp::CallbackParam& param, stdsc::StateContext& state,
           const char* port = PSP_PORT_FOR_USER,
           bool is_generate_securekey = false)
    {
        stdsc::CallbackFunctionContainer callback;

        std::shared_ptr<stdsc::CallbackFunction> cb_conn(
          new lbsr_psp::CallbackFunctionRequestConnect(param));
        callback.set(lbsr_share::kControlCodeRequestConnect, cb_conn);

        std::shared_ptr<stdsc::CallbackFunction> cb_disconn(
          new lbsr_psp::CallbackFunctionRequestDisconnect(param));
        callback.set(lbsr_share::kControlCodeRequestDisconnect, cb_disconn);

        std::shared_ptr<stdsc::CallbackFunction> cb_pubkey(
          new lbsr_psp::CallbackFunctionPubkeyRequest(param));
        callback.set(lbsr_share::kControlCodeDownloadPubkey, cb_pubkey);

        std::shared_ptr<stdsc::CallbackFunction> cb_locinfo(
          new lbsr_psp::CallbackFunctionLocationInfo(param));
        callback.set(lbsr_share::kControlCodeDataLocationInfo, cb_locinfo);

        std::shared_ptr<stdsc::CallbackFunction> cb_recomm(
          new lbsr_psp::CallbackFunctionRecommList(param));
        callback.set(lbsr_share::kControlCodeDataRecommList, cb_recomm);

        std::shared_ptr<stdsc::CallbackFunction> cb_result(
          new lbsr_psp::CallbackFunctionResultRequest(param));
        callback.set(lbsr_share::kControlCodeDownloadResult, cb_result);

        psp_ = std::make_shared<lbsr_psp::PSPThread>(
          port, callback, state, param.get_skm(), is_generate_securekey);
    }

    ~Server(void) = default;

    void start(void)
    {
        psp_->start();
    }

    void join(void)
    {
        psp_->join();
    }

private:
    std::shared_ptr<lbsr_psp::PSPThread> psp_;
};

} /* namespace lbsr_demo */

#endif /* SERVER_HPP */
