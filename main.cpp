#include"threadpool.h"
void test_proc(void *args) {  
    usleep(100*1000); 
	int *iID = (int*)args;
    cout << pthread_self()<<"param:" << iID << endl;  
}  
  
int main() {  
    // Test  
    ThreadPool *tp = new ThreadPool;  
    tp->init(5000);  
  
    for(int i=0; i<100; ++i) {  
        tp->add_event(test_proc, reinterpret_cast<void*>(i));  
    }  
  
    sleep(20);  
    tp->destroy();  
    return 0;  
}
