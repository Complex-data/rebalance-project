#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <math.h>
using namespace std;

#define AGENTS          480189               //max user id+1
#define ITEMS           17770                  //max artists id+1
#define R_AGENTS        429584                 //users
#define R_ITEMS         17764                //artists
#define delta           0.0000000001      //threshold 阈值
const long long int T=1304941497468;//@todo

//#define E_T       0   /* 0 for deactivate explicit trust, 1 otherwise */

// DECAY parameters
#define DEC           0   /* 0 for deactivate decay, 1 for power, 2 for exponential, 3 for theta */
#define tau0    50    /* it is also the treshold for the theta */
#define beta    1   /* exponent for power decay */

// other SWITCHES
//#define RENORM_Q  0   /* 0 for deactivate renormalization of quality, 1 otherwise */
//#define RENORM_R  0   /* 0 for deactivate renormalization of reputation, 1 otherwise */
//#define RENORM_T  0   /* 0 for deactivate renormalization of trust, 1 otherwise */
//#define RESC_Q  0   /* 0 for deactivate rescaled quality, 1 otherwise */
//#define RESC_R  0   /* 0 for deactivate rescaled reputation, 1 otherwise */
//#define RESC_T    0   /* 0 for deactivate rescaled trust, 1 otherwise */

struct agentstruct{
  //user datastruct
  double reputation;      //声誉值
  double r_tmp;       //
  vector <long> collection; //
  vector <long> c_time;   //
  vector <double> c_weight; //
  unsigned long totw;         //
  vector <long> neighbor;   //
  vector <double> t_exp;    //
  } agent[AGENTS+1];

struct itemstruct{
  //artist datastruct
  double quality;       //
  double q_tmp;       //
  vector <long> reader;   //
  vector <long> r_time;   //
  vector <double> r_weight; //
  unsigned long totw;         //
  } item[ITEMS+1];
vector <long>::iterator it;
int flag;
int E_T;
int RENORM_Q,RENORM_R,RENORM_T,RESC_Q,RESC_R,RESC_T;
double E_value;

ifstream ifs_rm("../../data/netflix_com/new_twenty_u&i/netflix_com_20u&i.txt");
ifstream ifs_sn("user_friends1.txt");
ofstream ofs_AG;
ofstream ofs_IT;
ofstream ofs;

void save(){
  long i,a;
  char filename[100];
  sprintf(filename,"agents_%d_%d_%d_%d_%d_%d_%d.txt",E_T,RENORM_Q,RENORM_R,RENORM_T,RESC_Q,RESC_R,RESC_T);
  ofs_AG.open(filename);
  sprintf(filename,"items_%d_%d_%d_%d_%d_%d_%d.txt",E_T,RENORM_Q,RENORM_R,RENORM_T,RESC_Q,RESC_R,RESC_T);
  ofs_IT.open(filename);
  for (i=1; i<=AGENTS; i++) ofs_AG << i << "\t" << agent[i].reputation << "\t" << agent[i].collection.size() << "\t" << agent[i].totw << "\t" << agent[i].neighbor.size() << endl;
  for (a=1; a<=ITEMS; a++) ofs_IT << a << "\t" << item[a].quality << "\t" << item[a].reader.size() << "\t" << item[a].totw << endl;
}

void read() {
  //init
  long i,j,a,w,time;
  time=0;//default
  for (i=1; i<=AGENTS; i++) agent[i].totw=0;
  for (a=1; a<=ITEMS; a++) item[a].totw=0;

  while(ifs_rm.good()) {
    ifs_rm >> i; ifs_rm >> a; ifs_rm >> w; ifs_rm >> time; if (time<0) time=0;
    // w = pow(w,10);
    if (find(agent[i].collection.begin(),agent[i].collection.end(),a)==agent[i].collection.end()) {//有问题？
      item[a].reader.push_back(i); item[a].r_weight.push_back(w); item[a].r_time.push_back(time);
      agent[i].collection.push_back(a); agent[i].c_weight.push_back(w); agent[i].c_time.push_back(time);
      item[a].totw+=w; agent[i].totw+=w;//权值之和
      }
    }
  while(ifs_sn.good()) {
    ifs_sn >> i; ifs_sn >> j;
    agent[i].neighbor.push_back(j); agent[i].t_exp.push_back(E_value);//好友关系，E_value是什么东西
    }
}

