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
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <lbsr_share/lbsr_securekey_filemanager.hpp>
#include <lbsr_psp/lbsr_psp_callback_param.hpp>

namespace lbsr_psp
{

struct CallbackParam::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void set_skm(std::shared_ptr<lbsr_share::SecureKeyFileManager>& skm)
    {
        skm_ = skm;
    }

    lbsr_share::SecureKeyFileManager& get_skm(void)
    {
        STDSC_THROW_FAILURE_IF_CHECK(skm_,
                                     "Err: SecurekeyFilemanager is not set.");
        return *skm_;
    }

private:
    std::shared_ptr<lbsr_share::SecureKeyFileManager> skm_;
};

CallbackParam::CallbackParam(void) : pimpl_(new Impl())
{
}

void CallbackParam::set_skm(
  std::shared_ptr<lbsr_share::SecureKeyFileManager>& skm)
{
    pimpl_->set_skm(skm);
}

lbsr_share::SecureKeyFileManager& CallbackParam::get_skm(void)
{
    return pimpl_->get_skm();
}

} /* namespace lbsr_psp */
