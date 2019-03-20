#ifndef LBSR_ENC_DATABASE_HPP
#define LBSR_ENC_DATABASE_HPP

#include <string>
#include <cstdbool>
#include <memory>

namespace stdsc
{
class StateContext;
}

namespace lbsr_pprs
{

/**
 * @brief This class is used to hold the encrypted data.
 */
struct EncDatabase
{
    EncDatabase(stdsc::StateContext& state,
                const char* enc_cooccurrence_filename = "enc_cooccurrence.txt");
    ~EncDatabase(void) = default;

    void initialize(const std::string& input_filename,
                    const std::string& pubkey_filename);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace lbsr_pprs */

#endif /* LBSR_ENC_DATABASE_HPP */
