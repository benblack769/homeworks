#include <iostream>
using namespace std;

constexpr double CURRENT_DOLLAR_EURO_CONVERSION_RATIO = 1.24;
constexpr double CURRENT_EURO_DOLLAR_CONVERSION_RATIO = 1.0/CURRENT_DOLLAR_EURO_CONVERSION_RATIO;

double euros_to_dollars(double dollars){
  return dollars*CURRENT_EURO_DOLLAR_CONVERSION_RATIO;
}
int64_t round_down(double x){
  return x >= 0 ? x : x;
}
void up_down_helper(int64_t n,int64_t i){
  if(i == n){
    cout << n << "\n";
  }
  else if(i < n){
    cout << i << "\n";
    up_down_helper(n,i+1);
    cout << i << "\n";
  }
}
void up_down(int64_t n){
  up_down_helper(n,1);
}
int64_t sum_sequence(int64_t n){
  return n >= 0 ? (n * (n+1)) / 2 : 0;
}

int main(){
  cout << "euros_to_dollars:" << endl;
  cout << euros_to_dollars(20) << endl;
  cout << "round_down:" << endl;
  cout << round_down(0.1) << endl;
  cout << round_down(-0.1) << endl;
  cout << round_down(0) << endl;
  cout << round_down(123) << endl;
  cout << round_down(122.99) << endl;
  cout << round_down(-123.1) << endl;
  cout << round_down(-11.9) << endl;
  cout << "up_down:" << endl;
  up_down(10);
  up_down(-1);
  up_down(1);
  cout << "sum_sequence:" << endl;
  cout << sum_sequence(-1) << endl;
  cout << sum_sequence(10) << endl;
  cout << sum_sequence(100000000) << endl;
}
