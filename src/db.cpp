#include "db.h"
#include <cstdint>
#include <fstream>
#include <zlib.h>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>


static bool read_u32(std::ifstream& in, uint32_t& v){
	return static_cast<bool>(in.read(reinterpret_cast<char*>(&v), 4));
}

static void write_u32(std::string &buf, uint32_t v){
	buf.append(reinterpret_cast<const char*>(&v), 4);
}

KV::KV():KV(Options{}){}

KV::KV(Options opt) : opt_(opt){
	fd_ = ::open("data.log", O_CREAT | O_WRONLY | O_APPEND, 0644);
}

KV::~KV(){
	if (fd_ >= 0){
		::close(fd_);
		fd_ = -1;
	}
}

bool KV::AppendRecord(char type, const std::string& key, const std::string& val)
{
	// if (!log_.is_open()){
	// 	log_.open("data.log", std::ios::binary | std::ios::app);
	// 	if(!log_.is_open())return false;//防止open打开失败情况，导致数据丢失
	// }
	if (fd_ < 0)return false;


	std::string body;
	write_u32(body, (uint32_t)key.size());
	write_u32(body, (uint32_t)val.size());
	body.push_back(type);
	body.append(key.data(), key.size());
	body.append(val.data(), val.size());

																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																		uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, reinterpret_cast<const Bytef*> (body.data()),(uInt)body.size());


	std::string buf;
	write_u32(buf, (uint32_t)crc);
	buf.append(body);


	// log_.write(buf.data(), (std::streamsize)buf.size());
	// log_.flush();
	// return log_.good();

	ssize_t n = ::write(fd_, buf.data(), buf.size());
	if (n != (ssize_t)buf.size()) return false;
	
	if (opt_.sync_on_write){
		if (::fsync(fd_) != 0)return false;
	}
	return true;
	
}

bool KV::Put(std::string key, std::string val){
	if (key.empty())return false;
	if (!AppendRecord(1, key, val))return false;
	// table[key] = val;
	list_.Insert(key, val);
	return true;
}

std::optional<std::string> KV::Get(std::string key){
	// auto it = table.find(key);
	// if (it == table.end())return std::nullopt;
	// return it->second;
	return list_.Find(key);
}

bool KV::Delete(std::string key){
	// if (!AppendRecord(2, key, "")) return false;
	// return table.erase(key) > 0;
	return list_.Erase(key);
}

bool KV::Load(){
	std::ifstream in("data.log", std::ios::binary);
	if(!in)return true;

	std::streamoff good_end = 0;

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

	std::string body;
	write_u32(body, klen);
	write_u32(body, vlen);
	body.push_back(type);
	body.append(k);
	body.append(v);

	uLong actual = crc32(0L, Z_NULL, 0);
	//重新计算一遍记入数据里的key和val的crc值
	actual = crc32(actual, reinterpret_cast<const Bytef*>(body.data()), (uInt)body.size());
	//看看存入的key和val是否有错误，跟存入前是否相同
	if ((uint32_t)actual != crc)break;

	// if (type == 1) table[k] = v;
	// else if (type == 2) table.erase(k);

	if (type == 1)list_.Insert(k, v);
	else if (type == 2)list_.Erase(k);


	good_end = in.tellg();
    }
	in.close();

	std::error_code ec;
	auto size = std::filesystem::file_size("data.log",ec);
	if (!ec && (std::uintmax_t)good_end < size){
		std::filesystem::resize_file("data.log",(std::uintmax_t)good_end,ec);
	}

	return true;
}

std::vector<std::pair<std::string, std::string>> KV::Scan(const std::string& begin, const std::string& end) const {
    return list_.Scan(begin, end);
}

