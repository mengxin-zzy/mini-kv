#include "skiplist.h"

SkipList::SkipList(){
    head_ = new (pool_.Alloc())Node("","",kMaxLevel);
}

SkipList::~SkipList(){
    Node* cur = head_->next[0];
    while(cur != nullptr){
        Node* next = cur->next[0];
        cur->~Node();
        pool_.Free(cur);
        cur = next;
    }
    head_->~Node();
    pool_.Free(head_);
}

int SkipList::RandomLevel(){
    int lv = 1;
    std::uniform_int_distribution<int> d(0,3);
    while(lv < kMaxLevel && d(rng_) == 0) ++lv;
    return lv;
}


SkipList::Node* SkipList::FindGreaterOrEqual(const std::string& key, std::vector<Node*>& prev) const {
    Node* cur = head_;
    for(int i = level_ - 1; i >= 0; i--){
        while(cur->next[i] != nullptr && cur->next[i]->key < key){
            cur = cur->next[i];
        }

        //接入表中数据，并且该数据是要查询数据本身或者表中无查询数据时，小于且最接近查询数据的数
        prev[i] = cur;

    }
    return cur->next[0];
}

std::optional<std::string> SkipList::Find(const std::string& key) const {
    std::vector<Node*> prev(kMaxLevel, nullptr);
    Node* node = FindGreaterOrEqual(key, prev);
    if (node != nullptr && node->key == key)return node->val;
    return std::nullopt;
}

bool SkipList::Insert(const std::string& key, const std::string& val) {
    std::vector<Node*> prev(kMaxLevel, nullptr);
    Node* found = FindGreaterOrEqual(key, prev); // 这一步同时把prev接入了表中数据
    if (found != nullptr && found->key == key){
        found->val = val;
        return true;
    }

    int lv = RandomLevel();
    if (lv > level_){
        for(int i = level_; i < lv; i++) prev[i] = head_;
        level_ = lv;
    }

    Node* node = new (pool_.Alloc()) Node(key, val, lv);
    for(int i = 0; i < lv; i++){                    //不可能出现高层有而低层没有的情况
        node->next[i] = prev[i]->next[i];
        prev[i]->next[i] = node;
    }
    size_++;
    return true;
}

bool SkipList::Erase(const std::string& key){
    std::vector<Node*> prev(kMaxLevel, nullptr);
    Node* found = FindGreaterOrEqual(key, prev);
    if (found == nullptr || found->key != key)return false;

    for(int i = 0; i < (int)found->next.size(); i++){
        if (prev[i]->next[i] == found) prev[i]->next[i] = found->next[i];
    }
    found->~Node();
    pool_.Free(found);

    while(level_ > 1 && head_->next[level_ - 1] == nullptr) level_--;
    size_--;
    return true;
}

std::vector<std::pair<std::string, std::string>> SkipList::Scan(const std::string& begin, const std::string& end)const{
    std::vector<std::pair<std::string, std::string>> result;
    std::vector<Node*> prev(kMaxLevel, nullptr);
    Node* found = FindGreaterOrEqual(begin, prev);
    while(found != nullptr && found->key < end){
        result.push_back({found->key, found->val});
        found = found->next[0];
    }
    return result;
}
