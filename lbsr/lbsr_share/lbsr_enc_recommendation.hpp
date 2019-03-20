#ifndef LBSR_ENC_RECOMMENDATION_HPP
#define LBSR_ENC_RECOMMENDATION_HPP

#include <iostream>
#include <memory>
#include <vector>

class Ctxt;

namespace lbsr_share
{

/**
 * @brief This class is used to hold the encrypted recommendation
 */
struct EncRecommendation
{
    EncRecommendation(void);
    ~EncRecommendation(void) = default;

    void generate(const Ctxt& enc_preference,
                  const std::vector<Ctxt>& enc_cooccurrence,
                  const std::string& pubkey_filename, const size_t size);

    void save_to_stream(std::ostream& os) const;

    void load_from_stream(std::istream& is, const std::string& pubkey_filename);

    void load_from_stream_with_seckey(std::istream& is,
                                      const std::string& seckey_filename);

    size_t size(void) const;

    const std::vector<Ctxt>& data(void) const;
    std::vector<Ctxt>& data(void);

    size_t stream_size(void) const;

    void decrypt(const std::string& seckey_filename, std::vector<long> outvec);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_share */

#endif /* LBSR_ENC_RECOMMENDATION_HPP */
