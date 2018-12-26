/*
 * vectree.hh: API for a simple, non-mutating binary tree class.
 * A tree cannot be empty: it always has at least a root with a value.
 * See tree.hh for virtual API documentation.
 * Written by Eitan Frachtenberg.
 */

#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "tree.hh"

namespace tree {

class VecTree : public Tree {
  public:
    VecTree(value_t value);
    ~VecTree() = default;
    VecTree(value_t newroot, const VecTree& left, const VecTree& right);

    virtual unsigned size() const override;

    virtual std::string pathTo(value_t value) const override;

    value_t getByPath(const std::string& path) const override;

  private:
    std::vector<value_t> flatTree_;
};

} // namespace
