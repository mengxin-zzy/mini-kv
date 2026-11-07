#pragma once
#include "skiplist.h"
#include <optional>
#include <unordered_map>
#include <string>
// #include <fstream>
#include <cstddef>

class KV{
public:
	struct Options{
		bool sync_on_write = false;
	};
	KV();
	explicit KV(Options opt);
	~KV();

	KV(const KV&) = delete;//禁止拷贝
	KV& operator = (const KV&) = delete;//禁止赋值给另一个对象

	bool Load();

	// size_t Size() const{ return table.size();}
	size_t Size() const {return list_.Size();}

	bool Put(std::string key, std::string value);

	std::optional<std::string> Get(std::string key);

	bool Delete(std::string key);

	std::vector<std::pair<std::string, std::string>> Scan(const std::string& begin, const std::string& end) const;



private:

	SkipList list_;
	Options opt_;
	int fd_ = -1;

	bool AppendRecord(char type, const std::string& key, const std::string& val);
	// std::unordered_map<std::string, std::string> table;
	// std::ofstream log_;
};
