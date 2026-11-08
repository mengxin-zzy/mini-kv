#pragma once
#include <optional>
#include <string>
#include <vector>
#include <random>
#include <utility>
#include <cstddef>
#include <new>
#include "pool.h"

class SkipList{
public:
    SkipList();
    ~SkipList();

    
    std::optional<std::string> Find(const std::string& key) const;
    bool Insert(const std::string& key, const std::string& val);
    bool Erase(const std::string &key);
    std::vector<std::pair<std::string, std::string>> Scan(const std::string& begin, const std::string& end) const;
    size_t Size() const {return size_;}
    

private:
    struct Node
    {
        std::string key;
        std::string val;
        std::vector<Node*> next;
        Node(std::string k, std::string v, int level)
        :key(std::move(k)), val(std::move(v)), next(level, nullptr){}
    };

    int RandomLevel();
    Node* FindGreaterOrEqual(const std::string& key, std::vector<Node*>& prev)const;

    static constexpr int kMaxLevel = 12;
    int level_ = 1;
    size_t size_ = 0;
    Node* head_;
    Nodepool pool_{sizeof(Node)};
    std::mt19937 rng_{std::random_device{}()};
};
