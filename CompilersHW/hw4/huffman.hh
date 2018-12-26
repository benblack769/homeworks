/*
 * huffman.hh: API for Huffman encoder/decoder class
 */

#pragma once

#include <exception>
#include <memory>
#include <vector>

namespace huffman {

class Huffman {
  public:
    using symbol_t = uint8_t; // All encoded symbols are bytes
    enum bit_t { ZERO = 0, ONE = 1 }; // Represent bits
    // All symbols are encoded as vectors of '0's and '1's:
    using encoding_t = std::vector<bit_t>;
    using enc_iter_t = encoding_t::const_iterator;

    // Initialize object: all symbol frequencies (counts) start at zero.
    Huffman() noexcept;
    ~Huffman() noexcept;

    // For a given input symbol, increment its frequency (count), and
    // update the Huffman encoding as necessary.
    void incFreq(symbol_t symbol);

    // For a given symbol, return a unique vector of "0"s and "1"s that
    // represent the symbol's Huffman encoding (shorter strings for more
    // frequent symbols).
    encoding_t encode(symbol_t symbol) const;

    // For a given range to code of 0s and 1s, return the first unique
    // symbol represented by a prefix in the encoding.
    // Adjust the beginning of the range forward to just past the
    // unique prefix that was discoverd.
    // Throws a runtime exception if the code is invalid.
    symbol_t decode(enc_iter_t& begin, const enc_iter_t& end) const noexcept(false);

    // Return a code that represents no valid symbol (or prefix thereof).
    encoding_t eofCode() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace
