#ifndef LBSR_ENC_PREFERENCE_HPP
#define LBSR_ENC_PREFERENCE_HPP

#include <iostream>
#include <memory>

class Ctxt;

namespace lbsr_share
{

/**
 * @brief This clas is used to hold the encrypted preference.
 */
struct EncPreference
{
    EncPreference(void);
    ~EncPreference(void) = default;

    void generate(const std::string& input_filename,
                  const std::string& pubkey_filename);

    void save_to_stream(std::ostream& os) const;

    void load_from_stream(std::istream& is, const std::string& pubkey_filename);

    size_t size(void) const;

    const Ctxt& data(void) const;
    Ctxt& data(void);

    size_t stream_size(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_share */

#endif /* LBSR_ENC_PREFERENCE_HPP */
