#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include "configFile.hpp"
configFile::configFile(const std::string& filename){
	std::ifstream file(filename.c_str());
	std::string line;
	while( std::getline(file, line) ){
		std::istringstream iss(line);
		std::string key, value;
		iss >> key;
		iss >> value;
		m_conf.insert( std::pair<std::string, std::string>(key,value) );
	}
	file.close();
}


void configFile::printout(const std::string &output_filename){
	std::ofstream output_file(output_filename);
	if(output_file.is_open()){
		for(auto i : m_conf){
			output_file << i.first << " " << i.second << "\n";
		}
	}
	else{ std::cerr << "Failure to open : " << output_filename << "\n";}
}

// bool overload so that it can read the 'true', 'false' strings
void configFile::get(const std::string& key, bool &out) const{
	auto val = m_conf.find(key);
	if ( val != m_conf.end() )
	{
		std::istringstream iss(m_conf.find(key)->second);
		out = false;
		iss >> out;
		if (iss.fail())
		{
			iss.clear();
			iss >> std::boolalpha >> out;
		}
	}
}
