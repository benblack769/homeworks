/*
 * vectree: An implementation of tree class using vectors.
 * Semantics: the ith-node in the tree has and two children at nodes:
 * ((i+1)*2-1) and ((i+1)*2).
 * This implementation imposes an additional constraint that the tree must be
 * fully balanced at all times. An exception is thrown otherwise.
 */

#include "vectree.hh"

#include <algorithm>
#include <stdexcept>

namespace tree {

///////////////////////////////////////////////////////////////////////////////
VecTree::VecTree(value_t value)
    : Tree(), flatTree_(1, value)
{ }


///////////////////////////////////////////////////////////////////////////////
unsigned
VecTree::size() const
{
    return flatTree_.size();
}


///////////////////////////////////////////////////////////////////////////////
VecTree::VecTree(value_t root, const VecTree& left, const VecTree& right)
{
    if (left.size() != right.size()) {
        throw std::runtime_error("tree_vec requires balanced trees!");
    }
    flatTree_.push_back(root);

    int to_copy = 1;
    auto start_left = left.flatTree_.cbegin();
    auto start_right = right.flatTree_.cbegin();

    for (unsigned copied = 0; copied < left.size(); copied += to_copy, to_copy *= 2) {
      std::copy(start_left, start_left + to_copy, back_inserter(flatTree_));
      start_left += to_copy;
      std::copy(start_right, start_right + to_copy, back_inserter(flatTree_));
      start_right += to_copy;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Scan vector for first ocurrence of value, then build path back up to root
std::string
VecTree::pathTo(value_t value) const
{
    const auto where = std::find(flatTree_.cbegin(), flatTree_.cend(), value);
    if (where == flatTree_.cend()) {
        throw std::runtime_error("Value not found!");
    }

    std::string path = "";
    for (int64_t idx = where - flatTree_.cbegin(); idx; idx = (idx - 1) / 2) {
        const char child = idx % 2 ? 'L' : 'R';
        path.insert(path.begin(), child);
    }
    return path;
}


///////////////////////////////////////////////////////////////////////////////
// Follow path from root to end of string, returning value when done
VecTree::value_t
VecTree::getByPath(const std::string& path) const
{
    size_t cur_idx = 0;        // Start by pointing index at root

    for (const auto c : path) {
        const auto right_idx = (cur_idx + 1) * 2;  // Index of right child
        if (right_idx > size()) {
            throw std::runtime_error("Path too long!");
        }
        if (c == 'R') {
            cur_idx = right_idx;
        } else if (c == 'L') {
            cur_idx = right_idx - 1;
        } else {
            throw std::runtime_error("Invalid character in path: " + path);
        }
    }
    return flatTree_[cur_idx];
}


} // namespace
