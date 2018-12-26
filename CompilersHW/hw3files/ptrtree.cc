/*
 * PtrTree: An implementation of tree class using pointers.
 */

#include "ptrtree.hh"
#include <stdexcept>

namespace tree {

///////////////////////////////////////////////////////////////////////////////
PtrTree::PtrTree(value_t value)
    : Tree(),
      _size(1),
      root_value(value),
      _left(nullptr),
      _right(nullptr)
{ }

///////////////////////////////////////////////////////////////////////////////
PtrTree::~PtrTree(){
    if(_left != nullptr){
        delete _left;
    }
    if(_right != nullptr){
        delete _right;
    }
}


PtrTree::PtrTree(const PtrTree & other){
    _size = other._size;
    root_value = other.root_value;
    _left = other._left ? new PtrTree(*other._left) : nullptr;
    _right = other._right ? new PtrTree(*other._right) : nullptr;
}

///////////////////////////////////////////////////////////////////////////////
unsigned
PtrTree::size() const
{
    return _size;
}


///////////////////////////////////////////////////////////////////////////////
PtrTree::PtrTree(value_t root, const PtrTree& left, const PtrTree& right):
    Tree(),
    _size(left.size() + right.size() + 1),
    root_value(root),
    _left(new PtrTree(left)),
    _right(new PtrTree(right))
{}

///////////////////////////////////////////////////////////////////////////////
bool
PtrTree::valueIn(value_t value) const
{
    return value == root_value ||
            (_left != nullptr && _left->valueIn(value)) ||
            (_right != nullptr && _right->valueIn(value));
}

///////////////////////////////////////////////////////////////////////////////
// Scan vector for first ocurrence of value, then build path back up to root
std::string
PtrTree::pathTo(value_t value) const
{
    if(value == root_value){
        return "";
    }
    else if(_left != nullptr && _left->valueIn(value)){
        return "L" + _left->pathTo(value);
    }
    else if(_right != nullptr && _right->valueIn(value)){
        return "R" + _right->pathTo(value);
    }
    else{
        throw std::runtime_error("Value not found!");
    }
}


///////////////////////////////////////////////////////////////////////////////
// Follow path from root to end of string, returning value when done
PtrTree::value_t
PtrTree::getByPath(const std::string& path) const
{
    if(path.size() == 0){
        return root_value;
    }
    else if(path[0] == 'L'){
        if(_left == nullptr){
            throw std::runtime_error("Path points to invalid part of tree!");
        }
        return _left->getByPath(std::string(path.begin()+1,path.end()));
    }
    else if(path[0] == 'R'){
        if(_right == nullptr){
            throw std::runtime_error("Path points to invalid part of tree!");
        }
        return _right->getByPath(std::string(path.begin()+1,path.end()));
    }
    else{
        throw std::runtime_error("Invalid character in path: " + path);
    }
}


} // namespace
