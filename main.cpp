#include<iostream>
#include "db.h"
int main(int argc, char** argv){
	KV db;
	db.Load();

	if (argc > 1 && std::string(argv[1]) == "stress"){
		for(int i = 0; i < 10000000; i++){
			db.Put("k" + std::to_string(i), "val" + std::to_string(i));
		}
		std::cout << "done\n";
		return 0;
	}

	std:: cout << "load size =" << db.Size() << std::endl;

	// db.Put("a", "1");
    // db.Put("b", "2");
    // db.Put("c", "3");

	// db.Put("name", "alice");
	// std::cout << "1." << db.Get("name").value() << "\n";
	// db.Put("name", "bob");
	// std::cout << "2." << db.Get("name").value() << "\n";
	// std::cout << "3." << db.Put("","x") << "\n";
	// std::cout << "4." << (db.Get("nokey").has_value() ? "has" : "empty") << "\n";

	// db.Delete("name");
	// std::cout << "5." << (db.Get("name").has_value() ? "has" : "delete") << "\n";
}
