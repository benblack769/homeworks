#include "catch.hpp"
#include "huffman.hh"

#include <limits.h>

using namespace huffman;

TEST_CASE("All symbols start with different codes", "[diff_codes]") {
  auto huff = Huffman();
  for (unsigned i = 0; i < 256; ++i) {
    for (unsigned j = 0; i < 256; ++i) {
      if (i != j) {
        REQUIRE(huff.encode(i) != huff.encode(j));
      }
    }
  }
}

TEST_CASE("Most frequent symbol encodes shortest", "[compression]") {
  auto huff = Huffman();
  huff.incFreq(0);
  huff.incFreq(0);
  const auto len0 = huff.encode(0).size();

  for (unsigned i = 1; i < 256; ++i) {
    REQUIRE(huff.encode(i).size() > len0);
  }
}

TEST_CASE("Most frequent symbol always encodes to single bit", "[compression]") {
  auto huff = Huffman();
  huff.incFreq(0);
  huff.incFreq(0);
  REQUIRE(huff.encode(0).size() == 1);
  REQUIRE(huff.encode(1).size() > 1);
  huff.incFreq(1);
  huff.incFreq(1);
  huff.incFreq(1);
  REQUIRE(huff.encode(0).size() > 1);
  REQUIRE(huff.encode(1).size() == 1);
}
