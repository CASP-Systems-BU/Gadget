import matplotlib
import matplotlib.pyplot as plt
import sys
import numpy as np
from matplotlib.pyplot import MultipleLocator
from matplotlib.ticker import ScalarFormatter

# kwd="mywl"
# dlist=['zipfian', 'uniform']
# wlist=['_a_','_d_','_f_']
# kvs=['_b','_l','_r','_f']
# metrics=['p99','ops','p50']


lidx=[['mywl_a_zipfian_r','mywl_d_zipfian_r','mywl_f_zipfian_r'],
      ['mywl_a_zipfian_l','mywl_d_zipfian_l','mywl_f_zipfian_l'],
      ['mywl_a_zipfian_f','mywl_d_zipfian_f','mywl_f_zipfian_f'],
      ['mywl_a_zipfian_b','mywl_d_zipfian_b','mywl_f_zipfian_b']]
# [[res for r], [res for l], [res for f], [res for b]]

p99set=[]
p50set=[]
opsset=[]

fnamelist=sys.argv[1:len(sys.argv)-1]
fnum=len(fnamelist)
print(fnamelist, fnum)
foutname="fig12_plot_"
for fname in fnamelist:
    foutname+=fname.replace('.txt','').replace('result','')+'_'

for fname in fnamelist:
    dp99={}
    dops={}
    dp50={}
    lp99=[[0,0,0],[0,0,0],[0,0,0],[0,0,0]]
    lops=[[0,0,0],[0,0,0],[0,0,0],[0,0,0]]
    lp50=[[0,0,0],[0,0,0],[0,0,0],[0,0,0]]
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
            dp99[_wltype]=_p99
            dops[_wltype]=_ops
            dp50[_wltype]=_p50
    for i in range(4):
        for j in range(3):
            kwd=lidx[i][j]
            lp99[i][j]=dp99[kwd]
            lp50[i][j]=dp50[kwd]
            lops[i][j]=dops[kwd]
    print('----------',fname)
    print(lp99)
    print(lp50)
    print(lops)
    p99set.append(lp99)
    p50set.append(lp50)
    opsset.append(lops)


FLG=sys.argv[-1]
if(FLG=="p99"):
    ERROR = np.std((p99set[0],p99set[1],p99set[2]), axis=0, ddof=1)
    DATA =  np.mean((p99set[0],p99set[1],p99set[2]), axis=0)
    ylabelstr="Latency P99.9(us) (log)"
elif(FLG=="p50"):
    ERROR = np.std((p50set[0],p50set[1],p50set[2]), axis=0, ddof=1)
    DATA =  np.mean((p50set[0],p50set[1],p50set[2]), axis=0)
    ylabelstr="Latency P50(us) (log)"
elif(FLG=="ops"):
    ERROR = np.std((opsset[0],opsset[1],opsset[2]), axis=0, ddof=1)
    DATA =  np.mean((opsset[0],opsset[1],opsset[2]), axis=0)
    ylabelstr="Throughput (log)"


print(DATA)


matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
font = {'family' : 'normal',
    # 'weight' : 'bold',
    'size'  : 42}
font_label = {'family' : 'normal',
    # 'weight' : 'bold',
    'size'  : 42}
font_xlabel = {'family' : 'normal',
    'weight' : 'bold',
    'size'  : 42}
plt.rc('font', **font)
plt.rc('legend', fontsize=42)


fig, ax1 = plt.subplots(figsize=(15,10))

# ax1.set_ylabel("Mean Response Time (" + r'$\mu$' + "s)" ,**font_label)
# ax1.set_xticklabels(["", "Baseline",  "+Marking", "", ""], rotation=-20)
ax1.grid(which='major', axis='y', linestyle='--')
#ax1.set_ylim(0, 10)
# ax1.set_yticks(np.arange(0,37,4))
ax1.set_xticks(np.arange(len(DATA[0])))
ax1.set_axisbelow(True)

