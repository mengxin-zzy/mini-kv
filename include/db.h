#pragma once
#include <optional>
#include <unordered_map>
#include <string>
class KV{
public:
	bool Put(std::string key, std::string value);
	std::optional<std::string> Get(std::string key);
	bool Delete(std::string key);
private:
	std::unordered_map<std::string, std::string> table;
};
