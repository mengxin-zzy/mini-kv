#include "db.h"
bool KV::Put(std::string key, std::string val){
	if (key.empty())return false;
	table[key] = val;
	return true;
}
std::optional<std::string> KV::Get(std::string key){
	auto it = table.find(key);
	if (it == table.end())return std::nullopt;
	return it->second;
}
bool KV::Delete(std::string key){
	return table.erase(key) > 0;
}
