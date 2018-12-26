#include "cache.h"
#include <string.h>
#include <time.h>
#include <cstdio>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <thread>
#include <iostream>
#include <array>
#include <algorithm>
#include <unistd.h>
#include <unordered_set>

using namespace std;

constexpr uint64_t MILS_PER_NANO = 1000000;

uint64_t get_time_ns();
discrete_distribution<uint64_t> init_dist();

//variables to keep HW8 comparable and consistent
constexpr bool USE_CONST_SIZES = false;
constexpr size_t CONST_KEY_SIZE = 8;
constexpr size_t CONST_VALUE_SIZE = 16;

constexpr uint64_t num_iterations = 3000000;

constexpr uint64_t ns_in_s = 1000000000ULL;
constexpr uint64_t mem_to_store = 10000000ULL;
constexpr uint64_t maxmem = (USE_CONST_SIZES ? 0.95 : 0.90) * mem_to_store;
constexpr double APROX_MEAN_WEIGHTED_VALUE_SIZE = USE_CONST_SIZES ? CONST_VALUE_SIZE : 461.258;//measured with unig.cpp
constexpr uint64_t tot_num_items = uint64_t(mem_to_store / APROX_MEAN_WEIGHTED_VALUE_SIZE);
constexpr size_t NUM_THREADS = 127;//at most two minus the number of ports of the server


string values[tot_num_items];
string keys[tot_num_items];

using gen_ty = std::default_random_engine;

uint64_t rand_key_size(gen_ty & generator){
	normal_distribution<double> norm_dsit(30,8);
	return USE_CONST_SIZES ? CONST_KEY_SIZE : max(int64_t(2),int64_t(norm_dsit(generator)));
}

