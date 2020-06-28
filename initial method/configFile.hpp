#ifndef NET_CONFIG_INCLUDED
#define NET_CONFIG_INCLUDED
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

class configFile
{
	public:
		configFile(const std::string& filename);
		void printout(const std::string &output_filename);

	template<typename T> void get(const std::string& key, T& out) const{
		auto val = m_conf.find(key);
		if ( val != m_conf.end() ){
			std::istringstream iss(val->second);
			iss >> out;
		}
	}
	void get(const std::string& key, bool &out) const;
	private:
		std::map<std::string, std::string> m_conf;
};


#endif
