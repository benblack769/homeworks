/*
 * tree.hh: Abstract API for a simple, non-mutating binary tree class.
 * A tree cannot be empty: it always has at least a root with a value.
 * Written by Eitan Frachtenberg.
 */

#pragma once

#include <string>

namespace tree {

class Tree {
  public:
    using value_t = unsigned;

    Tree() = default; // Base class doesn't initialize anything.
    virtual ~Tree() = default;  // Destructor must always be virtual in base class.

    // Disallow tree copies:
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;

    // Create a new tree with a new given root and two existing trees as the
    // left and right child of the new root. Note this method isn't virtual.
    Tree(value_t, const Tree&, const Tree&)
        {}// Empty implementation in base class. Derived class must reimplement.

    /*
     * The next methods are all pure virtual: derived classes must
     * implement them themselves, but this class can never be concrete.
     */


    // How many elements does this tree hold?
    virtual unsigned size() const = 0;

    // For a given value, return a string of "L" and "R" characters,
    // representing the left and right turns from the root to arrive at the value.
    // Returns and empty string if the value is at the root.
    // Throws a runtime_error exception if the value wasn't found.
    virtual std::string pathTo(value_t value) const = 0;

    // For a given path (string of "L"s and "R"s), return the value found at
    // that path of the tree.
    // Throws a runtime_error exception if the path is invalid in some way.
    virtual value_t getByPath(const std::string& path) const = 0;
};


} // namespace
