#pragma once
#include <optional>
#include <unordered_map>
#include <string>
#include <fstream>

class KV{
public:
	bool Load();
	size_t Size() const{ return table.size();}
	bool Put(std::string key, std::string value);
	std::optional<std::string> Get(std::string key);
	bool Delete(std::string key);
private:
	bool AppendRecord(char type, const std::string& key, const std::string& val);
	std::unordered_map<std::string, std::string> table;
	std::ofstream log_;
};