long double decay(int tau) {
  if (DEC==1) return( pow((1.+(long double)tau/tau0),-beta) );
  if (DEC==2) return( exp(-(long double)tau/tau0) );
  if (DEC==3) {
    if (tau<=tau0) return(1.);
    else return(0.);
    }
  else return(1.);
  }

void QTR(){
  unsigned int a,r,j;
  int i;
  double err,norm,avg_q,avg_r;
  cout << "starting QTR" << endl;
  err=AGENTS+ITEMS; avg_q=0; avg_r=0;
  for (a=1; a<=ITEMS; a++) item[a].q_tmp=1./sqrt(R_ITEMS);
  for (i=1; i<=AGENTS; i++) agent[i].r_tmp=1./sqrt(R_AGENTS);
  flag=0;
  char filename[100];
  sprintf(filename,"convergence_%d_%d_%d_%d_%d_%d_%d.txt",E_T,RENORM_Q,RENORM_R,RENORM_T,RESC_Q,RESC_R,RESC_T);
  ofs.open(filename);

  while (err>delta) {
    err=0;
    norm=0;

    for (a=1; a<=ITEMS; a++) {
      item[a].quality=0;
      for (r=0; r<item[a].reader.size(); r++) item[a].quality+=1*(agent[item[a].reader[r]].r_tmp-RESC_R*avg_r)*decay(T-item[a].r_time[r])/pow(item[a].reader.size(),RENORM_Q);
      //for (r=0; r<item[a].reader.size(); r++) item[a].quality+=item[a].r_weight[r]*(agent[item[a].reader[r]].r_tmp-RESC_R*avg_r)*decay(T-item[a].r_time[r])/pow(item[a].reader.size(),RENORM_Q);
      norm+=pow(item[a].quality,2);
      }

    for (a=1; a<=ITEMS; a++) {item[a].quality/=sqrt(norm); err+=fabs(item[a].quality-item[a].q_tmp);}
    norm=0;

    for (i=1; i<=AGENTS; i++) {
      agent[i].reputation=0;
	  //for (a=0; a<agent[i].collection.size(); a++) agent[i].reputation+=agent[i].c_weight[a]*(item[agent[i].collection[a]].q_tmp-RESC_Q*avg_q)*decay(T-agent[i].c_time[a])/pow(agent[i].collection.size(),RENORM_R);
      for (a=0; a<agent[i].collection.size(); a++) agent[i].reputation+=1*(item[agent[i].collection[a]].q_tmp-RESC_Q*avg_q)*decay(T-agent[i].c_time[a])/pow(agent[i].collection.size(),RENORM_R);
      //for (j=0; j<agent[i].neighbor.size(); j++) agent[i].reputation+=(agent[agent[i].neighbor[j]].r_tmp-RESC_T*avg_r)*E_value/pow(agent[i].neighbor.size(),RENORM_T);
      norm+=pow(agent[i].reputation,2);
      }

    for (i=1; i<=AGENTS; i++) {agent[i].reputation/=sqrt(norm); err+=fabs(agent[i].reputation-agent[i].r_tmp);}

    avg_q=0; avg_r=0;
    for (a=1; a<=ITEMS; a++) {
      item[a].q_tmp=item[a].quality;
      avg_q+=item[a].quality/R_ITEMS;
      }
    for (i=1; i<=AGENTS; i++) {
      agent[i].r_tmp=agent[i].reputation;
      avg_r+=agent[i].reputation/R_AGENTS;
      }

    flag++;
    cout << flag << "\t" << err << endl;
    ofs << flag << "\t" << err << endl;
    }
  ofs.close();
}

int main(int argc, char *argv[]){

  if (argc!=8) exit(99);
  E_T=atoi(argv[1]);
  RENORM_Q=atoi(argv[2]);
  RENORM_R=atoi(argv[3]);
  RENORM_T=atoi(argv[4]);
  RESC_Q=atoi(argv[5]);
  RESC_R=atoi(argv[6]);
  RESC_T=atoi(argv[7]);
  //E_value=745.2*pow(49.07,1-RENORM_R)*pow(13.44,RENORM_T-1)*E_T;//@todo
  //E_value=36566.58*pow(49.07,1-RENORM_R)*pow(13.44,RENORM_T-1)*E_T;//for last.fm
  E_value=123.45*pow(49.07,1-RENORM_R)*pow(13.44,RENORM_T-1)*E_T;//for randtest
  read();
  QTR();
  save();
  return(0);
}
