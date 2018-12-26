/*
 * PtrTree.hh: API for a simple, non-mutating binary tree class.
 * A tree cannot be empty: it always has at least a root with a value.
 * See tree.hh for virtual API documentation.
 */

#pragma once

#include <string>
#include <vector>

#include "tree.hh"

namespace tree {

class PtrTree : public Tree {
  public:
    PtrTree(value_t value);
    PtrTree(const PtrTree & other);
    ~PtrTree();
    PtrTree(value_t newroot, const PtrTree& left, const PtrTree& right);

    virtual unsigned size() const override;

    virtual std::string pathTo(value_t value) const override;

    value_t getByPath(const std::string& path) const override;

  private:
    bool valueIn(value_t value) const;
  private:
    unsigned _size;
    value_t root_value;
    PtrTree * _left;
    PtrTree * _right;
};

} // namespace
