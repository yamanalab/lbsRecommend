#include <vector>
#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_utility.hpp>
#include <lbsr_share/lbsr_enc_cooccurrence.hpp>

#include <FHE.h>
#include <EncryptedArray.h>

namespace lbsr_share
{

struct EncCoOccurrence::Impl
{
    static constexpr int MAX = 10;

    Impl() : size_()
    {
    }

    void generate(const std::string& input_filename,
                  const std::string& pubkey_filename)
    {
        if (!utility::file_exist(input_filename))
        {
            std::ostringstream oss;
            oss << "Err: Input file not found. (" << input_filename << ").";
            STDSC_THROW_FILE(oss.str());
        }
        if (!utility::file_exist(pubkey_filename))
        {
            std::ostringstream oss;
            oss << "Err: Publickey file not found. (" << pubkey_filename
                << ").";
            STDSC_THROW_FILE(oss.str());
        }

        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream ct(pubkey_filename);
        ::readContextBase(ct, m, p, r, gens, ords);
        ::FHEcontext context(m, p, r, gens, ords);
        ct >> context;
        ::FHEPubKey publicKey(context);
        ct >> publicKey;
        ct.close();
        ::EncryptedArray ea(context);

        size_t size = MAX; // TODO: Load from file.

        long nslots = ea.size();

        if (nslots < static_cast<long>(size))
        {
            std::cerr << "nslots is too small!" << std::endl;
            exit(-1);
        }

        std::vector<std::vector<int>> n(MAX, std::vector<int>(MAX));
        std::vector<std::vector<int>> N(MAX, std::vector<int>(MAX));

        std::ifstream infile(input_filename);
        for (int i = 0; i < MAX; i++)
        {
            for (int j = 0; j < MAX; j++)
            {
                infile >> n[i][j];
            }
        }

        std::cout << "The userpreference in plaintext is " << std::endl;
        for (int i = 0; i < MAX; i++)
        {
            for (int j = 0; j < MAX; j++)
            {
                std::cout << n[i][j] << " ";
            }
            std::cout << std::endl;
        }

        for (int i = 0; i < MAX; i++)
        {
            for (int j = 0; j < MAX; j++)
            {
                if (n[i][j] != 0)
                {
                    N[j][j] += 1;
                    for (int k = j + 1; k < MAX; k++)
                    {
                        if (n[i][k] != 0)
                        {
                            N[j][k] += 1;
                            N[k][j] += 1;
                        }
                    }
                }
            }
        }

        std::cout << "The co-occurence matrix is " << std::endl;
        for (int i = 0; i < MAX; i++)
        {
            for (int j = 0; j < MAX; j++)
            {
                std::cout << N[i][j] << "   ";
            }
            std::cout << std::endl;
        }

        std::vector<std::vector<long>> mat(size, std::vector<long>(nslots));
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                mat[i][j] = N[i][j];
            }
        }

        std::shared_ptr<std::vector<Ctxt>> ctxts_ptr(
          new std::vector<Ctxt>(size, Ctxt(publicKey)));
        auto& ctxts = *ctxts_ptr;
        for (size_t i = 0; i < size; i++)
        {
            ea.encrypt(ctxts[i], publicKey, mat[i]);
        }

        ctxts_ptr_ = ctxts_ptr;
        size_ = size;
    }

    void save_to_stream(std::ostream& os) const
    {
        if (ctxts_ptr_)
        {
            os << size_ << std::endl;

            auto& ctxts = *ctxts_ptr_;
            for (auto& ctxt : ctxts)
            {
                os << ctxt << std::endl;
            }
        }
    }

    void load_from_stream(std::istream& is, const std::string& pubkey_filename)
    {
        is >> size_;

        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream ct(pubkey_filename);
        ::readContextBase(ct, m, p, r, gens, ords);
        ::FHEcontext context(m, p, r, gens, ords);
        ct >> context;
        ::FHEPubKey publicKey(context);
        ct >> publicKey;
        ct.close();

        std::shared_ptr<std::vector<Ctxt>> ctxts_ptr(
          new std::vector<Ctxt>(size_, Ctxt(publicKey)));
        auto& ctxts = *ctxts_ptr;
        for (size_t i = 0; i < size_; i++)
        {
            is >> ctxts[i];
        }
        ctxts_ptr_ = ctxts_ptr;
    }

    size_t size(void) const
    {
        return size_;
    }

    const std::vector<Ctxt>& data(void) const
    {
        return *ctxts_ptr_;
    }

    std::vector<Ctxt>& data(void)
    {
        return *ctxts_ptr_;
    }

    size_t stream_size(void) const
    {
        std::ostringstream oss;
        save_to_stream(oss);
        return oss.str().size();
    }

private:
    std::shared_ptr<std::vector<Ctxt>> ctxts_ptr_;
    size_t size_;
};

EncCoOccurrence::EncCoOccurrence(void) : pimpl_(new Impl())
{
}

void EncCoOccurrence::generate(const std::string& input_filename,
                               const std::string& pubkey_filename)
{
    pimpl_->generate(input_filename, pubkey_filename);
}

void EncCoOccurrence::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void EncCoOccurrence::load_from_stream(std::istream& is,
                                       const std::string& pubkey_filename)
{
    pimpl_->load_from_stream(is, pubkey_filename);
}

size_t EncCoOccurrence::size(void) const
{
    return pimpl_->size();
}

const std::vector<Ctxt>& EncCoOccurrence::data(void) const
{
    return pimpl_->data();
}

std::vector<Ctxt>& EncCoOccurrence::data(void)
{
    return pimpl_->data();
}

size_t EncCoOccurrence::stream_size(void) const
{
    return pimpl_->stream_size();
}

} /* namespace lbsr_share */
