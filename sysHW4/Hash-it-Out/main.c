// Alex Pan
//
// main.c
//
// This is the file that runs all our tests. I stuck the main() in here to
// keep the other files cleaner.

int main(){
	
	//////////////////////// testing lru eviction //////////////////////
	testing_lru_add();
	testing_lru_update();
	testing_lru_delete();
	testing_create_evict();

	//////////////////////////// testing cache //////////////////////////
	testing_create_cache();
	testing_cache_set();
	testing_cache_get();
	testing_cache_delete();
	testing_cache_space_used();
	
	return 0;
}