uint64_t rand_val_size(gen_ty & generator){
	static discrete_distribution<uint64_t> val_dist = init_dist();
	//todo add large values to distribution
	return USE_CONST_SIZES ? CONST_VALUE_SIZE : val_dist(generator);
}
char rand_char(gen_ty & generator){
    uniform_int_distribution<char> lower_lettters_dist(97,96+26-1);//lower case letters
	return lower_lettters_dist(generator);
}
uint64_t uniform_rand_item(gen_ty & generator){
	uniform_int_distribution<uint64_t> unif_dist(0,tot_num_items-1);
	return unif_dist(generator);
}
string rand_str(gen_ty & generator,uint64_t size){
	string str(size,' ');
    for(uint64_t i = 0; i < size; i++){
		str[i] = rand_char(generator);
	}
	return str;
}
key_type to_key(string & val){
	return reinterpret_cast<key_type>(val.c_str());
}
val_type to_val(string & val){
	return reinterpret_cast<val_type>(val.c_str());
}
void init_values(gen_ty & generator,uint64_t num_items){
	for(uint64_t i = 0; i < num_items; i++){
		values[i] = rand_str(generator,rand_val_size(generator));
	}
}
void init_keys(gen_ty & generator,uint64_t num_items){
	unordered_set<string> all_keys;
	all_keys.reserve(num_items);
	for(uint64_t i = 0; i < num_items; i++){
		//gauentees key uniqueness
		string gen_str;
		do{
			gen_str = rand_str(generator,rand_key_size(generator));
		}
		while(all_keys.count(gen_str));
		keys[i] = gen_str;
		all_keys.insert(gen_str);
	}
}
struct action_data{
	uint64_t time;
	uint64_t hits;
	uint64_t count;
	uint64_t hit_count;
	action_data(uint64_t intime=0,uint64_t incount=0,uint64_t inhits=0,uint64_t inhitcount=0){
		time = intime;
		hits = inhits;
		count = incount;
		hit_count = inhitcount;
	}
	action_data & operator += (action_data other){
		time += other.time;
		hits += other.hits;
		count += other.count;
		hit_count += other.hit_count;
	}
	double get_av_ms_time(){
		return time / double(count * MILS_PER_NANO);
	}
	double get_hit_rate(){
		return hits / double(hit_count);
	}
};
template<typename timed_fn_ty>
uint64_t timeit(timed_fn_ty timed_fn){
	uint64_t start = get_time_ns();
	timed_fn();
	return get_time_ns() - start;
}
action_data get_action(gen_ty & generator,cache_t cache){
	uint64_t get_item = uniform_rand_item(generator);
	string & item = keys[get_item];
	val_type v = nullptr;
	uint64_t time = timeit([&](){
		uint32_t null_val = 0;
		v = cache_get(cache,to_key(item),&null_val);
	});
	bool hit = v != nullptr;
	if(hit){
		if(strcmp((const char *)v,values[get_item].c_str())){
			printf("value returned not what expected:\n%s\n%s\n\n",v,values[get_item].c_str());
		}
	}
	return action_data(time,1,hit,1);
}
action_data delete_action(gen_ty & generator,cache_t cache){
	string & item = keys[uniform_rand_item(generator)];
	return action_data(timeit([&](){
		cache_delete(cache,to_key(item));
	}),1);
}
action_data set_action(gen_ty & generator,cache_t cache){
	uint64_t item_num = uniform_rand_item(generator);
	string & key = keys[item_num];
	string & value = values[item_num];
	return action_data(timeit([&](){
		cache_set(cache,to_key(key),to_val(value),value.size()+1);
	}),1);
}
cache_t get_cache(int64_t tcp_portnum,int64_t udp_portnum){
	tcp_port = to_string(tcp_portnum);
	udp_port = to_string(udp_portnum);
	return get_cache_connection();
}
action_data rand_action_time(gen_ty & generator,cache_t cache){
	uniform_real_distribution<double> occurs_dis(0,100);//lower case letters
	double distri_val = occurs_dis(generator);

	//if(distri_val < 0){
	//	return delete_action(generator,cache);
	//} else
	if(distri_val < 30){
		return set_action(generator,cache);
	}
	else{
		return get_action(generator,cache);
	}
}
action_data run_rand_actions(gen_ty & generator,cache_t cache,uint64_t num_actions){
	action_data data;
	for(int i = 0; i < num_actions; i++){
		data += rand_action_time(generator,cache);
	}
	return data;
}
action_data arr[NUM_THREADS];
void run_requests(cache_t cache,uint64_t t_num,uint64_t num_actions){
	gen_ty generator(get_time_ns()+t_num);
	arr[t_num] = run_rand_actions(generator,cache,num_actions);
}
action_data time_threads(uint64_t tcp_port_start,uint64_t udp_port_start,uint64_t tot_num_actions){
	thread ts[NUM_THREADS];
	cache_t caches[NUM_THREADS];
	for(uint64_t t_n = 0; t_n < NUM_THREADS; t_n++){
		caches[t_n] = get_cache(tcp_port_start+t_n,udp_port_start+t_n);
		ts[t_n] = thread(run_requests,caches[t_n],t_n,tot_num_actions/NUM_THREADS);
	}
	for(uint64_t t_n = 0; t_n < NUM_THREADS; t_n++){
		ts[t_n].join();
		end_connection(caches[t_n]);
	}
	action_data sum_data;
	for(uint64_t t_n = 0; t_n < NUM_THREADS; t_n++){
		sum_data += arr[t_n];
	}
	return sum_data;
}
void populate_cache(cache_t cache){
	for(size_t i = 0; i < tot_num_items; i++){
		cache_set(cache,to_key(keys[i]),to_val(values[i]),values[i].size()+1);
	}
}
void get_port_start(int argc,char ** argv,bool & is_addon,uint64_t & tcp_port,uint64_t & udp_port){
	tcp_port = 10700;
    udp_port = 10900;
	is_addon = false;
	char c;
    while ((c = getopt(argc, argv, "ap:u:")) != -1) {
        switch (c) {
        case 'p':
            tcp_port = atoi(optarg);
            break;
        case 'u':
            udp_port = atoi(optarg);
            break;
        case 'a':
            is_addon = true;
            break;
        default:
            cout << "bad command line argument";
            exit(1);
        }
    }
}
//in order to start a client that does not make a new cache, just have set a random command line argument
int main(int argc,char ** argv){
	gen_ty generator(1);
	init_values(generator,tot_num_items);
	init_keys(generator,tot_num_items);

	uint64_t tcp_start,udp_start;
	bool is_addon = false;
	get_port_start(argc,argv,is_addon,tcp_start,udp_start);
	cout << "values generated" << endl;
	uint64_t init_tcp_port = tcp_start+NUM_THREADS+1;
	uint64_t init_udp_port = udp_start+NUM_THREADS+1;
	if (!is_addon){
	    tcp_port = to_string(init_tcp_port);
	    udp_port = to_string(init_udp_port);
		cache_t cache = create_cache(maxmem,NULL);
		cout << "connection created" << endl;

	    populate_cache(cache);
		cout << "values populated" << endl;

		end_connection(cache);
	}
	cout << "connection ended" << endl;
	//busywork while waiting for other client timers to be manually connected
	//time_threads(tcp_start,udp_start,1000000);
	//cout << "busywork finished" << endl;
	//actual timing
	action_data data = time_threads(tcp_start,udp_start,num_iterations);
	double av_ms_time = data.get_av_ms_time();
	double hit_rate = data.get_hit_rate();

	cout << "number of threads = " << NUM_THREADS << "\n";
	cout << "average time in ms = " << av_ms_time << "\n";
	cout << "average throughput = " <<  NUM_THREADS / (av_ms_time / 1000.0) << "\n";
	cout << "hit rate = " << hit_rate << endl;

	//more busywork while the other client is completeing
	//time_threads(tcp_start,udp_start,1000000);

	if(!is_addon){
		cache_t cache = get_cache(init_tcp_port+1,init_udp_port+1);
		destroy_cache(cache);//closes server
	}
	return 0;
}

uint64_t get_time_ns(){
	struct timespec t;
	clock_gettime(0,&t);
 	return 1000000000ULL * t.tv_sec + t.tv_nsec;
}
void normalize(double to_val,vector<double>::iterator valbeg,vector<double>::iterator valend){
	double sum = 0;
	for_each(valbeg,valend,[&](double val){
		sum += val;
	});
	for_each(valbeg,valend,[=](double & val){
		val *= to_val * (1.0/sum);
	});
}
discrete_distribution<uint64_t> init_dist(){
	const uint64_t fhd = 500;
	vector<double> vals;
	vals.push_back(0);
	vals.push_back(0);
	for(uint64_t i = 2; i <= fhd; i++){
		vals.push_back(1);
	}
	for(uint64_t i = fhd+1;i <= 30000;i++){
		vals.push_back(pow(10,-2.341611959e-4 * i));
	}
	normalize(0.9,vals.begin(),vals.begin() + fhd+1);
	normalize(0.1,vals.begin()+fhd+1,vals.end());
	return discrete_distribution<uint64_t> (vals.begin(),vals.end());
}
