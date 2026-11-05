#include "db.h"
#include <cstdint>
#include <fstream>
static bool read_u32(std::ifstream& in, uint32_t& v){
	return static_cast<bool>(in.read(reinterpret_cast<char*>(&v), 4));
}
static void write_u32(std::string &buf, uint32_t v){
	buf.append(reinterpret_cast<const char*>(&v), 4);
}
bool KV::Put(std::string key, std::string val){
	if (key.empty())return false;
	if (!log_.is_open()){
		log_.open("data.log", std::ios::binary | std::ios::app);
		if(!log_.is_open())return false;//防止open打开失败情况，导致数据丢失
	}
	std::string buf;
	write_u32(buf, 0);
	write_u32(buf, (uint32_t)key.size());
	write_u32(buf, (uint32_t)val.size());
	buf.push_back(1);
	buf.append(key.data(), key.size());
	buf.append(val.data(), val.size());
	log_.write(buf.data(), (std::streamsize)buf.size());
	log_.flush();
	table[std::string(key)] = std::string(val);
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
bool KV::Load(){
	std::ifstream in("data.log", std::ios::binary);
	if(!in)return true;
	while(true){
	uint32_t crc, klen, vlen;
	char type;
	if(!read_u32(in, crc)) break;
        if(!read_u32(in, klen)) break;
        if(!read_u32(in, vlen)) break;
        if(!in.read(&type, 1)) break;
	std::string k(klen, '\0');
	std::string v(vlen, '\0');
	if(klen > 0 && !in.read(&k[0], klen)) break;
	if(vlen > 0 && !in.read(&v[0], vlen)) break;
	if (type == 1) table[k] = v;
	else if (type == 2) table.erase(k);
    	}
	return true;
}














