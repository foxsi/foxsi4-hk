import sys, re

import numpy as np
from matplotlib import pyplot as plt

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("use like this:\n\t$ python plot_ping.py path/to/your/file.txt, path/to/save.pdf")
        sys.exit(0)
    
    fname = sys.argv[1]
    print("opening", fname, "...")
    icmp_seq = []
    rtt = []
    with open(fname, 'r') as f:
        for line in f.readlines():
            match_icmp = re.search(rf'(icmp_seq=)([1-9]\d*)', line)
            if match_icmp:
                icmp_seq.append(int(match_icmp.group(2)))
                if 'no answer yet' in line:
                    rtt.append(np.nan)
                else:
                    match_rtt = re.search(rf'(time=)(([0-9]*[.])?[0-9]+)', line)
                    if not match_rtt:
                        rtt.append(np.nan)
                    else:
                        rtt.append(np.float64(match_rtt.group(2)))
                print("seq=", icmp_seq[-1], "rtt=", rtt[-1])
    print(len(icmp_seq))
    print(len(rtt))


    fig = plt.figure(figsize=(8,4))

    plt.plot(icmp_seq, rtt)
    plt.grid(visible=True)
    plt.xlabel(xlabel="ICMP sequence number")
    plt.ylabel(ylabel="Ping time (round trip) [ms]")
    plt.title(label="Housekeeping ping response times and failure")
    plt.ylim([0,3000])
    plt.savefig(sys.argv[2])
    plt.show()
