#include <queue>
#include "huffman.hh"
#include "ptrtree.hh"
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace huffman {

constexpr size_t CHAR_SET_SIZE = 256;
using count_t = uint64_t;

constexpr tree::Tree::value_t EOF_VALUE = CHAR_SET_SIZE;
constexpr tree::Tree::value_t NODE_VALUE = CHAR_SET_SIZE+1;

bool is_charachter(tree::Tree::value_t value){
    return value < CHAR_SET_SIZE;
}

struct HuffPair{
    tree::PtrTree tree;
    count_t freq;
};

bool operator < (const std::unique_ptr<HuffPair> & one,const std::unique_ptr<HuffPair> & other){
    //this reversed so that the priority_queue in Huffman::Impl::buildTree is a min queue instead of a max queue.
    return one->freq > other->freq;
}
void verify_encoding(Huffman::encoding_t enc){
    for(Huffman::bit_t b : enc){
        assert(b == Huffman::ZERO || b == Huffman::ONE);
    }
}
Huffman::encoding_t path_to_encoding(std::string path){
    Huffman::encoding_t res(path.size());
    std::transform(path.begin(), path.end(), res.begin(), [](char dir){
        return dir == 'L' ? Huffman::ZERO : Huffman::ONE;
    });
    return res;
}
std::string encoding_to_path(Huffman::encoding_t path){
    std::string res(path.size(),' ');
    std::transform(path.begin(), path.end(), res.begin(), [](char dir){
        return dir == Huffman::ZERO ? 'L' : 'R';
    });
    return res;
}
struct Huffman::Impl{
    std::array<count_t,CHAR_SET_SIZE> frequencies;
    Impl(){
        frequencies.fill(0);
    }

    tree::PtrTree buildTree(){
        std::priority_queue<std::unique_ptr<HuffPair>> queue;
        for(size_t i = 0; i < CHAR_SET_SIZE; i++){
            queue.push(std::unique_ptr<HuffPair>(new HuffPair{tree::PtrTree(Huffman::symbol_t(i)),frequencies[i]}));
        }
        //put in EOF code into tree
        queue.push(std::unique_ptr<HuffPair>(new HuffPair{tree::PtrTree(EOF_VALUE),0}));

        while(queue.size() > 1){
            HuffPair top1(*queue.top());
            queue.pop();
            HuffPair top2(*queue.top());
            queue.pop();

            count_t new_freq = top1.freq + top2.freq;
            queue.push(std::unique_ptr<HuffPair>(new HuffPair{tree::PtrTree(NODE_VALUE,top1.tree,top2.tree),new_freq}));
        }
        return queue.top()->tree;
    }

    void incFreq(Huffman::symbol_t symb){
        frequencies[symb]++;
    }
};


Huffman::Huffman() noexcept:
    pImpl_(new Impl){

}
Huffman::~Huffman() noexcept{
}

void Huffman::incFreq(symbol_t symbol){
    pImpl_->incFreq(symbol);
}
Huffman::encoding_t Huffman::encode(symbol_t symbol) const{
    return path_to_encoding(pImpl_->buildTree().pathTo(symbol));
}
Huffman::symbol_t Huffman::decode(enc_iter_t& begin, const enc_iter_t& end) const noexcept(false){
    Huffman::encoding_t input(begin,end);
    verify_encoding(input);
    tree::Tree::value_t value = pImpl_->buildTree().getByPath(encoding_to_path(input));
    if(value == EOF_VALUE){
        begin = end;
        return symbol_t(0);
    }
    else if(is_charachter(value)){
        begin += pImpl_->buildTree().pathTo(value).size();
        return symbol_t(value);
    }
    else{
        throw std::runtime_error("bad value decoded from tree");
    }
}
Huffman::encoding_t Huffman::eofCode() const{
    return path_to_encoding(pImpl_->buildTree().pathTo(EOF_VALUE));
}

}//namespace
