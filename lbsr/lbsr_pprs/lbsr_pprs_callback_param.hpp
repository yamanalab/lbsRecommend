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

#ifndef LBSR_PPRS_CALLBACK_PARAM_HPP
#define LBSR_PPRS_CALLBACK_PARAM_HPP

#include <memory>
#include <vector>
#include <string>

namespace lbsr_share
{
class EncPreference;
}

namespace lbsr_pprs
{

class PSPClient;

/**
 * @brief This class is used to hold the callback parameters for PPRS server.
 */
struct CallbackParam
{
    CallbackParam(PSPClient& client);
    ~CallbackParam(void) = default;

    PSPClient& client_;

    std::string pubkey_filename;
    std::string enc_cooccurrence_filename;
    std::string enc_preference_filename;

    std::shared_ptr<lbsr_share::EncPreference> enc_preference_ptr;
};

} /* namespace lbsr_pprs */

#endif /* LBSR_PPRS_CALLBACK_PARAM_HPP */
