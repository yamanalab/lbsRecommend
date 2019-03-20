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

#include <fstream>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <lbsr_share/lbsr_packet.hpp>
#include <lbsr_share/lbsr_enc_cooccurrence.hpp>
#include <lbsr_pprs/lbsr_pprs_psp_client.hpp>
#include <lbsr_pprs/lbsr_pprs_state.hpp>

namespace lbsr_pprs
{

struct PSPClient::Impl
{
    Impl(const char* host, const char* port, stdsc::StateContext& state)
      : state_(state)
    {
        STDSC_LOG_TRACE("Connecting %s@%s", host, port);
        client_.connect(host, port);
        state_.set(kEventConnectSocket);
        STDSC_LOG_INFO("Connected %s@%s", host, port);
    }

    ~Impl(void) = default;

    void download_pubkey(const char* out_filename)
    {
        stdsc::Buffer pubkey;
        client_.recv_data_blocking(lbsr_share::kControlCodeDownloadPubkey,
                                   pubkey);

        auto data = reinterpret_cast<const char*>(pubkey.data());
        auto size = pubkey.size();
        STDSC_LOG_INFO("Created a public key file. (%s)", out_filename);

        std::ofstream ofs(out_filename, std::ios::binary);
        ofs.write(data, size);

        state_.set(kEventSendPubkeyRequest);
    }

    void upload_recomm_list(const stdsc::Buffer& buffer)
    {
        client_.send_data_blocking(lbsr_share::kControlCodeDataRecommList,
                                   buffer);
    }

private:
    stdsc::StateContext& state_;
    stdsc::Client client_;
};

PSPClient::PSPClient(const char* host, const char* port,
                     stdsc::StateContext& state)
  : pimpl_(new Impl(host, port, state))
{
}

void PSPClient::download_pubkey(const char* out_filename)
{
    pimpl_->download_pubkey(out_filename);
}

void PSPClient::upload_recomm_list(const stdsc::Buffer& buffer)
{
    pimpl_->upload_recomm_list(buffer);
}

} /* lbsr_pprs */
