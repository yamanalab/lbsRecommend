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
#include <string>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_client/lbsr_client_pprs_client.hpp>
#include <lbsr_client/lbsr_client_psp_client.hpp>
#include <share/define.hpp>

static constexpr const char* PubkeyFilename = "pubkey.txt";

struct Option
{
    std::string input_filename = "preference10.txt";
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

void exec(Option& option)
{
    const char* host = "localhost";

    lbsr_client::PSPParam psp_param;
    lbsr_client::PSPClient<> psp_client(host, SERVER_PORT_PSP_FOR_USER);
    psp_client.download_pubkey(PubkeyFilename);
    psp_client.start(psp_param);

    lbsr_client::PPRSParam pprs_param;
    pprs_param.input_filename = option.input_filename;
    pprs_param.pubkey_filename = PubkeyFilename;
    lbsr_client::PPRSClient<> pprs_client(host, SERVER_PORT_PPRS_FOR_USER);
    pprs_client.start(pprs_param);

    psp_client.wait_for_finish();
    pprs_client.wait_for_finish();
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Client demo app");
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
