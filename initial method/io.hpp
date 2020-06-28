#ifndef IO_INCLUDED
#define IO_INCLUDED
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;
typedef std::vector<std::vector<double> > Mat;
#define PI 3.14159265
struct network_config{
	network_config(const std::string p) : llink(1), rlink(2), time(0), rating(0), path(p) {}
	size_t llink;
	size_t rlink;
	size_t time;
	size_t rating;
	string path;
};


struct config{
    config() : user_item_net("../data/amazon_music.txt"), output_dir("./output"), class_file("genres.dat")  {}
    network_config user_item_net;
    std::string output_dir;
    std::string class_file;
};

// global with all config parameters
extern config conf;

template<typename Type> void print(const std::string &output, const std::vector<std::vector<Type> > &v){
	std::ofstream output_f(output);
    std::cerr << "print" << std::endl;
	if(output_f.is_open()){
		for(auto i : v){
			for(auto j : i){
				output_f << j << " ";
			}
			output_f << std::endl;
		}
        std::cerr << "output file: " << output << "\n"; 
	}
	else{
		std::cerr << "could not open file : " << output << std::endl;
	}
}

template<typename Type> void print(const std::string &output, const std::vector<Type> &v){
	std::ofstream output_f(output);
	if(output_f.is_open()){
		for(auto i : v){
			output_f << i << std::endl;
		}
	}
	else{
		std::cerr << "could not open file : " << output << std::endl;
	}
}
template<typename Type> void print(const std::string &output, const std::vector<pair<Type,Type>> &v){
	std::ofstream output_f(output);
	if(output_f.is_open()){
		for(auto i : v){
			output_f << i.first << " " << i.second << std::endl;
		}
        std::cerr << "output file: " << output << "\n"; 
	}
	else{
		std::cerr << "could not open file : " << output << std::endl;
	}
}

template<typename Type1,typename Type2> void print(const std::string &output, const std::vector<pair<Type1,Type2>> &v){
    std::cerr << "open file : " << output << std::endl;
    std::ofstream output_f(output);
    if(output_f.is_open()){
        for(auto i : v){
            output_f << i.first << " " << i.second << std::endl;
        }
    }
    else{
        std::cerr << "could not open file : " << output << std::endl;
    }
}

template <typename T> std::string to_string_with_precision(const T a_value, const int n=2){
    std::ostringstream out;
    out << std::setprecision(n) << fixed << a_value;
    return out.str();
}

template <typename T> double mean(const vector<T> v){
	T sum(0);
    for(const auto &i : v) sum+=i;
    return sum/ static_cast<double>(v.size());
}

template <typename T> double sum(const vector<T> v){
	T sum(0);
    for(const auto &i : v) sum+=i;
    return sum;
}

template <typename T> double std_dev(const std::vector<T> &v){
	if(v.size()<2){return 0;}
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	double mean = sum / v.size();
	
	std::vector<double> diff(v.size());
	std::transform(v.begin(), v.end(), diff.begin(),
				   std::bind2nd(std::minus<double>(), mean));
	double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / v.size());
	return stdev;
}

template <typename T> T max_v(const std::vector<T> &v){
	T max(0);
	for(const auto i : v) if(i>max) max=i;
	return max;
}

template <typename Type> void load(const std::string &input_file, std::vector<Type> &vec){
    ifstream infile(input_file);
    Type s;
	if(infile.is_open()){
		while (infile>>s){
			vec.push_back(s);
		}
	}
	else cerr << "Error when loading file " << input_file << "\n";
}

template <typename Type> void load(const std::string &input_file, std::vector<std::pair<Type,Type> > &vec){
    ifstream infile(input_file);
    Type s,t;
	if(infile.is_open()){
		while (infile>>s>>t){
			vec.push_back(make_pair(s,t));
		}
	}
	else cerr << "Error when loading file " << input_file << "\n";
}

// copy
template <typename T> void cumulative_histogram(const string &output,const vector<T> &datas, size_t N_bins=0)
{
    T max_data=max_v(datas);
    if(N_bins==0) N_bins=max_data;
    vector<pair<T,size_t> > bins(N_bins+1);
    for(size_t i(0);i<bins.size();++i){
        bins[i].first= static_cast<T>(i/static_cast<double>(N_bins) * max_data);
    }
    for(const auto i : datas){
        ++bins[ floor(static_cast<double>(i*N_bins)/max_data)].second;
    }
    for(size_t i(N_bins);i>0;--i){
        bins[i-1].second+=bins[i].second;
    }
    print(output,bins);
}

template <typename T> void normalized_histogram(const string &output,const vector<T> &datas, size_t N_bins=0)
{
    double max_data=max_v(datas);
    if(N_bins==0) N_bins=max_data;
    std::cerr << max_data << std::endl;
    vector<pair<T,double> > bins(N_bins+1);
    for(size_t i(0);i<bins.size();++i){
        bins[i].first= static_cast<T>(i/static_cast<double>(N_bins) * max_data);
    }
    for(const auto i : datas){
        ++bins[ floor(static_cast<double>(i*N_bins)/max_data)].second;
    }
    double sum=0;
    for(size_t i(0);i<bins.size();++i){ sum+=bins[i].second;}
    for(size_t i(0);i<bins.size();++i){ bins[i].second=bins[i].second/sum;}
    print(output,bins);
}

template <typename T> void histogram(const string &output,const vector<T> &datas, size_t N_bins=0)
{
    T max_data=max_v(datas);
    if(N_bins==0) N_bins=max_data;
    vector<pair<T,size_t> > bins(N_bins+1);
    for(size_t i(0);i<bins.size();++i){
        bins[i].first= static_cast<T>(i/static_cast<double>(N_bins) * max_data);
    }
    for(const auto i : datas){
        ++bins[ floor(static_cast<double>(i*N_bins)/max_data)].second;
    }
    print(output,bins);
}

//==========================//
template <typename T> double average(const std::vector<T> &v)
{
    if(v.size()==0) return 0.;
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();
    return mean;
}
#endif
