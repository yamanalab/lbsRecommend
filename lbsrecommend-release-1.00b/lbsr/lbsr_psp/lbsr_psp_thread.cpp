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

#include <sstream>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_utility.hpp>
#include <lbsr_psp/lbsr_psp_thread.hpp>
#include <lbsr_psp/lbsr_psp_state.hpp>

namespace lbsr_psp
{

struct PSPThread::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state, const char* pubkey_filename,
         const char* seckey_filename)
      : server_(new stdsc::Server<>(port, state)),
        state_(state),
        pubkey_filename_(pubkey_filename),
        seckey_filename_(seckey_filename)
    {
        server_->set_callback(callback);
        state_.set(kEventConnectSocket);
        STDSC_LOG_INFO("Lanched PSP server (%s)", port);
    }

    ~Impl(void) = default;

    void start(void)
    {
        if (!lbsr_share::utility::file_exist(pubkey_filename_))
        {
            std::ostringstream oss;
            oss << "Err: public key file not found. (" << pubkey_filename_
                << ")" << std::endl;
            STDSC_THROW_FILE(oss.str());
        }
        if (!lbsr_share::utility::file_exist(seckey_filename_))
        {
            std::ostringstream oss;
            oss << "Err: security key file not found. (" << seckey_filename_
                << ")" << std::endl;
            STDSC_THROW_FILE(oss.str());
        }
        state_.set(kEventGeneratedKeys);
        server_->start();
    }

    void join(void)
    {
        server_->wait_for_finish();
    }

private:
    std::shared_ptr<stdsc::Server<>> server_;
    stdsc::StateContext& state_;
    std::string pubkey_filename_;
    std::string seckey_filename_;
};

PSPThread::PSPThread(const char* port,
                     stdsc::CallbackFunctionContainer& callback,
                     stdsc::StateContext& state, const char* pubkey_filename,
                     const char* seckey_filename)
  : pimpl_(new Impl(port, callback, state, pubkey_filename, seckey_filename))
{
}

void PSPThread::start(void)
{
    pimpl_->start();
}

void PSPThread::join(void)
{
    pimpl_->join();
}

} /* namespace lbsr_psp */
