

#ifdef RUNF
#include "plg_faster.h"
plg_faster instance;
#endif

#ifdef RUNR
#include "plg_rocksdb.h"
plg_rocksdb instance;
#endif

#ifdef RUNB
#include "plg_berkeley.h"
plg_berkeley instance;
#endif

#include "util.h"

using namespace std::chrono_literals;

void runworkload(std::string _file, std::string _exptag){
    int failcnt=0;
    std::cout<<"--------Running workload: "<<_file<<std::endl;

    const std::string prefix{ "usertable user" };

    srand(time(NULL));
    int valsize=VALUESIZE;
    char val[valsize];
    for(int i=0; i<valsize; i++)
        val[i]='A'+rand()%26;
    int vidx;
    printf("valsize:  %lu\n", sizeof(val));


    std::vector<double> tot_latency;
    std::vector<std::string> cmds;
    std::vector<uint64_t> keys;

    std::string cmd;
    uint64_t ikey;
    double _latency=0.0;

    std::ifstream fin(_file);

    while (!fin.eof()) {
        std::string line;
        std::getline(fin, line);

        std::string::size_type pos = line.find(prefix);
        if (pos == std::string::npos) {
            continue;
        }
        cmd = line.substr(0, pos - 1);

        std::string::size_type pos2 = line.find(" [ ");
        std::string _key = line.substr(pos + 14, pos2 - (pos + 14));

        const char *key = _key.c_str();
        ikey = cstr2int64(key);

        cmds.push_back(cmd);
        keys.push_back(ikey);
    }

    std::vector<long long> res0=getiostat();
    auto start_time = std::chrono::high_resolution_clock::now();

    for(int i=0;i<cmds.size();i++){
//        cmd=cmds[i];
//        ikey=keys[i];

        //std::cout<<cmds[i]<<"##"<<keys[i]<<"##"<<key<<std::endl;
        //std::cout<<cmds[i]<<"____ksize=="<<sizeof(key)<<"____";        printf("%s____%s\n", key, val);        //keys are 8 bytes, same as uint64_t

        try {
            int opr = -1;
            if (cmds[i] == "INSERT") {
                opr = 1;
//                for(int i=0; i<valsize; i++)
//                    val[i]='A'+rand()%26;
//                vidx = rand() % valsize;
//                val[vidx] = 'A' + rand() % 26;
                _latency = instance.opr_insert(keys[i], val);
                tot_latency.push_back(_latency);
            } else if (cmds[i] == "UPDATE") {
                opr = 2;
//                for(int i=0; i<valsize; i++)
//                    val[i]='A'+rand()%26;
//                vidx = rand() % valsize;
//                val[vidx] = 'A' + rand() % 26;
                _latency = instance.opr_insert(keys[i], val);    //UPDATE->merge
                tot_latency.push_back(_latency);
            } else if (cmds[i] == "RMW") {          // for FASTER only
                opr = 4;
//                for(int i=0; i<valsize; i++)
//                    val[i]='A'+rand()%26;
//                vidx = rand() % valsize;
//                val[vidx] = 'A' + rand() % 26;
                _latency = instance.opr_insert(keys[i], val);
                tot_latency.push_back(_latency);
            } else if (cmds[i] == "READ") {
                opr = 3;
                _latency = instance.opr_read(keys[i]);
                tot_latency.push_back(_latency);
            } else {
                std::cout << "unknown cmd " << cmds[i] << "\n";
                exit(-1);
            }
        }catch(const  char* msg ){
            failcnt+=1;
            //std::cout<<"failed opr ----"<<cmds[i]<<"--"<<keys[i]<<"\n";
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration = end_time - start_time;
    std::vector<long long> res1=getiostat();

    sort(tot_latency.begin(),tot_latency.end(), cmp);

    int opr_done=cmds.size();
    double tot_dur=0.0;
    for(auto i : tot_latency) {
        tot_dur += i;
        //printf("%.8f\n", i);
    }
    double avg=tot_dur/opr_done;

    double t50=tot_latency[(int)(0.5*opr_done)];
    double t99=tot_latency[(int)(0.001*opr_done)];
    double ops=opr_done/(tot_dur/1000000);

    static constexpr uint64_t kNanosPerSecond = 1000000000;
    float dur=(double)duration.count() / kNanosPerSecond;

    printf("  p99.9 latency: %.4f us, avg latency: %.4f us, median latency: %.4f us\n", t99, avg, t50);
    printf("  total running time: %.4f seconds. total storage opr time: %.4f seconds \n", dur, tot_dur/1000000);
    printf("  %d operations. ops: %.4f. \n", opr_done, ops);
    printf("  %lld kB_read. %lld kB_wrtn \n", res1[0]-res0[0], res1[1]-res0[1]);
    printf("  num of failed opr: %d \n", failcnt);
    //std::cout<<(int)(0.01*opr_done)<<"  "<<tot_latency[(int)(0.01*opr_done)]<<"\n";

    if(_exptag!=""){
        std::ofstream write;
        write.open("result.txt", std::ios::app);
        write.setf(std::ios::fixed, std::ios::floatfield);
        write.precision(4);
        // tag, p99, avg, median, ops, kB_read, kB_wrtn
        write << _exptag << ", " << t99 << ", " << avg << ", " << t50 << ", " << ops << ", " << res1[0]-res0[0] << ", " << res1[1]-res0[1] << std::endl;
//        for(auto i : tot_latency)
//            write<<i<<"\n";
//    for(int i=0; i<opr_done; i++)
//        write<<i<<" "<<tot_latency[i]<<"\n";
        write.close();
    }
}

void unittest(){
    //put a value - get a value - delete it - get it again - rmw tests
    srand(time(NULL));
    int _valsize=VALUESIZE;
    char _val[_valsize];
    for(int i=0; i<_valsize; i++)
        _val[i]='A'+rand()%26;
    printf("valsize:  %lu\n", sizeof(_val));
    std::cout<<_val<<std::endl;

    uint64_t _ikey=3537999523654637759;

    double _latency=0.0;

    printf("(1) put::::\n");
    _latency = instance.opr_insert(_ikey, _val);
    printf("%.8f\n", _latency);
    printf("(2) get::::\n");
    _latency = instance.opr_read(_ikey);
    printf("%.8f\n", _latency);
    printf("(3) del::::\n");
    _latency = instance.opr_delete(_ikey);
    printf("%.8f\n", _latency);
    printf("(4) get::::\n");
    _latency = instance.opr_read(_ikey);
    printf("%.8f\n", _latency);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Requires 3 arguments: ycsb_load_file  ycsb_run_file exptag\n");
        exit(-1);
    }

    std::cout<<"--------------------------------------------------------"<<std::endl;

    instance.opendb();

    std::string from_filename{ argv[1] };
    std::string run_filename{ argv[2] };
    std::string exptag{ argv[3] };

    runworkload(from_filename, "");
    runworkload(run_filename, exptag);
    //unittest();

    instance.closedb();

    std::cout<<"--------------------------------------------------------"<<std::endl;
}

