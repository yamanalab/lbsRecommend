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

#ifndef LBSR_PSP_STATE_HPP
#define LBSR_PSP_STATE_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace lbsr_psp
{

/**
 * @brief Enumeration for state of PSP.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateInit = 1,
    kStateConnected = 2,
    kStateGeneratedKeys = 3,
    kStateDecryptedRecommList = 4,
    kStateComputed = 5,
    kStateExit = 6,
};

/**
 * @brief Enumeration for events of PSP.
 */
enum Event_t : uint64_t
{
    kEventConnectSocket = 0,
    kEventDisconnectSocket = 1,
    kEventGeneratedKeys = 2,
    kEventReceivedPubkey = 3,
    kEventReceivedLocationInfo = 4,
    kEventReceivedRecommList = 5,
    kEventReceivedResultRequest = 6,
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
    static std::shared_ptr<stdsc::State> create();
    StateConnected(void);
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
 * @brief Provides 'GeneratedKeys' state.
 */
struct StateGeneratedKeys : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create(bool is_received_location_info =
                                                  false,
                                                bool is_received_recomm_list =
                                                  false);

    StateGeneratedKeys(bool is_received_location_info = false,
                       bool is_received_recomm_list = false);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateGeneratedKeys;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'DecryptedRecommList' state.
 */
struct StateDecryptedRecommList : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateDecryptedRecommList(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateDecryptedRecommList;
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

} /* lbsr_psp */

#endif /* LBSR_PSP_STATE_HPP */
