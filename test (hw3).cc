#define CATCH_CONFIG_MAIN
#include "cache.hh"
#include <cassert> 
#include <ctime> 
#include <string> 
#include "catch.hpp"

using namespace std; 


TEST_CASE("set_value")
{          
        Cache c(10000);
        void* a;
        void* v;
        uint32_t i, vp;
        i = 25;
        a = (uint32_t*)&c;
        vp = 2;
        c.set("alien", a, vp);
        REQUIRE(c.get("alien", vp) == a);
}


TEST_CASE("overwrite_value")
{
        Cache c(10000);
        void* a;
        void* b;
        uint32_t i, j, vp;
        i = 1000;
        j = 22;
        a = (uint32_t*)&i;
        b = (uint32_t*)&j;
        vp = 3;
        c.set("pumpkin", a, vp);
        vp = 1;
        c.set("pumpkin", b, vp);
        REQUIRE(c.get("pumpkin", vp) == b);
}


TEST_CASE("get_value_from_empty")
{       
        Cache c(10000);
        uint32_t vp;
        vp = 2;
        REQUIRE(c.get("monster", vp) == NULL);
}


TEST_CASE("oversize_test")
{       
        Cache c(10000);
        void* a;
        uint32_t i, vp;
        i = 1000;
        a = (uint32_t*)&i;
        vp = 10001;
        c.set("witch", a, vp);
        REQUIRE(c.get("witch", vp) == NULL);
}


TEST_CASE("delete_value_test")
{       
        Cache c(10000);
        void* a;
        uint32_t i, vp;
        i = 13;
        a = (uint32_t*)&i;
        vp = 1;
        c.set("ghosts", a, vp);
        c.del("ghosts");
        REQUIRE(c.get("ghosts", vp) == NULL);
}
    
TEST_CASE("delete_and_replace")
{       
        Cache c(10000);
        void* a;
        void* b;
        uint32_t i, j, vp;
        i = 666;
        j = 16;
        a = (uint32_t*)&i;
        b = (int32_t*)&j;
        vp = 3;
        c.set("ghoul", a, vp);
        c.del("ghoul");
        vp = 1;
        c.set("ghoul", b, vp);
        REQUIRE(c.get("ghoul", vp) == b);
}

TEST_CASE("delete_empty")
{       
        Cache c(10000);
        uint32_t vp;
        c.del("spider");
        REQUIRE(c.get("spider", vp) == NULL);
}

TEST_CASE("value_size_test")
{       
        Cache c(10000);
        void* a;
        uint32_t i, vp;
        i = 4;
        a = (uint32_t*)&i;
        vp = 1;
        c.set("skeleton", a, vp);
        uint32_t badvp = 12;
        const void* dummyvar = c.get("skeleton", badvp);
        dummyvar = dummyvar;
        REQUIRE(badvp == 1);
}

TEST_CASE("spaced_used_update")
{       
        Cache c(10000);
        void* a;
        uint32_t i, vp;
        uint32_t oldstored = c.space_used();
        i = 31;
        a = (uint32_t*)&i;
        vp = 2;
        c.set("vampire", a, vp);
        uint32_t newstored = c.space_used();
        REQUIRE(oldstored + 2 == newstored);
}
