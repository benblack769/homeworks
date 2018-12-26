/*
 * Example "compression" program to encode an input file using Huffman
 * encoding. The output is currently just a string of zeros and ones,
 * not actual bits.
 * The input comes in through stdandard input, and the output goes to
 * standard output. Use shell redicrection to compress files.
 */

#include <iostream>
#include <fstream>
#include <string>

#include "huffman.hh"

using namespace std;

int main(int argc, char** argv)
{
  const bool verbose = argc > 1 && string(argv[1]) == "-v";
  huffman::Huffman huff;

  // Read in all of stdin, line by line.
  // Iterate over input characters, output their encoding
  // and update their frequency:
  for (string line; getline(cin, line); ) {
      for (auto c : line + '\n') {
          if (verbose)  cout << c << "\t";
          for (auto bit : huff.encode(c)) {
              cout << bit;
          }
          huff.incFreq(c);
          if (verbose) cout << "\n";
      }
  }

  // Finally, output end-of-file code
  if (verbose) cout << "EOF\t";
  for (auto bit : huff.eofCode()) {
      cout << bit;
  }
  if (verbose) cout << "\n";

  return 0;
}
