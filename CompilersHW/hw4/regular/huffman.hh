#include <queue>
#include <cassert>
#include <stdint.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>

using namespace  std;

constexpr size_t CHAR_TABLE_SIZE = 256;
using count_t = uint64_t;


struct FreqEntry{
    uint8_t c;
    count_t count;
    bool operator < (FreqEntry other)const{
        return count < other.count;
    }
};

class FrequencyTable{
protected:
    array<count_t, CHAR_TABLE_SIZE> frequencies;
public:
    FrequencyTable(){
        frequencies.fill(0);
    }
    void add_char(uint8_t c){
        frequencies[c] += 1;
    }
    vector<FreqEntry> output_frequencies(){
        vector<FreqEntry> res;
        for(size_t i = 0; i < CHAR_TABLE_SIZE; i++){
            if(frequencies[i]){
                res.push_back(FreqEntry{ uint8_t(i), frequencies[i] });
            }
        }
        return res;
    }
};
class FlatHuffmanNode{
private:
    bool is_node;
    uint8_t value;
    uint16_t left;
    uint16_t right;
public:
    FlatHuffmanNode(){
        is_node = false;
        value = 0;
        left = -1;
        right = -1;
    }
    FlatHuffmanNode(uint8_t in_value){
        is_node = false;
        value = in_value;
        left = -1;
        right = -1;
    }
    FlatHuffmanNode(uint16_t left_loc, uint16_t right_loc){
        is_node = true;
        value = 0;
        left = left_loc;
        right = right_loc;
    }
    bool is_char()const{
        return !is_node;
    }
    uint8_t get_value()const{
        assert(is_char());
        return value;
    }
    uint16_t get_left()const{
        assert(!is_char() && left != uint16_t(-1));
        return left;
    }
    uint16_t get_right()const{
        assert(!is_char() && right != uint16_t(-1));
        return right;
    }
};

struct QueueEntry{
    uint16_t node;
    count_t count;
    bool operator < (QueueEntry other)const{
        return count > other.count;
    }
};
enum CharEncoding { LEFT=0, RIGHT=1 };

class HuffmanTree{
protected:
    vector<FlatHuffmanNode> tree_entries;

