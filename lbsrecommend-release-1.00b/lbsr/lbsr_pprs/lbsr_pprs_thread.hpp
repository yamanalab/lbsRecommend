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

#ifndef LBSR_PPRS_THEAD_HPP
#define LBSR_PPRS_THEAD_HPP

#include <memory>
#include <string>

namespace stdsc
{
class CallbackFunctionContainer;
class StateContext;
}

namespace lbsr_pprs
{

/**
 * @brief Provides PPRS server.
 */
class PprsThread
{
public:
    PprsThread(const char* port, stdsc::CallbackFunctionContainer& callback,
               stdsc::StateContext& state);
    ~PprsThread(void) = default;

    void start(void);
    void join(void);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_pprs */

#endif /* LBSR_PPRS_THEAD_HPP */
