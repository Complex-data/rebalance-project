#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "io.hpp"
#include "random.hpp"
config conf;
#include "configFile.hpp"
#include "network.hpp"
using namespace std;
template<typename Type> void print(const std::string &output, const std::vector<Type> &v);

// all parameters used for config are contained in conf struct (see io.hpp);
void load_config(const string &input_file);
std::string set_filename();

int main(int argc, char* argv[]){
	string input_file("config.in");
	if (argc>1){
		input_file=argv[1];
	}
	configFile cf(input_file);
	// conf is a global variable containing all variables
	// from input config file;
	load_config(input_file);
	network bip_net;
}


std::string set_filename(){
	std::string filename_out= conf.output_dir+"/"+".out";
	return filename_out;
}

void load_config(const string &input_file){
	configFile cf(input_file);
	cf.get("input_links",conf.user_item_net.path);
	cf.get("user_col",conf.user_item_net.llink);
	cf.get("item_col",conf.user_item_net.rlink);
	cf.get("rating_col",conf.user_item_net.rating);
	cf.get("time_col",conf.user_item_net.time);
    
    cf.get("class_file",conf.class_file);
	cf.get("output_dir",conf.output_dir);
}
