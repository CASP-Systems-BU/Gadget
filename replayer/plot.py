import matplotlib
import matplotlib.pyplot as plt
import sys
import numpy as np

FNTSIZE=25
DPISIZE=120
LNESIZE=40

#matplotlib.rcParams['text.usetex'] = True
bar_width=0.17
colorset=[['tab:red','tab:green','tab:blue','tab:orange'], 
        ['tab:pink','olive','tab:cyan','gold'], 
        ['tomato','lightseagreen','steelblue','darkorange']]

kwd="mywl_"
dlist=['zipfian', 'uniform']
wlist=['a','d','f']

# dlist=['sequential', 'hotspot', 'latest']
# wlist=['tumbinc', 'contaggr']

kvgraph=['b','l','r','f']
kvtitle=['BerkerlyDB', 'Lethe', 'RocksDB', 'FASTER']
metrics=['p99','ops','p50']
# mrange=[300, 80000]
mlabel=['P99 latency', 'Throughput', 'P50 latency']
#_m=int(sys.argv[2])

lp99={}
lops={}
lp50={}


fnamelist=sys.argv[1:]
fnum=len(fnamelist)
print(fnamelist, fnum)
foutname="result_plot_"
for fname in fnamelist:
    foutname+=fname.replace('.txt','').replace('result','')+'_'

def insor1st(d,k,v):
    if(k in d):
        tmp=d[k]
        d[k]=tmp+v/fnum
    else:
        d[k]=v/fnum

for fname in fnamelist:
    ff=open(fname,'r').readlines()
    for _r, _l in enumerate(ff):
        if(_r>0):
            _l=_l.replace('\n','')
            fl=_l.split(', ')
            _wltype=fl[0]
            _p99=float(fl[1])
            _avg=float(fl[2])
            _p50=float(fl[3])
            _ops=float(fl[4])
            _kbr=float(fl[5])
            _kbw=float(fl[6])
            # print(_wltype, _p99, _avg, _ops, _p50)
            # lp99[_wltype]=_p99
            # lops[_wltype]=_ops
            # lp50[_wltype]=_p50
            insor1st(lp99, _wltype, _p99)
            insor1st(lp50, _wltype, _p50)
            insor1st(lops, _wltype, _ops)
            print(lp99, lp50, lops)

#plt.subplots(figsize=(10,8))
# plt.title("aa", fontsize=18)

for _m, mt in enumerate(metrics):
    i=0
    dlabel=[]
    # plt.figure(figsize=(40,40), dpi=DPISIZE, linewidth = LNESIZE)
    plt.figure(figsize=(15,10), linewidth = LNESIZE)
    for dtype in dlist:
        for wtype in wlist:
            wltype=wtype+'_'+dtype
            data=[]
            dlabel.append(wltype)
            for kvs in kvgraph:
                _k=kwd+wltype+'_'+kvs
                if(_m==0):
                    if(not (_k in lp99)):
                        data.append(0)
                    else:
                        data.append(lp99[_k])
                elif(_m==1):
                    if(not (_k in lops)):
                        data.append(0)
                    else:
                        data.append(lops[_k])
                elif(_m==2):
                    if(not (_k in lp50)):
                        data.append(0)
                    else:
                        data.append(lp50[_k])
            print(i,metrics[_m],wltype,data)
            for _d in range(len(data)):
                plt.bar(i+bar_width*_d, data[_d], bar_width, color=colorset[0][_d])
            #plt.ylim((0,mrange[_m]))
            i+=1
    plt.xticks(np.arange(len(dlabel))+bar_width*2, dlabel, rotation=45, fontsize=FNTSIZE)
    plt.yticks(fontsize=FNTSIZE)
    for _d in range(len(kvtitle)):
         plt.bar(0,0,0,color=colorset[0][_d],label=kvtitle[_d])
    plt.legend(fontsize=FNTSIZE)
    plt.ylabel(mlabel[_m],fontsize=FNTSIZE)
    plt.tight_layout()

    #plt.subplots_adjust(wspace =0.3, hspace =0.6)
    plt.savefig(foutname+'_'+metrics[_m]+'.png')
    #plt.show()
