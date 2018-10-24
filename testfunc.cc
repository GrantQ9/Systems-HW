using namespace std;
#include "cache.hh"
#include <cassert>
#include <ctime>
#include <string>

/* tests if set and get work correctly together */
void set_get_test(Cache* testCache, std::string name, uint32_t& size, void* value)
{     

      testCache->set(name, value, sizeof(value)); 
      assert (testCache->get(name, size) == value);
}

/* tests if values are written over correctly */
void overwrite_value_test(Cache* testCache, std::string name,
     uint32_t& size, void* startvalue, void* endvalue)
{
      testCache->set(name, startvalue, sizeof(startvalue)); 
      testCache->set(name, endvalue, sizeof(endvalue)); 
      assert (testCache->get(name, size) == endvalue);
}
/* Tests if empty caches give null on runs of get*/
void empty_get_test(Cache* emptyCache, std::string name, uint32_t& val_size)
{     

      assert (emptyCache->get(name, val_size) == NULL);
}
/* Tests if items with too big a size are rejected */
void oversize_test(Cache* testCache, std::string name, void* value, uint32_t cachesize, uint32_t& val_size)
{
    testCache->set(name, value, cachesize+1);
    assert (testCache->get(name, val_size) == NULL);
}


/* Tests if deleting a value makes get return null given that value*/
void delete_get_test(Cache* testCache, std::string name, void* value, uint32_t val_size)
{
      testCache->set(name, value, sizeof(value)); 
      testCache->del(name);
      assert (testCache->get(name, val_size) == NULL);
}
/* Tests if deleting and then replacing a value works as intended*/
void delete_replace_test(Cache* testCache, std::string name, void* startvalue, 
     uint32_t& val_size, void* endvalue)
{
      testCache->set(name, startvalue, sizeof(startvalue)); 
      testCache->del(name);
      testCache->set(name, endvalue, sizeof(endvalue)); 
      assert (testCache->get(name, val_size) == endvalue);
}

/* Tests if deleting a name value pair not in the cache works */
void delete_empty_test(Cache* testCache, const std::string name, uint32_t val_size)
{
     testCache->del(name);
     assert (testCache->get(name, val_size) == NULL);
}


/* Tests if val_size gets the correct number */
void val_size_test(Cache* testCache, std::string name, uint32_t& val_size, void* value, uint32_t true_size)
{
    testCache->set(name, value, true_size); 
    const void* dummyvar = testCache->get(name, val_size);
    assert(true_size == val_size);
}

/* Tests if space_used() updates correctly when values are added (assuming there is room) */
void space_used_update(Cache* testCache, std::string name, uint32_t val_size, void* value)
{
    uint32_t oldstored = testCache->space_used();
    testCache->set(name, value, val_size); 
    uint32_t newstored = testCache->space_used();
    assert (oldstored + val_size == newstored);
}
/* Tests if FIFO works as intended, removing the first entered element of the cache when full */
void FIFO_works(Cache* testCache, void* i1, void* i2, void* i3) {\
    uint32_t v_size;
    testCache->set("tiny", i1, sizeof(i1)); 
    testCache->set("big", i2, sizeof(i2)); 
    testCache->set("enough", i3, sizeof(i3));
    assert (testCache->get("enough", v_size) == i3);
    assert (testCache->get("big", v_size) == i2);
    assert (testCache->get("tiny", v_size) == NULL);
}



int main()
{   
    /* Cache and variables are built between function calls */
    Cache cachebase(10000);
    Cache* cache1;
    cache1 = (Cache*)&cachebase;
    /* For functions where val_size is not used, we have a dummy vp here */
    uint32_t vp;
    delete_empty_test(cache1, "nothere", vp);
    empty_get_test(cache1, "meneither", vp);
    int16_t t1 = 100;
    void* t1v = (int*)&t1;
    set_get_test(cache1, "onehundred", vp, t1v);
    string t2 = "alpha";
    void* t2v = (string*)&t2;
    string t3 = "omega";
    void* t3v = (string*)&t3;
    overwrite_value_test(cache1, "alphatoomega", vp, t2v, t3v);
    bool t4 = true;
    void* t4v = (bool*)&t4;
    bool t5 = false;
    void* t5v = (bool*)&t5;
    delete_replace_test(cache1, "timeschangin",t4v, vp, t5v);
    delete_get_test(cache1, "junkozymandius", t5v, vp);
    bool scouter = true;
    void* t6v = (bool*)&scouter;
    val_size_test(cache1, "big", vp, t6v, 1);
    double num = 1000000;
    void* t7v = (double*)&num;
    space_used_update(cache1, "topper", 8, t7v);
    Cache cachenew(17);
    Cache* cache2;
    cache2 = (Cache*)&cachenew;
    FIFO_works(cache2, t4v, t6v, t1v);
}