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

#include <cstdbool>
#include <mutex>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>
#include <lbsr_pprs/lbsr_pprs_state.hpp>

namespace lbsr_pprs
{

/* client, PSP */
static constexpr std::size_t MAX_CLIENT_NUM = 2;

struct StateInit::Impl
{
    Impl(size_t initial_connection_count, bool is_generated_occurrence,
         bool is_received_pubkey)
      : count_(initial_connection_count),
        is_generated_occurrence_(is_generated_occurrence),
        is_received_pubkey_(is_received_pubkey)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateInit: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventConnectSocket:
                if (++count_ >= 2)
                {
                    sc.next_state(StateConnected::create(
                      is_generated_occurrence_, is_received_pubkey_));
                }
                break;
            case kEventGeneratedOccurrence:
                is_generated_occurrence_ = true;
                break;
            case kEventSendPubkeyRequest:
                is_received_pubkey_ = true;
                break;
            case kEventDisconnectSocket:
                --count_;
                break;
            default:
                break;
        }
    }

private:
    std::size_t count_;
    bool is_generated_occurrence_;
    bool is_received_pubkey_;
    std::mutex mutex_;
};

struct StateConnected::Impl
{
    Impl(bool is_generated_occurrence, bool is_received_pubkey)
      : is_generated_occurrence_(is_generated_occurrence),
        is_received_pubkey_(is_received_pubkey),
        is_received_user_preference_(false)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateConnected: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventGeneratedOccurrence:
                is_generated_occurrence_ = true;
                break;
            case kEventReceivedUserPreference:
                is_received_user_preference_ = true;
                break;
            case kEventSendPubkeyRequest:
                is_received_pubkey_ = true;
                break;
            case kEventDisconnectSocket:
                sc.next_state(StateInit::create(MAX_CLIENT_NUM - 1,
                                                is_generated_occurrence_,
                                                is_received_pubkey_));
                break;
            default:
                break;
        }

        if (is_generated_occurrence_ && is_received_user_preference_ &&
            is_received_pubkey_)
        {
            sc.next_state(StateReady::create());
        }
    }

private:
    bool is_generated_occurrence_;
    bool is_received_pubkey_;
    bool is_received_user_preference_;
    std::mutex mutex_;
};

struct StateReady::Impl
{
    Impl()
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateReady: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventReceivedComputeRequest:
                sc.next_state(StateComputed::create());
                break;
            case kEventDisconnectSocket:
                sc.next_state(
                  StateInit::create(MAX_CLIENT_NUM - 1, true, true));
                break;
            default:
                break;
        }
    }

private:
    std::mutex mutex_;
};

struct StateComputed::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateComputed(%lu): event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventGeneratedOccurrence:
            case kEventReceivedUserPreference:
            case kEventSendPubkeyRequest:
                sc.next_state(StateReady::create());
                break;
            case kEventDisconnectSocket:
                sc.next_state(
                  StateInit::create(MAX_CLIENT_NUM - 1, true, true));
                break;
            default:
                break;
        }
    }

private:
    std::mutex mutex_;
};

struct StateExit::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
    }

private:
    std::mutex mutex_;
};

// Init

std::shared_ptr<stdsc::State> StateInit::create(size_t initial_connection_count,
                                                bool is_generated_occurrence,
                                                bool is_received_pubkey)
{
    auto s = std::shared_ptr<stdsc::State>(new StateInit(
      initial_connection_count, is_generated_occurrence, is_received_pubkey));
    return s;
}

StateInit::StateInit(size_t initial_connection_count,
                     bool is_generated_occurrence, bool is_received_pubkey)
  : pimpl_(new Impl(initial_connection_count, is_generated_occurrence,
                    is_received_pubkey))
{
}

void StateInit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Connected

std::shared_ptr<stdsc::State> StateConnected::create(
  bool is_generated_occurrence, bool is_received_pubkey)
{
    auto s = std::shared_ptr<stdsc::State>(
      new StateConnected(is_generated_occurrence, is_received_pubkey));
    return s;
}

StateConnected::StateConnected(bool is_generated_occurrence,
                               bool is_received_pubkey)
  : pimpl_(new Impl(is_generated_occurrence, is_received_pubkey))
{
}

void StateConnected::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Ready

std::shared_ptr<stdsc::State> StateReady::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateReady());
    return s;
}

StateReady::StateReady() : pimpl_(new Impl())
{
}

void StateReady::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Computed

std::shared_ptr<stdsc::State> StateComputed::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateComputed());
    return s;
}

StateComputed::StateComputed(void) : pimpl_(new Impl())
{
}

void StateComputed::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Exit

std::shared_ptr<stdsc::State> StateExit::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateExit());
    return s;
}

StateExit::StateExit(void) : pimpl_(new Impl())
{
}

void StateExit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

} /* lbsr_pprs */
