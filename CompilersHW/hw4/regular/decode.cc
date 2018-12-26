#include "huffman.hh"

void decode_huffman(ifstream & in_file, ofstream & output_file){
    char label;
    in_file.read(&label,sizeof(label));
    if(label == 'O'){
        output_file << in_file.rdbuf();
    }
    else if(label == 'C'){
        HuffmanDecoder decoder(in_file);
        BitIstream in_stream(&in_file);
        while(!decoder.has_ended()){
            char c = decoder.decode_element(in_stream);
            output_file.write(&c, sizeof(c));
        }
    }
    else{
        throw runtime_error("bad file label");
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
    if(!in_file.is_open()){
        throw runtime_error("input file didn't open");
    }
    ofstream out_file(out_filename,ios::binary);

    if(!out_file.is_open()){
        throw runtime_error("output file didn't open");
    }
    decode_huffman(in_file,out_file);
}
