#mini-kv

用c++20从零实现的持久化键值存储（学习项目，进行中）。
##目前支持

- 'Put'/'Get'/'Delete',内存索引基于'unordered_map'
- 'std::optional' 表达“键不存在”，与空值区分
-空key拒绝
-gtest单元测试，4个用例

##构建与测试

```bash

cmake -B build
cmake --build build -j
cmake --test-dir build --output-on-failure
