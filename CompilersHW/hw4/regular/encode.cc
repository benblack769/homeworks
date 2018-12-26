#include "huffman.hh"

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

vector<FreqEntry> count_frequencies(ifstream & file,uint64_t file_size){
    FrequencyTable table;
    char c;
    for(uint64_t i = 0; i < file_size; i++){
        file.read(&c,sizeof(c));
        table.add_char(c);
    }
    return table.output_frequencies();
}

void encode_huffman(vector<FreqEntry> freqs, ifstream & in_file, ofstream & output_file, uint64_t file_size){
    HuffmanEncoder encoder(freqs);
    if(encoder.total_file_size() < encoder.encoding_size()){
        output_file << 'O';
        output_file << in_file.rdbuf();
    }
    else{
        char output_label = 'C';
        output_file.write(&output_label,sizeof(output_label));
        encoder.save_binary(output_file);
        BitOstream out_bits(&output_file);
        char c;
        for(uint64_t i = 0; i < file_size; i++){
            in_file.read(&c,sizeof(c));
            encoder.encode_element(out_bits,c);
        }
        out_bits.flush();
    }
}

int main(int argc, char ** argv){
    if(argc != 3){
        cout << "usage: \nencode <in_filename> <out_filename>\n";
        return -1;
    }
    char * in_filename = argv[1];
    char * out_filename = argv[2];
    ifstream in_file(in_filename,ios::binary);
    ofstream out_file(out_filename,ios::binary);
    if(!in_file.is_open()){
        throw runtime_error("file didn't open");
    }
    uint64_t file_size = filesize(in_filename);
    vector<FreqEntry> frequencies = count_frequencies(in_file,file_size);
    in_file.clear();
    in_file.seekg(0,in_file.beg);
    encode_huffman(frequencies,in_file,out_file,file_size);
}
