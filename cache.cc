/*
 * Implementation for a generic cache object.
 * Data is given as blobs (void *) of a given size,
 * and indexed by a string key.
 */
#include <unordered_map>
#include <queue>
#include "cache.hh"
#include <cassert>
#include <ctime>
#include <string>

/*
 * Generic custom hash function that returns the ascii value of the given string 
 */
struct myHash
{
    std::size_t operator()(std::string s)
    { return stoi(s);};
};





/*
 * We want to create a an Impl struct so that we can have a pImple pointer.
 * This allows us to reduce compilation time and alter the private members 
 * while remaining independent from the header file / interface.
 */



struct Cache::Impl
{
    // This struct has the following private members
    uint32_t maxmem_; //maximum memory allowed for our 'cache'
    evictor_type dummy_; //fake evictor to deal with the .hh file
    hash_func hasher_; //hasher related to our map members
    uint32_t memused_; // running total of how much memory has been used
    std::unordered_map<std::string, void*> cache_map;
    std::unordered_map<const void*, uint32_t> size_storage;
    /*This is the code for the FIFO evictor; I've implemented it as a struct using a queue
    so that the implementation can be changed without requiring changes in the rest of the code*/
    struct EvictorStruct
    {   
        std::queue <std::string> droplist;
        void addkey(std::string newkey)
        {   
            droplist.push(newkey);
        }

        std::string pushoff()
        {
            std::string kickoff = droplist.front();
            droplist.pop();
            return kickoff;
        }
    };
    EvictorStruct true_evictor_; //type of eviction we are implementing 

  
  public:
    /*
     * Impl constructor 
     */
    Impl(uint32_t maxmem, evictor_type evictor, hash_func hasher = myHash())
        :maxmem_(maxmem), dummy_(evictor), memused_(0),
        hasher_(hasher)
    {
    }
    
    /*
     * Impl destructor 
     */
    ~Impl() = default;

    //Return a pointer to a value associated with a given key
    Cache::val_type get(Cache::key_type key, Cache::index_type& val_size)
    {
        auto it = cache_map.find(key);
        if(it == cache_map.end())
        {
            return NULL;
        }
        void* pval = it->second;
        auto it2 = size_storage.find(pval);
        uint32_t new_size = it2->second;
        val_size = new_size;
        return it->second;
    }


        // Delete a <key, value> pair from a given key
    void del(std::string key)
    {
        auto it = cache_map.find(key);
        if( it == cache_map.end())
        {
            return;
        }
        void* pval = it->second;
        cache_map.erase(key);
        auto it2 = size_storage.find(pval);
        if( it2 == size_storage.end())
        {
            return;
        } 
        uint32_t del_size = it2->second;
        size_storage.erase(pval);
        memused_ -= del_size; 
    }


    /*
     * Adds a <key, value> pair to our cache_map 
     */
    void set(Cache::key_type key, Cache::val_type val, Cache::index_type size)
    {
        // If the value we are trying to add is larger than the cache itself, do nothing
        if(sizeof(val) > maxmem_)
        {
            EXIT_SUCCESS;
        }
        /*
         * First check if this key is already in the map. In order to replace the value,
         * we must alter the memory used with regard to the size of the old value to the new value.
         */ 
        auto it = cache_map.find(key);
        if(it != cache_map.end()) //This implies that the key is in cache_map
        {
            val_type v = it->second;
            auto it2 = size_storage.find(v);
            uint32_t d = it2->second;
            memused_ -= d;
        }
        cache_map[key] = (void*)val;
        memused_ += size;
        size_storage[val] = size;
        true_evictor_.addkey(key);


        if(memused_ > maxmem_) //If the memory used exceeds our maximum memory, we must evict something from the cache
        {   
            bool noevic = true;
            EvictorStruct s = true_evictor_;
            Cache::index_type dummy_val;
            Cache::key_type k;
            while(noevic) {
                Cache::key_type k = s.pushoff();  //returns index to delete, given by evictor
                if (get(k, dummy_val) != NULL) // this determines if the evicttee has already been deleted
                {
                    noevic = false;
                    del(k);
                }
            }                       
        }
    }

    // Returns the amount of memory that has been used
    const Cache::index_type space_used()
    {
        return memused_;
    }
};



// Create a new cache object with a given maximum memory capacity.
Cache::Cache(Cache::index_type maxmem, evictor_type evictor, hash_func hasher)
    :pImpl_(new Impl(maxmem, evictor, hasher))
{
}

Cache::~Cache()
{
}

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void 
Cache::set(Cache::key_type key, Cache::val_type val, Cache::index_type size)
{
    pImpl_->set(key, val, size);
}

// Retrieve a pointer to the value associated with key in the cache,
// or NULL if not found.
// Sets the actual size of the returned value (in bytes) in val_size.
Cache::val_type 
Cache::get(Cache::key_type key, Cache::index_type& val_size) const
{
    return pImpl_->get(key, val_size);
}

// Delete an object from the cache, if it's still there
void 
Cache::del(std::string key)
{
    pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::index_type 
Cache::space_used() const
{
    return pImpl_->space_used();
}

