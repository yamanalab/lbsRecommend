#include <vector>
#include <lbsr_share/lbsr_utility.hpp>
#include <lbsr_share/lbsr_enc_recommendation.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace lbsr_share
{

struct EncRecommendation::Impl
{
    static constexpr int MAX = 10;

    Impl() : size_()
    {
    }

    void generate(const Ctxt& enc_preference,
                  const std::vector<Ctxt>& enc_cooccurrence,
                  const std::string& pubkey_filename, const size_t size)
    {
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
        int sz = size;
        long nslots = ea.size();

        if (nslots < sz)
        {
            std::cerr << "nslots is too small!" << std::endl;
            exit(-1);
        }

        auto& ctxts = enc_cooccurrence;

        auto& vecCtxt = enc_preference;

        std::shared_ptr<std::vector<Ctxt>> results_ptr(
          new std::vector<Ctxt>(sz, Ctxt(publicKey)));
        auto& results = *results_ptr;
        for (int i = 0; i < sz; i++)
        {
            results[i] = ctxts[i];
            results[i].multiplyBy(vecCtxt);
            totalSums(ea, results[i]);
        }

        ctxts_ptr_ = results_ptr;
        size_ = size;
    }

    void save_to_stream(std::ostream& os) const
    {
        if (ctxts_ptr_)
        {
            os << size_ << std::endl;

            auto& ctxts = *ctxts_ptr_;
            for (auto&& ctxt : ctxts)
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

    void load_from_stream_with_seckey(std::istream& is,
                                      const std::string& seckey_filename)
    {
        is >> size_;

        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream sk(seckey_filename);
        ::readContextBase(sk, m, p, r, gens, ords);
        ::FHEcontext context(m, p, r, gens, ords);
        sk >> context;
        ::FHESecKey secretKey(context);
        const ::FHEPubKey& publicKey = secretKey;
        sk >> secretKey;
        sk.close();

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

    void decrypt(const std::string& seckey_filename, std::vector<long> outvec)
    {
        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream sk(seckey_filename);
        ::readContextBase(sk, m, p, r, gens, ords);
        ::FHEcontext context(m, p, r, gens, ords);
        sk >> context;
        ::FHESecKey secretKey(context);
        sk >> secretKey;
        sk.close();
        ::EncryptedArray ea(context);

        int sz = size_;
        long nslots = ea.size();

        std::cout << sz << std::endl;
        if (nslots < sz)
        {
            std::cerr << "nslots is too small!" << std::endl;
            exit(-1);
        }

        auto& ctxts = *ctxts_ptr_;

        std::cout << "The decrept result is " << std::endl;
        for (int i = 0; i < sz; i++)
        {
            std::vector<long> res;
            ea.decrypt(ctxts[i], secretKey, res);
            std::cout << res[0] << std::endl;
            outvec.push_back(res[0]);
        }
    }

private:
    std::shared_ptr<std::vector<Ctxt>> ctxts_ptr_;
    size_t size_;
};

EncRecommendation::EncRecommendation(void) : pimpl_(new Impl())
{
}

void EncRecommendation::generate(const Ctxt& enc_preference,
                                 const std::vector<Ctxt>& enc_cooccurrence,
                                 const std::string& pubkey_filename,
                                 const size_t size)
{
    pimpl_->generate(enc_preference, enc_cooccurrence, pubkey_filename, size);
}

void EncRecommendation::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void EncRecommendation::load_from_stream(std::istream& is,
                                         const std::string& pubkey_filename)
{
    pimpl_->load_from_stream(is, pubkey_filename);
}

void EncRecommendation::load_from_stream_with_seckey(
  std::istream& is, const std::string& seckey_filename)
{
    pimpl_->load_from_stream_with_seckey(is, seckey_filename);
}

size_t EncRecommendation::size(void) const
{
    return pimpl_->size();
}

const std::vector<Ctxt>& EncRecommendation::data(void) const
{
    return pimpl_->data();
}

std::vector<Ctxt>& EncRecommendation::data(void)
{
    return pimpl_->data();
}

size_t EncRecommendation::stream_size(void) const
{
    return pimpl_->stream_size();
}

void EncRecommendation::decrypt(const std::string& seckey_filename,
                                std::vector<long> outvec)
{
    pimpl_->decrypt(seckey_filename, outvec);
}

} /* namespace lbsr_share */
