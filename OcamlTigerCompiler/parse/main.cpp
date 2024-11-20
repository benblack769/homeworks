#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>

using namespace std;

constexpr int num_symbols = 2;
enum Symbols { EXPR = 0, PROD, LITERAL, PLUS, TIMES, RIGHTPAREN, LEFTPAREN };

bool is_terminal(Symbols x){
    return x > num_symbols;
}
int symbol_of(Symbols x){
    assert(!is_terminal(x));
    return x;
}

vector<vector<vector<Symbols>>> grammar = {
    {//EXPR
        { PROD, PLUS, EXPR },
        { PROD },
    },
    {//PROD
        { LITERAL, TIMES, PROD },
        { LITERAL },
        { LEFTPAREN, EXPR, RIGHTPAREN }
    },
};



int main(){

}
