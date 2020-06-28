#ifndef NETWORK_INCLUDED
#define NETWORK_INCLUDED
#include <vector>
#include <string>
#include <map>
#include "io.hpp"

struct net_link{
    net_link(){}
	net_link(const size_t ll, const size_t rl, const size_t t=0, const double r=0) :
	llink(ll), rlink(rl), time(t), rating(r), w(0){}
	size_t llink;
	size_t rlink;
	size_t time;
	double rating;
    double w;
    bool operator<(const net_link &nl) const{
        return nl.time < time;
    }
};

typedef std::vector<std::pair<size_t,size_t> > int_pairs;
typedef std::vector<std::pair<size_t,double> > pairs;
typedef std::vector<std::vector<size_t> > Mat_i;
typedef std::vector<std::vector<std::string> > Mat_s;
typedef std::vector<std::vector<double> > Mat;
typedef std::vector<std::vector<pair<size_t,size_t> > > Mat_p;
typedef std::vector<net_link > vec_links;
typedef std::vector<vec_links > Mat_links;

class network{
    
public:
    
    network();
    ~network();
    void create_net();
    size_t N,M,m;
    size_t min_time, max_time;
    size_t max_class_id;
    vec_links net_links;
    size_t ka_time(const size_t item, const size_t t);
    std::vector<size_t> ki,ka;
    std::vector<size_t> class_id;
    Mat_links neighbors_users, neighbors_items;
    
    std::vector<size_t> first_seen;
    std::vector<size_t> age;
    void printout();
    
private:
    
    void network_properties();
    void network_stats();
    void run_algorithms();
    void load_net(const network_config &nc, vec_links &datas);
    void neighbors(std::vector<size_t> &ki, std::vector<size_t> &ka, Mat_links &neighbors_users, Mat_links &neighbors_items, const vec_links &l);
    std::vector<size_t> get_kt(const Mat_links &neighbors, const size_t time);
    void users_info();
    void items_info();
    
    //birank
    void birank(const size_t time, const std::string weight, const std::string weight_d);
    std::vector<double> get_random_vector(const size_t size);
    std::vector<double> get_p(const std::vector<double> &u, const size_t time, const std::string weight, const std::string weight_d);
    std::vector<double> get_u(const std::vector<double> &p, const size_t time, const std::string weight, const std::string weight_d);
    double diff(const std::vector<double> &u, const std::vector<double> &v);
    void write_w(Mat_links &neighbors_users, const size_t user_idx, const size_t time, const std::string weight);
    std::vector<double> get_d(const Mat_links &neighbors, const size_t time, const std::string weight);
    void output(const std::vector<double> &p, const std::vector<double> &u, const size_t time, const std::string &weight, const std::string &weight_d);
    double a,b, delta;
	
	void bihits(const size_t time, const std::string weight, const std::string weight_d);
	std::vector<double> get_p_hit(const std::vector<double> &u, const size_t time, const std::string weight, const std::string weight_d);
    std::vector<double> get_u_hit(const std::vector<double> &p, const size_t time, const std::string weight, const std::string weight_d);
    
	void brgm(const size_t time, const std::string weight, const std::string weight_d);
	std::vector<double> get_p_brgm(const std::vector<double> &u, const size_t time, const std::string weight, const std::string weight_d);
    std::vector<double> get_u_brgm(const std::vector<double> &p, const size_t time, const std::string weight, const std::string weight_d);
    // time properties
    void time_properties(const vec_links &l, std::map< std::pair<size_t,size_t>,size_t>& T);
};

#endif
