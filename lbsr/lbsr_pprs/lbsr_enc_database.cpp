#include <vector>
#include <fstream>
#include <lbsr_share/lbsr_utility.hpp>
#include <lbsr_share/lbsr_enc_cooccurrence.hpp>
#include <lbsr_pprs/lbsr_enc_database.hpp>
#include <lbsr_pprs/lbsr_pprs_state.hpp>

namespace lbsr_pprs
{

struct EncDatabase::Impl
{
    Impl(stdsc::StateContext& state, const char* enc_cooccurrence_filename)
        : state_(state), enc_cooccurrence_filename_(enc_cooccurrence_filename)
    {
    }

    void initialize(const std::string& input_filename,
                    const std::string& pubkey_filename)
    {
        lbsr_share::EncCoOccurrence enc_cooccurrence;
        enc_cooccurrence.generate(input_filename, pubkey_filename);

        std::ofstream ofs(enc_cooccurrence_filename_);
        enc_cooccurrence.save_to_stream(ofs);

        state_.set(lbsr_pprs::kEventGeneratedOccurrence);
    }

private:
    stdsc::StateContext& state_;
    const std::string enc_cooccurrence_filename_;
};

EncDatabase::EncDatabase(stdsc::StateContext& state,
                         const char* enc_cooccurrence_filename)
  : pimpl_(new Impl(state, enc_cooccurrence_filename))
{
}

void EncDatabase::initialize(const std::string& input_filename,
                             const std::string& pubkey_filename)
{
    pimpl_->initialize(input_filename, pubkey_filename);
}

} /* namespace lbsr_pprs */
