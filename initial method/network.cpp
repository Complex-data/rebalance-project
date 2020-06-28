#include "network.hpp"
#include "io.hpp"
#include "random.hpp"
#include <algorithm>
#include <chrono>
#include <random>
#include <math.h>
using namespace std;
network::network(){
    N=0;M=0;m=0;
    max_time=0;min_time=-1;
    a=1./(365.*3600.*24.); b=0;
    delta = 0.85;
    create_net();
    run_algorithms();
}

network::~network(){
    //~ output_new();
}

void network::run_algorithms()
{
    birank(max_time, "time", "time");
    birank(max_time, "rating", "rating");
	//brgm(max_time, "rating", "rating");
    //~ birank(max_time, "binary", "time");
    //birank(max_time, "time", "time");
    //bihits(max_time,"rating","rating");

}

void network::create_net(){
    load_net(conf.user_item_net,net_links);
    network_properties();
    
    std::sort(net_links.begin(), net_links.end(),
              [&](const net_link &l, const net_link &r){
                  return l.time < r.time; });       
    cout << "Users: " << N << " Items: " << M << " links: " << net_links.size() << "\n";
	
    
    neighbors(ki,ka,neighbors_users, neighbors_items,net_links);
    network_stats();
}

void network::load_net(const network_config &nc, vec_links &datas){
    
    datas.clear();
    ifstream infile(nc.path);
    while (infile) {
        std::vector<std::string> line;
        string s;
        // read line
        if (!getline(infile,s)) break;
        istringstream ss(s);
        
        // now separate line
        while (ss) {
            if (!getline(ss,s,'\t')) break;
            stringstream fs( s );
            std::string l;
            fs >> l;
            line.push_back(l);
        }
        size_t llink(std::stoul(line[nc.llink-1]));
		size_t rlink(std::stoul(line[nc.rlink-1]));
		size_t time(0);
		double rating(0);
		int score(0);
		double latitude(0);
		double longitude(0);
		if(nc.time!=0) time=std::stoul(line[nc.time-1]);
		if(nc.rating!=0) rating=std::stod(line[nc.rating-1]);
		datas.push_back( net_link(llink,rlink,time,rating) );		
    }
    if (!infile.eof()) cerr << "Error when loading file: " << nc.path << "\n";
}

void network::network_properties()
{
    m=net_links.size();
    for(auto &i : net_links){
        if(i.time>max_time){max_time=i.time;}
        if(i.time<min_time){min_time=i.time;}
        if(i.llink>N){N=i.llink;}
        if(i.rlink>M){M=i.rlink;}
    }
}

// some stats about the network
void network::network_stats()
{
    std::vector<std::string> metric_types = {"gyration","gyration_ki","gyration_ka","gyration_ka_ki","gyration_kat","gyration_kit","gyration_kat_kit"};
}

void network::neighbors(std::vector<size_t> &ki, std::vector<size_t> &ka, Mat_links &neighbors_users, Mat_links &neighbors_items, const vec_links &l)
{
    ki=vector<size_t>(N,0);
    ka=vector<size_t>(M,0);
    neighbors_items=Mat_links(std::vector<std::vector<net_link> >(M,std::vector<net_link>(0)));
    neighbors_users=Mat_links(std::vector<std::vector<net_link> >(N,std::vector<net_link>(0)));
    for(const auto &i : l){
        ++ki[i.llink-1];
        ++ka[i.rlink-1];
        neighbors_users[i.llink-1].push_back(i);
        neighbors_items[i.rlink-1].push_back(i);
    }
    for(auto &i : neighbors_items){
        sort(i.begin(),i.end());
    }
    for(auto &i : neighbors_users){
        sort(i.begin(),i.end());
    }
}

void network::users_info()
{
    Mat_s mat_users;
    mat_users.push_back( std::vector<std::string> { "%#userID " ," ki "} );
    for(size_t i(0);i<N;++i){
        mat_users.push_back(std::vector<std::string> (mat_users[0].size()) );
        mat_users[i+1][0]=to_string(i+1);
        mat_users[i+1][1]=to_string(ki[i]);
    }
    print(conf.output_dir+"/users_info.dat",mat_users);
}

void network::items_info()
{
    Mat_s mat_items;
    mat_items.push_back( vector<string> { "%#itemID " , " ka "} );
    for(size_t i(0);i<M;++i){
        mat_items.push_back(vector<string> (mat_items[0].size()) );
        mat_items[i+1][0]=to_string(i+1);
        mat_items[i+1][1]=to_string(ka[i]);
    }
    print(conf.output_dir+"/items_info.dat",mat_items);
}

