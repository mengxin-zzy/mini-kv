#pragma
#include<cstddef>
#include<vector>

class Nodepool
{
public:
    explicit Nodepool(size_t slot_size_, size_t slot_size_block_ = 256);
    ~Nodepool();

    Nodepool(const Nodepool&) = delete;
    Nodepool& operator = (const Nodepool&) = delete;

    void* Alloc();
    void Free(void* p);

private:
    void Grow();
    struct FreeNode
    {
        FreeNode* next;
    };

    size_t slot_size_;
    size_t slot_size_block_;
    FreeNode* head_free_ = nullptr;
    std::vector<char*> blocks_;
};