    HuffmanTree(){

    }
    uint16_t build_node(FlatHuffmanNode entry){
        tree_entries.push_back(entry);
        return uint16_t(tree_entries.size()-1);
    }
public:
    HuffmanTree(vector<FreqEntry> frequencies){
        if(frequencies.size() == 0){
            return;
        }
        std::priority_queue<QueueEntry> queue;
        for(size_t i = 0; i < frequencies.size(); i++){
            queue.push(QueueEntry{ build_node(FlatHuffmanNode(frequencies[i].c)), frequencies[i].count });
        }
        while(queue.size() > 1){
            QueueEntry top1 = queue.top();
            queue.pop();
            QueueEntry top2 = queue.top();
            queue.pop();

            uint16_t next_node = build_node(FlatHuffmanNode(top1.node, top2.node));
            QueueEntry next_entry = { next_node, top1.count + top2.count };

            queue.push(next_entry);
        }
    }
protected:
    void save_binary_tree(ostream & binary_file){
        size_t tree_size = tree_entries.size();
        binary_file.write((char *)(&tree_size),sizeof(tree_size));
        binary_file.write((char *)(tree_entries.data()),sizeof(tree_entries[0]) * tree_entries.size());
    }
    void load_binary_tree(istream & binary_file){
        size_t tree_size = 0;
        binary_file.read((char *)(&tree_size),sizeof(tree_size));
        tree_entries.resize(tree_size);
        binary_file.read((char *)(tree_entries.data()),sizeof(tree_entries[0]) * tree_entries.size());
    }
protected:
    uint16_t root_loc(){
        return tree_entries.size()-1;
    }
};
void tree_to_bit_strings(const vector<FlatHuffmanNode> & tree, uint16_t tree_pos, string & current_bits, array<string,CHAR_TABLE_SIZE> & table){
    FlatHuffmanNode cur_node = tree[tree_pos];
    if(cur_node.is_char()){
        table[cur_node.get_value()] = current_bits;
    }
    else{
        current_bits.push_back(CharEncoding::LEFT);
        tree_to_bit_strings(tree,cur_node.get_left(),current_bits,table);
        current_bits.pop_back();

        current_bits.push_back(CharEncoding::RIGHT);
        tree_to_bit_strings(tree,cur_node.get_right(),current_bits,table);
        current_bits.pop_back();
    }
}
constexpr int CHAR_BITS = 8;
class BitOstream{
protected:
    ostream * my_file_stream;
    uint8_t chunk;
    int bits_saved;
public:
    BitOstream(ostream * out_stream):
        my_file_stream(out_stream),
        chunk(0),
        bits_saved(0){
    }
    void write_bit(CharEncoding bit){
        assert(bit == LEFT || bit == RIGHT);
        if(bits_saved == CHAR_BITS){
            flush();
        }
        bits_saved++;
        chunk <<= 1;
        uint8_t or_bit = bit == LEFT ? 0 : 1;
        chunk |= or_bit;
    }
    void flush(){
        chunk <<= (CHAR_BITS - bits_saved);
        my_file_stream->write((char*)(&chunk),sizeof(chunk));
        bits_saved = 0;
        chunk = 0;
    }
};
class BitIstream{
protected:
    istream * my_file_stream;
    uint8_t chunk;
    int bits_left;
public:
    BitIstream(istream * in_stream):
        my_file_stream(in_stream),
        chunk(0),
        bits_left(0){
    }
    CharEncoding consume_bit(){
        if(bits_left == 0){
            load_chunk();
        }
        //uint8_t top_bit = chunk & 0x7f;
        uint8_t cur_bit = chunk >> 7;
        chunk <<= 1;
        bits_left -= 1;
        return cur_bit == 0 ? LEFT : RIGHT;
    }
protected:
    void load_chunk(){
        my_file_stream->read((char*)(&chunk),sizeof(chunk));
        bits_left = CHAR_BITS;
    }
};
class HuffmanEncoder:
        public HuffmanTree{
protected:
    array<string,CHAR_TABLE_SIZE> tree_encodings;
    vector<FreqEntry> frequencies;
public:
    HuffmanEncoder(vector<FreqEntry> in_frequencies):
        HuffmanTree(in_frequencies){
        frequencies = in_frequencies;
        string oper_str;
        tree_to_bit_strings(tree_entries,root_loc(),oper_str,tree_encodings);
    }
    void encode_element(BitOstream & stream, uint8_t element){
        for(char c : tree_encodings[element]){
            assert(c == LEFT || c == RIGHT);
            stream.write_bit(CharEncoding(c));
        }
    }
    count_t encoding_size(){
        //exact output size in bytes based on frequency table
        count_t total_size = 0;
        total_size += sizeof(char);//for the initial byte
        total_size += sizeof(count_t); //for number of charachters in file (needed to keep binary format consistent)
        total_size += sizeof(size_t); //for tree size
        total_size += sizeof(tree_entries[0]) * tree_entries.size(); // for tree data

        //actual compressed bits size
        count_t bit_size = 0;
        for(FreqEntry entry : frequencies){
            bit_size += tree_encodings[entry.c].size() * entry.count;
        }
        count_t byte_size = (bit_size + CHAR_BITS - 1) / CHAR_BITS;
        total_size += byte_size;

        return total_size;
    }
    count_t total_file_size(){
        return std::accumulate(frequencies.begin(), frequencies.end(), count_t(0),
                               [](count_t one, FreqEntry two){return two.count + one; });
    }
    void save_binary(ostream & binary_file){
        count_t orig_file_size = total_file_size();
        binary_file.write((char *)(&orig_file_size),sizeof(orig_file_size));
        save_binary_tree(binary_file);
    }
protected:
};

class HuffmanDecoder:
        public HuffmanTree{
protected:
    count_t orig_file_size;
    count_t cur_pos;
public:
    HuffmanDecoder(istream & binary_file):
        HuffmanTree(){
        cur_pos = 0;
        load_binary(binary_file);
    }
    uint8_t decode_element(BitIstream & stream){
        cur_pos += 1;
        return decode_element_help(stream,root_loc());
    }
    bool has_ended(){
        return cur_pos >= orig_file_size;
    }
protected:
    uint8_t decode_element_help(BitIstream & stream, uint16_t cur_tree_pos){
        FlatHuffmanNode node = this->tree_entries[cur_tree_pos];
        if(node.is_char()){
            return node.get_value();
        }
        else{
            CharEncoding code = stream.consume_bit();
            if(code == LEFT){
                return decode_element_help(stream,node.get_left());
            }
            else{
                return decode_element_help(stream,node.get_right());
            }
        }
    }
    void load_binary(istream & binary_file){
        binary_file.read((char *)(&orig_file_size),sizeof(orig_file_size));
        load_binary_tree(binary_file);
    }
};
