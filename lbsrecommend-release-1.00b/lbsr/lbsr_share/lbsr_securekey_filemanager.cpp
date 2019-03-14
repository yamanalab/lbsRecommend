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

#include <stdsc/stdsc_log.hpp>
#include <lbsr_share/lbsr_utility.hpp>
#include <lbsr_share/lbsr_securekey_filemanager.hpp>

#include "FHE.h"
#include "EncryptedArray.h"

namespace lbsr_share
{

struct SecureKeyFileManager::Impl
{
    Impl(const std::string& pubkey_filename, const std::string& seckey_filename,
         const long fheM = DefaultFheM, const long fheL = DefaultFheL)
      : pubkey_filename_(pubkey_filename),
        seckey_filename_(seckey_filename),
        m_(fheM),
        L_(fheL)
    {
    }
    ~Impl(void) = default;

    void initialize(void)
    {
        FHEcontext context(m_, p_, r_);
        buildModChain(context, L_, c_);

        NTL::ZZX G = context.alMod.getFactorsOverZZ()[0];

        FHESecKey secretKey(context);
        const FHEPubKey& publicKey = secretKey;

        secretKey.GenSecKey(w_);
        addSome1DMatrices(secretKey);

        EncryptedArray ea(context, G);

        if (context.zMStar.numOfGens() != 1)
        {
            STDSC_LOG_ERR("noise increase when it rotates\n");
            exit(0);
        }
        for (int i = 0; i < (int)context.zMStar.numOfGens(); i++)
        {
            if (!context.zMStar.SameOrd(i))
            {
                STDSC_LOG_ERR("noise increase when it rotates\n");
                exit(0);
            }
        }

        std::fstream publicFile(pubkey_filename_,
                                std::fstream::out | std::fstream::trunc);
        writeContextBase(publicFile, context);
        publicFile << context << std::endl;
        publicFile << publicKey << std::endl;
        publicFile.close();

        std::fstream secretFile(seckey_filename_,
                                std::fstream::out | std::fstream::trunc);
        writeContextBase(secretFile, context);
        secretFile << context << std::endl;
        secretFile << secretKey << std::endl;
        secretFile.close();
    }

    size_t pubkey_size(void) const
    {
        return lbsr_share::utility::file_size(pubkey_filename_);
    }
    
    size_t seckey_size(void) const
    {
        return lbsr_share::utility::file_size(seckey_filename_);
    }

    void pubkey_data(void* buffer)
    {
        size_t size = pubkey_size();
        std::ifstream ifs(pubkey_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::cout << "failed to open " << pubkey_filename_ << std::endl;
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }

    void seckey_data(void* buffer)
    {
        size_t size = seckey_size();
        std::ifstream ifs(seckey_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::cout << "failed to open " << seckey_filename_ << std::endl;
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }

    bool is_exist_pubkey(void) const
    {
        std::ifstream ifs(pubkey_filename_);
        return ifs.is_open();
    }

    bool is_exist_seckey(void) const
    {
        std::ifstream ifs(seckey_filename_);
        return ifs.is_open();
    }

    std::string pubkey_filename(void) const
    {
        return pubkey_filename_;
    }

    std::string seckey_filename(void) const
    {
        return seckey_filename_;
    }

private:
    std::string pubkey_filename_;
    std::string seckey_filename_;
    long m_ = 30269;
    long L_ = 34;
    
    long p_ = 8191; // p^r
    long r_ = 3;
    long c_ = 3;  // 3
    long w_ = 64; // 64
    long d_ = 0;
    long k_ = 128; // 90 or 128
    long s_ = 0;

};

SecureKeyFileManager::SecureKeyFileManager(const std::string& pubkey_filename,
                                           const std::string& seckey_filename,
                                           const long fheM, const long fheL)
  : pimpl_(new Impl(pubkey_filename, seckey_filename, fheM, fheL))
{
}

void SecureKeyFileManager::initialize(void)
{
    pimpl_->initialize();
}

size_t SecureKeyFileManager::pubkey_size(void) const
{
    return pimpl_->pubkey_size();
}

size_t SecureKeyFileManager::seckey_size(void) const
{
    return pimpl_->seckey_size();
}

void SecureKeyFileManager::pubkey_data(void* buffer)
{
    pimpl_->pubkey_data(buffer);
}

void SecureKeyFileManager::seckey_data(void* buffer)
{
    pimpl_->seckey_data(buffer);
}

bool SecureKeyFileManager::is_exist_pubkey(void) const
{
    return pimpl_->is_exist_pubkey();
}

bool SecureKeyFileManager::is_exist_seckey(void) const
{
    return pimpl_->is_exist_seckey();
}

std::string SecureKeyFileManager::pubkey_filename(void) const
{
    return pimpl_->pubkey_filename();
}

std::string SecureKeyFileManager::seckey_filename(void) const
{
    return pimpl_->seckey_filename();
}

} /* namespace lbsr_share */
