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

#ifndef LBSR_PPRS_STATE_HPP
#define LBSR_PPRS_STATE_HPP

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace lbsr_pprs
{

/**
 * @brief Enumeration for state of PPRS.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateInit = 1,
    kStateConnected = 2,
    kStateReady = 3,
    kStateComputed = 4,
    kStateExit = 5,
};

/**
 * @brief Enumeration for events of PPRS.
 */
enum Event_t : uint64_t
{
    kEventConnectSocket = 0,
    kEventDisconnectSocket = 1,
    kEventSendPubkeyRequest = 2,
    kEventGeneratedOccurrence = 3,
    kEventReceivedUserPreference = 4,
    kEventReceivedComputeRequest = 5,
};

/**
 * @brief Provides 'Init' state.
 */
struct StateInit : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateInit(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateInit;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Connected' state.
 */
struct StateConnected : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create(
      bool is_generated_occurrence = false,
      bool is_received_user_preference = false,
      bool is_received_pubkey = false);
    StateConnected(bool is_generated_occurrence = false,
                   bool is_received_user_preference = false,
                   bool is_received_pubkey = false);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateConnected;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create();
    StateReady(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateReady;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
* @brief Provides 'Computed' state.
*/
struct StateComputed : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateComputed(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateComputed;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Exit' state.
 */
struct StateExit : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateExit(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateExit;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* lbsr_pprs */

#endif /* LBSR_PPRS_STATE_HPP */
