# FlexTOE: Flexible TCP Offload with Fine-Grained Parallelism

FlexTOE is a *flexible*, yet *high-performance* TCP offload engine (TOE) to
SmartNICs. FlexTOE eliminates almost all host data-path TCP processing
and is fully customizable. FlexTOE interoperates well with other TCP
stacks, is robust under adverse network conditions, and supports
POSIX sockets.

FlexTOE focuses on data-path offload of established connections,
avoiding complex control logic and packet buffering in the NIC.
FlexTOE leverages fine-grained parallelization of the TCP data-path and
segment reordering for high performance on wimpy SmartNIC
architectures, while remaining flexible via a modular design. We
compare FlexTOE on a
[Netronome Agilio-CX40](https://www.netronome.com/media/documents/PB_Agilio_CX_2x40GbE-7-20.pdf)
to host TCP stacks Linux and [TAS](https://doi.org/10.1145/3302424.3303985),
and to the
[Chelsio Terminator TOE](https://www.chelsio.com/wp-content/uploads/resources/Chelsio-Terminator-6-Brief.pdf).
We find that Memcached scales up to
38\% better on FlexTOE versus TAS, while saving up to 81\% host CPU
cycles versus Chelsio. FlexTOE provides competitive performance for
RPCs, even with wimpy SmartNICs. FlexTOE cuts 99.99th-percentile RPC
RTT by 3.2&#215; and 50\% versus Chelsio and TAS,
respectively. FlexTOE's data-path parallelism generalizes across
hardware architectures, improving single connection RPC throughput
up to 2.4&#215; on x86 and 4&#215; on
[BlueField](http://www.mellanox.com/related-docs/npu-multicore-processors/PB_BlueField_Ref_Platform.pdf).

FlexTOE supports C and XDP programs written in eBPF. It
allows us to implement popular data center transport features, such
as TCP tracing, packet filtering and capture, VLAN stripping, flow
classification, firewalling, and connection splicing.

For more information, please refer to:
- *NSDI 22*: https://www.usenix.org/conference/nsdi22/presentation/shashidhara
- *arXiv*: https://arxiv.org/abs/2110.10919

## Prerequisites
- [Netronome Agilio SmartNICs](https://www.netronome.com/products/smartnic/overview/) based on NFP-4000 or NFP-6000 architecture:
[out-of-tree driver](https://help.netronome.com/support/solutions/articles/36000049975-basic-firmware-user-guide#appendix-b-installing-the-out-of-tree-nfp-driver), SDK and [Board Support Packages](https://help.netronome.com/support/solutions/articles/36000049975-basic-firmware-user-guide#appendix-c-working-with-board-support-package).
- Gigantic (1G) hugepages (>= 1G available memory)
- Linux kernel 4.8.0+

## Run
- `make` to build.
- Setup [1G hugepages](https://www.kernel.org/doc/html/v4.14/admin-guide/kernel-parameters.html). Mount `hugetlbfs` at `/dev/hugepages-1048576kB`
```Shell
mount -t hugetlbfs -o pagesize=1G none /dev/hugepages-1048576kB
echo 4 | tee /sys/devices/system/node/node*/hugepages/hugepages-1048576kB/nr_hugepages
```

- Run
```Shell
# Load driver
modprobe uio
insmod kernel/flextoe_uio.ko

# Show supported network devices; Obtain pci_id
./scripts/devbind.py --status

# Bind network device to driver
./scripts/devbind.py -b flextoe_uio <pci_id>

# Run FlexTOE
./user/flextoe.out
```

## Usage
```
Usage: ./user/flextoe.out [OPTION]... --ip-addr=IP[/PREFIXLEN]

Memory Sizes:
  --shm-len=LEN               Shared memory len [default: 1073741824]
  --nic-rx-len=LEN            Kernel rx queue len [default: 256]
  --nic-tx-len=LEN            Kernel tx queue len [default: 256]
  --app-kin-len=LEN           App->Kernel queue len [default: 1048576]
  --app-kout-len=LEN          Kernel->App queue len [default: 1048576]

TCP protocol parameters:
  --tcp-rtt-init=RTT          Initial rtt for CC (us) [default: 50]
  --tcp-link-bw=BANDWIDTH     Link bandwidth (gbps) [default: 40]
  --tcp-rxbuf-len             Flow rx buffer len [default: 8192]
  --tcp-txbuf-len             Flow tx buffer len [default: 8192]
  --tcp-handshake-timeout=TIMEOUT  Handshake timeout (us) [default: 10000]
  --tcp-handshake-retries=RETRIES  Handshake retries [default: 10]

Congestion control parameters:
  --cc=ALGORITHM              Congestion-control algorithm [default: dctcp-rate]
     Options: dctcp-win, dctcp-rate, const-rate, timely
  --cc-control-granularity=G  Minimal control iteration [default: 50]
  --cc-control-interval=INT   Control interval (multiples of RTT) [default: 2]
  --cc-rexmit-ints=INTERVALS  #of RTTs without ACKs before rexmit [default: 4]
  --cc-dctcp-weight=WEIGHT    DCTCP: EWMA weight for ECN rate [default: 0.062500]
  --cc-dctcp-mimd=INC_FACT    DCTCP: enable multiplicative inc  [default: disabled]
  --cc-dctcp-min=RATE         DCTCP: minimum cap for flow rates  [default: 0]
  --cc-const-rate=RATE        Constant rate for all flows [default: 0]
  --cc-timely-tlow=TIME       Timely: low threshold (us) [default: 30]
  --cc-timely-thigh=TIME      Timely: high threshold (us) [default: 150]
  --cc-timely-step=STEP       Timely: additive increment step (kbps) [default: 10000]
  --cc-timely-init=RATE       Timely: initial flow rate (kbps) [default: 10000]
  --cc-timely-alpha=FRAC      Timely: EWMA weight for rtt diff [default: 0.020000]
  --cc-timely-beta=FRAC       Timely: mult. decr. factor [default: 0.800000]
  --cc-timely-minrtt=RTT      Timely: minimal rtt without queueing [default: 11]
  --cc-timely-minrate=RTT     Timely: minimal rate to use [default: 10000]

IP protocol parameters:
  --ip-route=DEST[/PREFIX],NEXTHOP  Add route
  --ip-addr=ADDR[/PREFIXLEN]        Set local IP address

ARP protocol parameters:
  --arp-timeout=TIMEOUT       ARP request timeout (us) [default: 500]
  --arp-timeout-max=TIMEOUT   ARP request max timeout (us) [default: 10000000]

Miscellaneous:
  --fp-poll-interval-app      App polling interval before blocking in us [default: 10000]
  --quiet                     Disable non-essential logging [default: disabled]
  --debug-console             Enable debug console [default: disabled]
```