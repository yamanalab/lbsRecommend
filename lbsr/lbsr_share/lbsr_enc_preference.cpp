#include <vector>
#include <lbsr_share/lbsr_utility.hpp>
#include <lbsr_share/lbsr_enc_preference.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace lbsr_share
{

struct EncPreference::Impl
{
    static constexpr int MAX = 10;

    Impl() : size_()
    {
    }

    void generate(const std::string& input_filename,
                  const std::string& pubkey_filename)
    {

        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream ct(pubkey_filename);
        readContextBase(ct, m, p, r, gens, ords);
        FHEcontext context(m, p, r, gens, ords);
        ct >> context;
        FHEPubKey publicKey(context);
        ct >> publicKey;
        ct.close();
        EncryptedArray ea(context);

        int sz = MAX; // TODO: load size from input file.

        long nslots = ea.size();

        if (nslots < sz)
        {
            std::cerr << "nslots is too small!" << std::endl;
            exit(-1);
        }

        std::vector<long> userprefer(MAX);
        std::ifstream infile(input_filename);
        for (int i = 0; i < MAX; i++)
        {
            infile >> userprefer[i];
        }

        std::cout << "The std::vector of user's preference: ";
        for (int j = 0; j < sz; j++)
        {
            std::cout << userprefer[j] << " ";
        }
        std::cout << std::endl;

        userprefer.resize(nslots, 0);
        std::shared_ptr<Ctxt> preferCtxt_ptr(new Ctxt(publicKey));
        auto& preferCtxt = *preferCtxt_ptr;
        ea.encrypt(preferCtxt, publicKey, userprefer);

        ctxt_ptr_ = preferCtxt_ptr;
        size_ = sz;
    }

    void save_to_stream(std::ostream& os) const
    {
        if (ctxt_ptr_)
        {
            os << size_ << std::endl;

            auto& ctxt = *ctxt_ptr_;
            os << ctxt << std::endl;
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

        std::shared_ptr<Ctxt> ctxt_ptr(new Ctxt(publicKey));
        auto& ctxt = *ctxt_ptr;
        is >> ctxt;

        ctxt_ptr_ = ctxt_ptr;
    }

    size_t size(void) const
    {
        return size_;
    }

    const Ctxt& data(void) const
    {
        return *ctxt_ptr_;
    }

    Ctxt& data(void)
    {
        return *ctxt_ptr_;
    }

    size_t stream_size(void) const
    {
        std::ostringstream oss;
        save_to_stream(oss);
        return oss.str().size();
    }

private:
    std::shared_ptr<Ctxt> ctxt_ptr_;
    size_t size_;
};

EncPreference::EncPreference(void) : pimpl_(new Impl())
{
}

void EncPreference::generate(const std::string& input_filename,
                             const std::string& pubkey_filename)
{
    pimpl_->generate(input_filename, pubkey_filename);
}

void EncPreference::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void EncPreference::load_from_stream(std::istream& is,
                                     const std::string& pubkey_filename)
{
    pimpl_->load_from_stream(is, pubkey_filename);
}

size_t EncPreference::size(void) const
{
    return pimpl_->size();
}

const Ctxt& EncPreference::data(void) const
{
    return pimpl_->data();
}

Ctxt& EncPreference::data(void)
{
    return pimpl_->data();
}

size_t EncPreference::stream_size(void) const
{
    return pimpl_->stream_size();
}

} /* namespace lbsr_share */