void network::birank(const size_t time, const std::string weight, const std::string weight_d){
    std::vector<double> p = get_random_vector(N);
    std::vector<double> u = get_random_vector(M);
    std::vector<double> p_old = std::vector<double>(N,0);
    std::vector<double> u_old = std::vector<double>(M,0);
    int i=0;
    while( diff(p,p_old)>0.001 || diff(u,u_old)>0.001) {
        std::cerr << "Step:"  << ++i << " diff: " << diff(p,p_old) << "\n";
        u_old = u;
        p_old = p;
        p = get_p(u,time, weight, weight_d);
        u = get_u(p,time, weight, weight_d);
        
    }
    output(p,u, time, weight, weight_d);
}

void network::brgm(const size_t time, const std::string weight, const std::string weight_d){
    std::vector<double> p = std::vector<double>(N,1);
    std::vector<double> u = std::vector<double>(M,1);
    std::vector<double> p_old = std::vector<double>(N,0);
    std::vector<double> u_old = std::vector<double>(M,0);
    int i=0;
    while( i<=10) {
        std::cerr << "Step:"  << ++i << "\n";
        u_old = u;
        p_old = p;
        p = get_p_brgm(u,time, weight, weight_d);
        u = get_u_brgm(p,time, weight, weight_d);

    }
    output(p,u, time, weight, weight_d);
}

void network::bihits(const size_t time, const std::string weight, const std::string weight_d){
    std::vector<double> p = std::vector<double>(N, 1);
    std::vector<double> u = std::vector<double>(M, 1);
    std::vector<double> p_old = std::vector<double>(N,0);
    std::vector<double> u_old = std::vector<double>(M,0);
    int i=0;
    while( diff(p,p_old)>0.001 || diff(u,u_old)>0.001) {
        std::cerr << "Step:"  << ++i << " diff: " << diff(p,p_old) << "\n";
        u_old = u;
        p_old = p;
        p = get_p_hit(u,time, weight, weight_d);
        u = get_u_hit(p,time, weight, weight_d);
    }
    output(p,u, time, weight, weight_d);
}



void network::output(const std::vector<double> &p, const std::vector<double> &u, const size_t time, const std::string &weight, const std::string &weight_d)
{
    std::string out_file_p = conf.output_dir+"/p_"+weight+"_"+weight_d+"_"+std::to_string(time)+".dat";
    std::string out_file_u = conf.output_dir+"/u_"+weight+"_"+weight_d+"_"+std::to_string(time)+".dat";
    print( out_file_p, p);
    print( out_file_u, u);
}

std::vector<double> network::get_p(const std::vector<double> &u, const size_t time, const std::string weight, const std::string weight_d)
{
    std::vector<double> p(N,0);
    std::vector<double> di = get_d(neighbors_users, time, weight_d);
    std::vector<double> da = get_d(neighbors_items, time, weight_d);
    for( size_t user_idx(0); user_idx<N; ++user_idx) {
        write_w( neighbors_users, user_idx, time, weight );
        for( const auto &link : neighbors_users[user_idx]) {
            p[user_idx]+= link.w/std::sqrt(di[user_idx]*da[link.rlink-1]) * u[link.rlink-1];
        }
    }
    return p;
}
std::vector<double> network::get_p_brgm(const std::vector<double> &u, const size_t time, const std::string weight, const std::string weight_d)
{
    std::vector<double> p(N,0);
    std::vector<double> di = get_d(neighbors_users, time, weight_d);
    std::vector<double> da = get_d(neighbors_items, time, weight_d);
    for( size_t user_idx(0); user_idx<N; ++user_idx) {
        write_w( neighbors_users, user_idx, time, weight );
        for( const auto &link : neighbors_users[user_idx]) {
            p[user_idx]+= link.w/(di[user_idx]*da[link.rlink-1]) * u[link.rlink-1];
        }
    }
    return p;
}

