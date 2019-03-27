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

#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_packet.hpp>
#include <lbsr_share/lbsr_define.hpp>
#include <lbsr_client/lbsr_client_psp_client.hpp>

namespace lbsr_client
{
template <class T>
struct PSPClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port) : host_(host), port_(port)
    {
        te_ = stdsc::ThreadException::create();
    }

    void get_result(std::vector<long>& results)
    {
        results.resize(results_.size());
        std::copy(results_.begin(), results_.end(), results.begin());
    }

    void download_pubkey(const char* out_filename)
    {
        stdsc::Buffer pubkey;
        client_.recv_data_blocking(lbsr_share::kControlCodeDownloadPubkey,
                                   pubkey);

        auto data = reinterpret_cast<const char*>(pubkey.data());
        auto size = pubkey.size();
        STDSC_LOG_INFO("Saved a public key file. (%s)", out_filename);

        std::ofstream ofs(out_filename, std::ios::binary);
        ofs.write(data, size);
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            constexpr uint32_t retry_interval_usec = LBSR_RETRY_INTERVAL_USEC;
            constexpr uint32_t retry_interval_usec_to_request_result = 4000000;
            constexpr uint32_t timeout_sec = LBSR_TIMEOUT_SEC;

            STDSC_LOG_INFO("Connecting to PSP.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to PSP.");

            STDSC_LOG_INFO("Requesting connect to PSP.");
            client_.send_request_blocking(
              lbsr_share::kControlCodeRequestConnect, retry_interval_usec,
              timeout_sec);

            download_pubkey(args.pubkey_filename.c_str());

            stdsc::Buffer dummy_locinfo_buffer(16);
            client_.send_data_blocking(lbsr_share::kControlCodeDataLocationInfo,
                                       dummy_locinfo_buffer,
                                       retry_interval_usec, timeout_sec);

            stdsc::Buffer result_buffer;
            client_.recv_data_blocking(lbsr_share::kControlCodeDownloadResult,
                                       result_buffer,
                                       retry_interval_usec_to_request_result);

            size_t n = result_buffer.size() / sizeof(long);
            results_.resize(n);
            memcpy(static_cast<void*>(&results_[0]), result_buffer.data(),
                   result_buffer.size());

            STDSC_LOG_INFO("Requesting disconnect to PSP.");
            client_.send_request_blocking(
              lbsr_share::kControlCodeRequestDisconnect, retry_interval_usec,
              timeout_sec);
            client_.close();
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to client process (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    const char* host_;
    const char* port_;
    stdsc::Client client_;
    std::vector<long> results_;
};

template <class T>
PSPClient<T>::PSPClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

template <class T>
PSPClient<T>::~PSPClient(void)
{
    super::join();
}

template <class T>
void PSPClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void PSPClient<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void PSPClient<T>::get_result(std::vector<long>& results)
{
    this->wait_for_finish();
    pimpl_->get_result(results);
}

template <class T>
void PSPClient<T>::exec(T& args,
                        std::shared_ptr<stdsc::ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

template class PSPClient<PSPParam>;

} /* namespace lbsr_client */
