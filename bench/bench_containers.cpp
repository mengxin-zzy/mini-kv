#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include "skiplist.h"

using Clock = std::chrono::steady_clock;

int main(){
    const int N = 200000;
    //unordered_map测试模块
    {
        std::unordered_map<std::string, std::string> m;
        auto t0 = Clock::now();
        for(int i = 0; i < N; i++){
            m["k" + std::to_string(i)] = "v";
        }
        auto t1 = Clock::now();
        
        size_t hit = 0;
        for(int i = 0; i < N; i++){
            if (m.count("k" + std::to_string(i))) hit++;
        }
        auto t2 = Clock::now();
        std::cout << "hash  insert: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms"
                  << "   find: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms"
                  << "  (hit " << hit << ")\n";
    }

    //skiplist测试模块
    {
        SkipList s;
        auto t0 = Clock::now();
        for (int i = 0; i < N; i++){
            s.Insert("k" + std::to_string(i), "v");
        }
        auto t1 = Clock::now();

        size_t hit = 0;
        for (int i = 0; i < N; i++){
            if(s.Find("k" + std::to_string(i)).has_value())++hit;
        }
        auto t2 = Clock::now();
        std::cout << "skip  insert: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms"
                  << "   find: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms"
                  << "  (hit " << hit << ")\n";
        
    }
}