std::vector<double> network::get_p_hit(const std::vector<double> &u, const size_t time, const std::string weight, const std::string weight_d)
{
    std::vector<double> p(N,1);
    std::vector<double> di = get_d(neighbors_users, time, weight_d);
    std::vector<double> da = get_d(neighbors_items, time, weight_d);
	double norm = 0;
    for( size_t user_idx(0); user_idx<N; ++user_idx) {
        write_w( neighbors_users, user_idx, time, weight );
        for( const auto &link : neighbors_users[user_idx]) {
            p[user_idx]+= link.rating * u[link.rlink-1];
        }
		norm += pow(p[user_idx],2);
    }
	norm = sqrt(norm);
	for( size_t user_idx(0); user_idx<N; ++user_idx){
		
			p[user_idx] /=norm;
		
	}
    return p;
}

std::vector<double> network::get_u_hit(const std::vector<double> &p, const size_t time, const std::string weight, const std::string weight_d)
{
    std::vector<double> u(M, 1);
    std::vector<double> di = get_d(neighbors_users, time, weight_d);
    std::vector<double> da = get_d(neighbors_items, time, weight_d);
    double norm = 0;
    for( size_t item_idx(0); item_idx<M; ++item_idx) {
        write_w( neighbors_items, item_idx, time, weight );
        for( const auto &link : neighbors_items[item_idx]) {
            u[item_idx]+= link.rating * p[link.llink-1];
        }
		norm += pow(u[item_idx],2);
    }
	norm = sqrt(norm);
	for( size_t item_idx(0); item_idx<M; ++item_idx) {
		u[item_idx] /=norm;
	}
    return u;
}

std::vector<double> network::get_u(const std::vector<double> &p, const size_t time, const std::string weight, const std::string weight_d)
{
    std::vector<double> u(M, 0);
    std::vector<double> di = get_d(neighbors_users, time, weight_d);
    std::vector<double> da = get_d(neighbors_items, time, weight_d);
    
    for( size_t item_idx(0); item_idx<M; ++item_idx) {
        write_w( neighbors_items, item_idx, time, weight );
        for( const auto &link : neighbors_items[item_idx]) {
            u[item_idx]+= link.w/std::sqrt(di[link.llink-1]*da[item_idx]) * p[link.llink-1];
        }
    }
    return u;
}
std::vector<double> network::get_u_brgm(const std::vector<double> &p, const size_t time, const std::string weight, const std::string weight_d)
{
    std::vector<double> u(M, 0);
    std::vector<double> di = get_d(neighbors_users, time, weight_d);
    std::vector<double> da = get_d(neighbors_items, time, weight_d);
    
    for( size_t item_idx(0); item_idx<M; ++item_idx) {
        write_w( neighbors_items, item_idx, time, weight );
        for( const auto &link : neighbors_items[item_idx]) {
            u[item_idx]+= link.w/(di[link.llink-1]*da[item_idx]) * p[link.llink-1];
        }
    }
    return u;
}

double network::diff(const std::vector<double> &u, const std::vector<double> &v)
{
    double diff(0),sum(0);
    for(size_t i(0);i<v.size();++i) {
        diff+= std::abs( u[i]-v[i] );
        sum+= std::abs(u[i])+std::abs(v[i]);
    }
    return diff/sum;
}

std::vector<double> network::get_random_vector(const size_t size)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0., 1.0);
    std::vector<double> vec(size);
    for(size_t i(0);i<size;++i){
        vec[i]=dist(mt);
    }
    return vec;
}

void network::write_w(Mat_links &neighbors_users, const size_t user_idx, const size_t time, const std::string weight)
{
    for( auto &link : neighbors_users[user_idx] ) {
        if(weight == "binary"){
            link.w = 1;
        }
        else if(weight == "rating"){
            link.w = link.rating;
        }
        else if(weight == "time"){
            link.w = std::pow(delta,a*(time-link.time)+b);
        }
        else{
            std::cerr << "Wrong weight specified in config\n";
        }
    }
}

std::vector<double> network::get_d(const Mat_links &neighbors, const size_t time, const std::string weight)
{
    std::vector<double> d(neighbors.size(),0);
    for(size_t idx(0) ; idx<neighbors.size() ; ++idx) {
        auto up = std::upper_bound (neighbors[idx].begin(), neighbors[idx].end(), time,
                            [](const size_t t, const net_link &link){ return t < link.time; });
        for(auto it = neighbors[idx].begin(); it < up; ++it) {
            if(weight == "binary"){
                d[idx] += 1;
            }
            else if(weight == "rating"){
                d[idx] += (*it).rating;
            }
            else if(weight == "time"){
                d[idx] += std::pow(delta,a*(time-(*it).time)+b);
            }
            else{
                std::cerr << "Wrong weight specified in config\n";
            }
        }
    }
    return d;
}
