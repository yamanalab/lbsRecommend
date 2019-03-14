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
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_enc_preference.hpp>
#include <lbsr_share/lbsr_packet.hpp>
#include <lbsr_client/lbsr_client_pprs_client.hpp>

namespace lbsr_client
{
template <class T>
struct PPRSClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port)
    {
        te_ = stdsc::ThreadException::create();
        STDSC_LOG_INFO("Connecting to PPRS.");
        client_.connect(host, port);
    }

    ~Impl(void)
    {
        client_.close();
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            lbsr_share::EncPreference enc_preference;
            enc_preference.generate(args.input_filename, args.pubkey_filename);

            stdsc::BufferStream buffstream(enc_preference.stream_size());
            std::iostream stream(&buffstream);
            enc_preference.save_to_stream(stream);

            stdsc::Buffer* buffer = &buffstream;
            client_.send_data_blocking(
              lbsr_share::kControlCodeDataUserPreference, *buffer);
            client_.send_request_blocking(
              lbsr_share::kControlCodeRequestCompute);
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to client process (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    stdsc::Client client_;
};

template <class T>
PPRSClient<T>::PPRSClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

template <class T>
PPRSClient<T>::~PPRSClient(void)
{
}

template <class T>
void PPRSClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void PPRSClient<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void PPRSClient<T>::exec(T& args,
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

template class PPRSClient<PPRSParam>;

} /* namespace lbsr_client */
