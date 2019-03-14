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

#ifndef LBSR_PPRS_PSP_CLIENT_HPP
#define LBSR_PPRS_PSP_CLIENT_HPP

#include <memory>

namespace stdsc
{
class StateContext;
class Buffer;
}

namespace lbsr_pprs
{

/**
 * @brief Provides client for PSP.
 */
class PSPClient
{
    friend class CallbackFunctionComputeRequest;

public:
    PSPClient(const char* host, const char* port, stdsc::StateContext& state);
    virtual ~PSPClient(void) = default;

    void download_pubkey(const char* out_filename = "pukey.txt");

private:
    void upload_recomm_list(const stdsc::Buffer& buffer);

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_pprs */

#endif /* LBSR_PPRS_PSP_CLIENT_HPP */
