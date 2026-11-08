#include"pool.h"

Nodepool::Nodepool(size_t slot_size, size_t slot_pre_block)
    :slot_size_(slot_size), slot_size_block_(slot_pre_block)
    {
        if (slot_size_ % 8 != 0)slot_size_ = (slot_size_ / 8 + 1) * 8;
    }


Nodepool::~Nodepool(){
    for(char* b: blocks_)delete[] b;
}

void Nodepool::Grow(){
    char* block = new char[slot_size_ * slot_size_block_];
    blocks_.push_back(block);

    for(size_t i = slot_size_block_; i-- > 0;){
        Free(block + i * slot_size_);
    }
}

void* Nodepool::Alloc(){
    if (head_free_ == nullptr)Grow();
    FreeNode* n = head_free_;
    head_free_ = n->next;
    return n;
}

void Nodepool::Free(void* p){
    FreeNode* n = static_cast<FreeNode*>(p);
    n->next = head_free_;
    head_free_ = n;
}
    
