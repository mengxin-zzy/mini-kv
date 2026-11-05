#mini-kv

用c++20从零实现的持久化键值存储（学习项目，进行中）。


##目前支持

- 'Put'/'Get'/'Delete',内存索引基于'unordered_map'
- 'std::optional' 表达“键不存在”，与空值区分
-空key拒绝
- WAL 追加写：每次修改追加一条记录到 `data.log`
- 启动时重放日志重建索引，删除通过墓碑（tombstone）记录
- 每条记录带 CRC32 校验，损坏时停止回放并截断文件
-gtest单元测试，4个用例

##构建与测试

```bash

cmake -B build
cmake --build build -j
cmake --test-dir build --output-on-failure
```


## 目录结构

```
include/db.h    对外接口
src/db.cpp      实现（编码、写入、回放、校验）
tests/          单元测试
main.cpp        手动测试入口
```

## 崩溃恢复验证

```bash
./build/mini_kv stress &     # 持续写入
kill -9 $!                   # 强杀
./build/mini_kv              # 重启：不崩溃，数据可恢复
```
实测：141MB 日志、253 万条记录，启动重放全量恢复。

## 路线图

- [x] 内存版 KV
- [x] WAL 追加写 + 重启恢复
- [x] CRC32 校验 + 崩溃恢复
- [ ] 跳表索引 + 范围查询
- [ ] epoll 网络层
- [ ] 压测：QPS / P99 / 内存

## 设计决策

见 [DESIGN.md](DESIGN.md)