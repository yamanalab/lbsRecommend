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
#include <iostream>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <lbsr_share/lbsr_enc_preference.hpp>
#include <lbsr_share/lbsr_enc_recommendation.hpp>
#include <lbsr_share/lbsr_enc_cooccurrence.hpp>
#include <lbsr_pprs/lbsr_pprs_callback_function.hpp>
#include <lbsr_pprs/lbsr_pprs_callback_param.hpp>
#include <lbsr_pprs/lbsr_pprs_state.hpp>
#include <lbsr_pprs/lbsr_pprs_psp_client.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace lbsr_pprs
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

// CallbackFunctionUserPreference

void CallbackFunctionUserPreference::data_function(uint64_t code,
                                                   const stdsc::Buffer& buffer,
                                                   stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received user preference. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected <= state.current_state(),
      "Warn: must be connected state to receive user preference.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    std::shared_ptr<lbsr_share::EncPreference> enc_preference_ptr(
      new lbsr_share::EncPreference());
    enc_preference_ptr->load_from_stream(stream, param_.pubkey_filename);
    param_.enc_preference_ptr = enc_preference_ptr;

    std::ofstream ofs(param_.enc_preference_filename);
    enc_preference_ptr->save_to_stream(ofs);

    state.set(kEventReceivedUserPreference);
}
DEFINE_REQUEST_FUNC(CallbackFunctionUserPreference);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionUserPreference);

// CallbackFunctionComputeRequest

void CallbackFunctionComputeRequest::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_LOG_INFO(
      "Received compute request from Querier. (current state : %lu)",
      state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be ready state to receive compute request.");

    size_t size = 0;
    lbsr_share::EncCoOccurrence enc_cooccurrence;
    {
        std::ifstream ifs(param_.enc_cooccurrence_filename);
        enc_cooccurrence.load_from_stream(ifs, param_.pubkey_filename);
        size = enc_cooccurrence.size();
    }
    lbsr_share::EncPreference enc_preference;
    {
        std::ifstream ifs(param_.enc_preference_filename);
        enc_preference.load_from_stream(ifs, param_.pubkey_filename);
    }

    const auto& enc_preference_data = enc_preference.data();
    const auto& enc_cooccurrence_data = enc_cooccurrence.data();

    lbsr_share::EncRecommendation enc_recommendation;
    enc_recommendation.generate(enc_preference_data, enc_cooccurrence_data,
                                param_.pubkey_filename, size);

    STDSC_LOG_INFO("Encrypting recommendation list. (size: %lu)",
                   enc_recommendation.stream_size());

    stdsc::BufferStream buffstream(enc_recommendation.stream_size());
    std::iostream stream(&buffstream);
    enc_recommendation.save_to_stream(stream);

    stdsc::Buffer* buff_ptr = &buffstream;
    param_.client_.upload_recomm_list(*buff_ptr);

    state.set(kEventReceivedComputeRequest);
}
DEFINE_DATA_FUNC(CallbackFunctionComputeRequest);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionComputeRequest);

} /* namespace lbsr_pprs */
