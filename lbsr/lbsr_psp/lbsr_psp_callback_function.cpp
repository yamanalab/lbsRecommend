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
#include <vector>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <lbsr_share/lbsr_packet.hpp>
#include <lbsr_share/lbsr_securekey_filemanager.hpp>
#include <lbsr_share/lbsr_enc_recommendation.hpp>
#include <lbsr_psp/lbsr_psp_callback_function.hpp>
#include <lbsr_psp/lbsr_psp_callback_param.hpp>
#include <lbsr_psp/lbsr_psp_state.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace lbsr_psp
{

// CallbackFunctionRequestConnect

void CallbackFunctionRequestConnect::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received connect request. (current state : %lu)",
                   state.current_state());

    state.set(kEventConnectSocket);
    STDSC_LOG_TRACE("End callback.");
}
DEFINE_DATA_FUNC(CallbackFunctionRequestConnect);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionRequestConnect);

// CallbackFunctionRequestDisconnect

void CallbackFunctionRequestDisconnect::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received disconnect request. (current state : %lu)",
                   state.current_state());

    state.set(kEventDisconnectSocket);
    STDSC_LOG_TRACE("End callback.");
}
DEFINE_DATA_FUNC(CallbackFunctionRequestDisconnect);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionRequestDisconnect);
    
// CallbackFunctionPubkeyRequest

void CallbackFunctionPubkeyRequest::download_function(
  uint64_t code, const stdsc::Socket& sock, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received public key request. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected <= state.current_state(),
      "Warn: must be generatedkeys state to receive pubkey request.");

    auto skm = param_.get_skm();
    stdsc::Buffer pubkey(skm.pubkey_size());
    skm.pubkey_data(pubkey.data());
    STDSC_LOG_INFO("Sending public key.");
    sock.send_packet(stdsc::make_data_packet(lbsr_share::kControlCodeDataPubkey,
                                             skm.pubkey_size()));
    sock.send_buffer(pubkey);
    state.set(kEventReceivedPubkey);
}
DEFINE_REQUEST_FUNC(CallbackFunctionPubkeyRequest);
DEFINE_DATA_FUNC(CallbackFunctionPubkeyRequest);

// CallbackFunctionLocationInfo

void CallbackFunctionLocationInfo::data_function(uint64_t code,
                                                 const stdsc::Buffer& buffer,
                                                 stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received location info. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected <= state.current_state(),
      "Warn: must be generatedkeys state to receive location info.");

    // TODO: Implementation for Location info.

    state.set(kEventReceivedLocationInfo);
}
DEFINE_REQUEST_FUNC(CallbackFunctionLocationInfo);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionLocationInfo);

// CallbackFunctionRecommList

void CallbackFunctionRecommList::data_function(uint64_t code,
                                               const stdsc::Buffer& buffer,
                                               stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received recommendation list. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected <= state.current_state(),
      "Warn: must be generatedkeys state to receive recommendation list.");

    auto skm = param_.get_skm();

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    std::shared_ptr<lbsr_share::EncRecommendation> enc_recommendation_ptr(
      new lbsr_share::EncRecommendation());
    enc_recommendation_ptr->load_from_stream(stream, skm.pubkey_filename());
    param_.enc_recommendation_ptr = enc_recommendation_ptr;

    std::ofstream ofs(param_.enc_recommendation_filename);
    enc_recommendation_ptr->save_to_stream(ofs);

    state.set(kEventReceivedRecommList);
}
DEFINE_REQUEST_FUNC(CallbackFunctionRecommList);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionRecommList);

// CallbackFunctiondResultRequest

void CallbackFunctionResultRequest::download_function(
  uint64_t code, const stdsc::Socket& sock, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received result request. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateDecryptedRecommList <= state.current_state(),
      "Warn: must be decryptedrecommlist state to send results.");

    auto skm = param_.get_skm();

    std::vector<long> results;
    {
        size_t size;
        std::ifstream ifs(param_.enc_recommendation_filename);

        ifs >> size;

        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream sk(skm.seckey_filename());
        ::readContextBase(sk, m, p, r, gens, ords);
        ::FHEcontext context(m, p, r, gens, ords);
        sk >> context;
        ::FHESecKey secretKey(context);
        const ::FHEPubKey& publicKey = secretKey;
        sk >> secretKey;
        sk.close();
        EncryptedArray ea(context);

        std::vector<Ctxt> ctxts(size, Ctxt(publicKey));
        for (size_t i = 0; i < size; i++)
        {
            ifs >> ctxts[i];
        }
        for (size_t i = 0; i < size; i++)
        {
            std::vector<long> res;
            ea.decrypt(ctxts[i], secretKey, res);
            results.push_back(res[0]);
        }
    }

    size_t size = results.size() * sizeof(long);
    stdsc::Buffer buffer(size);
    memcpy(buffer.data(), static_cast<void*>(&results[0]), size);
    sock.send_packet(
      stdsc::make_data_packet(lbsr_share::kControlCodeDataResult, size));
    sock.send_buffer(buffer);
}
DEFINE_REQUEST_FUNC(CallbackFunctionResultRequest);
DEFINE_DATA_FUNC(CallbackFunctionResultRequest);

} /* namespace lbsr_psp */
