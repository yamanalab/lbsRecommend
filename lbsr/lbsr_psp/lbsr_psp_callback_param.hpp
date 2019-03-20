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

#ifndef LBSR_PSP_CALLBACK_PARAM_HPP
#define LBSR_PSP_CALLBACK_PARAM_HPP

#include <memory>
#include <vector>

namespace lbsr_share
{
    class SecureKeyFileManager;
    class EncRecommendation;
}

namespace lbsr_psp
{

/**
 * @brief This class is used to hold the callback parameters for PSP server.
 */
struct CallbackParam
{
    CallbackParam(void);
    ~CallbackParam(void) = default;
    
    void set_skm(std::shared_ptr<lbsr_share::SecureKeyFileManager>& skm);
    lbsr_share::SecureKeyFileManager& get_skm(void);

    std::shared_ptr<lbsr_share::EncRecommendation> enc_recommendation_ptr;

    std::string enc_recommendation_filename;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_psp */

#endif /* LBSR_PSP_CALLBACK_PARAM_HPP */