# ax1.set_xticklabels(["", 55, 65, 75, 85, 95])
# ax1.set_xticklabels(["Cont. Aggr.", "Cont. Join", "Join Sliding", "Join Tumbling", "Sliding Hol.", "Sliding Incr.", "Tumbing Hol.", "Tumbing Incr.", "Join Interval", "Session Hol.", "Session Incr."], rotation = 45, **font_xlabel)
ax1.set_xticklabels(["A", "D", "F"], rotation = 0, **font_xlabel)
plt.ylabel(ylabelstr, **font_label)
plt.setp(ax1.xaxis.get_majorticklabels(), ha='right')

plt.xlabel("", **font_label)

# ax1.set_yscale('log')
# ax1.yaxis.set_major_formatter(ScalarFormatter())

# get rid of the frame
# for spine in plt.gca().spines.values()[1:]:
#     print(spine)
#     spine.set_visible(False)
ax1.spines['right'].set_visible(False)
ax1.spines['left'].set_visible(False)
ax1.spines['top'].set_visible(False)
plt.setp(ax1.spines.values(), color="black")
if(FLG=="ops"):
    ax1.set_yscale('log')
    ax1.set_ylim((4*pow(10,5),1.5*pow(10,6)))
    ax1.yaxis.set_major_locator(MultipleLocator(1000000))
else:
    ax1.set_yscale('log')
    ax1.set_ylim((pow(10,0),1.5*pow(10,1)))
    ax1.yaxis.set_major_locator(MultipleLocator(10))

# ax1.grid()
# ax1.set_xticks(np.arange(0,91,10))
# ax1.set_xticklabels([])
# ax1.set_yticks(np.arange(1,1100000,250000))
# ax1.set_yticklabels(['0','.25','.50','.75','1'])
# ax1.set_xlim(0,89)
BAR_LEN = 0.2
B2 = 0.15
x = np.arange(len(DATA[0]))
# p1 = ax1.bar(x - (3/2)*BAR_LEN, DATA[0], B2, yerr=ERROR[0], hatch='//', edgecolor='gray', color='darkblue', ecolor='red', capsize=3, label="RocksDB", alpha=0.9)
# p2 = ax1.bar(x - (1/2)*BAR_LEN, DATA[1], B2, yerr=ERROR[1],  hatch='-', edgecolor='gray', color='darkred', ecolor='red', capsize=3, label="Lethe", alpha=0.9)
# p3 = ax1.bar(x + (1/2)*BAR_LEN, DATA[2], B2, yerr=ERROR[2], hatch='\\',edgecolor='gray',  color='darkgoldenrod', ecolor='red', capsize=3, label="Faster", alpha=0.9)
# p4 = ax1.bar(x + (3/2)*BAR_LEN, DATA[3], B2, yerr=ERROR[3],  hatch='+', edgecolor='gray', color='darkgreen', ecolor='red', capsize=3, label="BerkeleyDB", alpha=0.9)
p1 = ax1.bar(x - (3/2)*BAR_LEN, DATA[0], B2, yerr=ERROR[0], color='tab:gray', hatch='//' , edgecolor='gray', ecolor='red', capsize=3, label='RocksDB', alpha=0.9)
p2 = ax1.bar(x - (1/2)*BAR_LEN, DATA[1], B2, yerr=ERROR[1], color='tab:olive', hatch='-', edgecolor='gray', ecolor='red', capsize=3, label='Lethe', alpha=0.9)
p3 = ax1.bar(x + (1/2)*BAR_LEN, DATA[2], B2, yerr=ERROR[2], color='tab:cyan', hatch='\\', edgecolor='gray', ecolor='red', capsize=3, label='Faster', alpha=0.9)
p4 = ax1.bar(x + (3/2)*BAR_LEN, DATA[3], B2, yerr=ERROR[3], color='tab:purple', hatch='+', edgecolor='gray', ecolor='red', capsize=3, label='BerkeleyDB', alpha=0.9)

plt.legend(loc='upper center', ncol=4, bbox_to_anchor=(0.4, 1.2), frameon=False, prop={'size': 42}, columnspacing=0.3)

plt.savefig(foutname+FLG+'.pdf', format='pdf', dpi=300, bbox_inches='tight')
# plt.show()
