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
#include <unistd.h>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_securekey_filemanager.hpp>
#include <lbsr_psp/lbsr_psp_state.hpp>
#include <psp/server.hpp>

static constexpr const char* PUBKEY_FILENAME = "pubkey.txt";
static constexpr const char* SECKEY_FILENAME = "seckey.txt";
static constexpr const char* ENC_RECOMMENDATION_FILENAME =
  "enc_recommendation.txt";

struct Param
{
    std::string pubkey_filename = PUBKEY_FILENAME;
    std::string seckey_filename = SECKEY_FILENAME;
    bool is_generate_securekey = false;
};

void init(Param& param, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "p:s:gh")) != -1)
    {
        switch (opt)
        {
            case 'p':
                param.pubkey_filename = optarg;
                break;
            case 's':
                param.seckey_filename = optarg;
                break;
            case 'g':
                param.is_generate_securekey = true;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-p pubkey_filename] [-s seckey_filename] [-g]\n",
                  argv[0]);
                exit(1);
        }
    }
}

void exec(const Param& param)
{
    lbsr_psp::CallbackParam cb_param;

    std::shared_ptr<lbsr_share::SecureKeyFileManager> skm_ptr(
      new lbsr_share::SecureKeyFileManager(param.pubkey_filename,
                                           param.seckey_filename));
    cb_param.set_skm(skm_ptr);
    cb_param.enc_recommendation_filename = ENC_RECOMMENDATION_FILENAME;

    STDSC_LOG_INFO("Launched PSP demo app");
    stdsc::StateContext state(std::make_shared<lbsr_psp::StateInit>());

    lbsr_demo::Server server_for_user(cb_param, state, PSP_PORT_FOR_USER,
                                      param.is_generate_securekey);
    lbsr_demo::Server server_for_pprs(cb_param, state, PSP_PORT_FOR_PPRS);
    
    server_for_user.start();
    server_for_pprs.start();

    server_for_user.join();
    server_for_pprs.join();
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Param param;
        init(param, argc, argv);
        exec(param);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }
    return 0;
}
