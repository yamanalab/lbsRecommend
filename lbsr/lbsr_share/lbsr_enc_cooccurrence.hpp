#ifndef LBSR_CO_OCCURRENCE_HPP
#define LBSR_CO_OCCURRENCE_HPP

#include <iostream>
#include <vector>
#include <memory>

class Ctxt;

namespace lbsr_share
{

/**
 * @brief This class is used to hold the encrypted co-occurrence matrix
 */
struct EncCoOccurrence
{
    EncCoOccurrence(void);
    ~EncCoOccurrence(void) = default;

    void generate(const std::string& input_filename,
                  const std::string& pubkey_filename);

    void save_to_stream(std::ostream& os) const;

    void load_from_stream(std::istream& is, const std::string& pubkey_filename);

    size_t size(void) const;

    const std::vector<Ctxt>& data(void) const;
    std::vector<Ctxt>& data(void);

    size_t stream_size(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_share */

#endif /* LBSR_CO_OCCURRENCE_HPP */
