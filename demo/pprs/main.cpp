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

#include <unistd.h>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_packet.hpp>
#include <lbsr_pprs/lbsr_enc_database.hpp>
#include <lbsr_pprs/lbsr_pprs_state.hpp>
#include <lbsr_pprs/lbsr_pprs_psp_client.hpp>
#include <lbsr_pprs/lbsr_pprs_thread.hpp>
#include <lbsr_pprs/lbsr_pprs_callback_param.hpp>
#include <lbsr_pprs/lbsr_pprs_callback_function.hpp>
#include <share/define.hpp>

static constexpr char* PubkeyFilename = (char*)"pubkey.txt";
static constexpr char* CoOccurrenceInputFilename = (char*)"inputdata10.txt";
static constexpr char* EncCoOccurrenceFilename = (char*)"enc_cooccurrence.txt";
static constexpr char* EncPreferenceFilename = (char*)"enc_preference.txt";


struct Option
{
    std::string input_filename = CoOccurrenceInputFilename;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                option.input_filename = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s [-i input_filename]\n",
                       argv[0]);
                exit(1);
        }
    }
}

void create_psp_server(lbsr_pprs::CallbackParam& param,
                       stdsc::StateContext& state,
                       const char* port = SERVER_PORT_PPRS_FOR_USER)
{
    std::shared_ptr<stdsc::CallbackFunction> cb_pref(
      new lbsr_pprs::CallbackFunctionUserPreference(param));
    std::shared_ptr<stdsc::CallbackFunction> cb_compute(
      new lbsr_pprs::CallbackFunctionComputeRequest(param));

    stdsc::CallbackFunctionContainer callback;
    callback.set(lbsr_share::kControlCodeDataUserPreference, cb_pref);
    callback.set(lbsr_share::kControlCodeRequestCompute, cb_compute);

    lbsr_pprs::PprsThread server(port, callback, state);
    server.start();
    server.join();
}

void exec(const Option& option)
{
    STDSC_LOG_INFO("Launched PPRS demo app");
    stdsc::StateContext state(std::make_shared<lbsr_pprs::StateInit>());

    lbsr_pprs::PSPClient client("localhost", SERVER_PORT_PSP_FOR_PPRS, state);
    client.download_pubkey(PubkeyFilename);

    lbsr_pprs::EncDatabase encdb(state, EncCoOccurrenceFilename);
    encdb.initialize(option.input_filename, PubkeyFilename);

    lbsr_pprs::CallbackParam param(client);
    param.pubkey_filename = PubkeyFilename;
    param.enc_cooccurrence_filename = EncCoOccurrenceFilename;
    param.enc_preference_filename = EncPreferenceFilename;

    create_psp_server(param, state);
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        exec(option);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("catch exception: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("catch unknown exception");
    }
    return 0;
}
