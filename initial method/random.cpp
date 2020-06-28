#include <random>

using namespace std;
mt19937 gen(0);
size_t random(const size_t max) {
   uniform_int_distribution<size_t> dist(0, max);
   return dist(gen);
}
double rand_d(){
	uniform_real_distribution<> unif(0,1);
	return unif(gen);
}

size_t rand_pow(){
	return round(pow(1.-rand_d(),-2./3.));
}
