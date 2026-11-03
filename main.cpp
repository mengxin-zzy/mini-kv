#include<iostream>
#include "db.h"
int main(){
	KV db;
	db.Put("name", "alice");
	std::cout << "1." << db.Get("name").value() << "\n";
	db.Put("name", "bob");
	std::cout << "2." << db.Get("name").value() << "\n";
	std::cout << "3." << db.Put("","x") << "\n";
	std::cout << "4." << (db.Get("nokey").has_value() ? "has" : "empty") << "\n";
	db.Delete("name");
	std::cout << "5." << (db.Get("name").has_value() ? "has" : "delete") << "\n";
}
