#include <vector>
//
// Created by tidb on 4/26/21.
//

#ifndef REPLAYER_UTIL_H
#define REPLAYER_UTIL_H


std::vector<long long> getiostat(){
    std::vector<long long> _res;
    char buf[1102];
    char *devid="nvme0n1";

    FILE *p = popen("iostat -k", "r");
    while(fgets(buf, sizeof(buf), p)!=NULL){
        if(strstr(buf,devid)!=NULL){
            std::string line{ buf };
            std::cout<<line;
            //Device  tps  kB_read/s  kB_wrtn/s  kB_read  kB_wrtn
            std::string::size_type pos2 = line.rfind(" ");
            std::string::size_type pos3 = line.find("\n");
            std::string _kB_wrtn = line.substr(pos2+1, pos3);

            std::string rline = line.substr(0, pos2);
            std::string::size_type pos1 = rline.rfind(" ");
            std::string _kB_read = rline.substr(pos1+1, pos2);

            long long kB_read=atoll(_kB_read.c_str());
            long long kB_wrtn=atoll(_kB_wrtn.c_str());
            //std::cout<<"__"<<kB_read<<"__"<<kB_wrtn<<"__\n";
            _res.push_back(kB_read);
            _res.push_back(kB_wrtn);
        }
    }
    pclose(p);
    _res.push_back(0);
    _res.push_back(0);
    return(_res);
}


uint64_t cstr2int64(const char* key){
    int klen = strlen(key);
    uint64_t ikey=0;
    for(int i=0;i<klen;i++){
        ikey=ikey*10;
        ikey=ikey+((int)key[i]-'0');
    }
    return(ikey);
}

uint64_t str2int64(std::string _key){
    std::hash<std::string> hasher;
    uint64_t ikey=hasher(_key);
    return(ikey);
}

int find_nth_str(std::string tt, int nth){
    auto pos=boost::find_nth(tt, " ", nth);
    int pidx=std::distance(tt.begin(), pos.begin());
    return(pidx);
}

bool cmp(double x,double y){
    return x > y;
}
#endif //REPLAYER_UTIL_H
