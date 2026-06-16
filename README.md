# M8 – ARP Traffic Inspector (Basic Level)

## Project Description

This project is based on the Netfilter kernel module example provided in the `kernel-playground` repository.

The original example inspected ICMPv6 traffic. I modified it to inspect ARP traffic instead.

The module counts ARP packets and prints the results to the kernel log using `printk()`.

## Design Choices

The following modifications were made to the original code:

* Removed the IPv6 and ICMPv6 specific logic.
* Added ARP packet parsing using `arp_hdr()`.
* Added counters for ARP Requests and ARP Replies.
* Kept `NF_ACCEPT` so that packets are not blocked and can continue normally through the network stack.

## How to Reproduce the Experiment

1. Compile the module:

```bash
cd kernel/modules
make
```

2. Load the module:

```bash
sudo insmod snf_lkm.ko
```

3. Generate network traffic:

```bash
sudo ip neigh flush all
ping -c 1 8.8.8.8
```

4. Check the kernel logs:

```bash
sudo dmesg | tail -20
```

5. Remove the module:

```bash
sudo rmmod snf_lkm
```

## Experimental Results

During testing, the module successfully detected ARP traffic and updated the counters.

Example output:

```
ARP Reply received (total=1)
ARP Reply received (total=2)
```

## Conclusion

This project helped me understand how Netfilter hooks work inside the Linux kernel, how ARP packets can be inspected, and how Linux kernel modules can be compiled, loaded, and tested.
