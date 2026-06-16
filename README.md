# M8 – Basic ARP Traffic Inspector

## Project Description

This project is based on the Netfilter kernel module example provided in the `kernel-playground` repository.

The original example dropped incoming ICMPv6 packets. In this project, I modified the code to inspect ARP traffic instead of IPv6 traffic.

The module counts:

* ARP Request packets
* ARP Reply packets

The packets are **not blocked or modified**. The module only logs the number of observed ARP packets using `printk()`.

---

## Design Choices

I started from the original ICMPv6 Netfilter example and made the following modifications:

### Removed

* IPv6 packet parsing (`struct ipv6hdr`)
* ICMPv6 filtering logic
* Packet dropping (`NF_DROP`)

### Added

* ARP-related headers:

  * `linux/netfilter_arp.h`
  * `linux/if_arp.h`
  * `linux/if_ether.h`
* ARP packet parsing using:

```c
struct arphdr *arph;
arph = arp_hdr(skb);
```

* Two counters:

```c
static unsigned int arp_requests = 0;
static unsigned int arp_replies = 0;
```

* Detection of:

  * `ARPOP_REQUEST`
  * `ARPOP_REPLY`

* Logging using:

```c
printk(KERN_INFO "ARP Request received (total=%u)\n", arp_requests);

printk(KERN_INFO "ARP Reply received (total=%u)\n", arp_replies);
```

The hook returns:

```c
return NF_ACCEPT;
```

because the Basic version only monitors traffic without enforcing policies.

---

## Experimental Procedure

### 1. Build the module

```bash
make
```

### 2. Load the module

```bash
sudo insmod snf_lkm.ko
```

### 3. Verify that the module is loaded

```bash
lsmod | grep snf_lkm
```

### 4. Generate network traffic

For example:

```bash
ping -c 1 8.8.8.8
```

I also flushed the ARP cache to force new ARP exchanges:

```bash
sudo ip neigh flush all
```

### 5. Check kernel logs

```bash
sudo dmesg | tail -20
```

---

## Experimental Results

The module successfully counted ARP packets.

Example output:

```text
ARP Reply received (total=1)
ARP Reply received (total=2)
```

This demonstrates that the Netfilter hook correctly intercepted ARP traffic and updated the counters.

---

## What I Learned

Through this project, I learned:

* How Netfilter hooks work inside the Linux kernel.
* The difference between filtering IPv6 traffic and inspecting ARP traffic.
* How to parse ARP headers using kernel APIs.
* How to use `printk()` and `dmesg` for debugging kernel modules.
* How to compile, load, and test Linux kernel modules.
* How to use Git and GitHub to submit kernel development projects.

---

## Conclusion

The Basic ARP Traffic Inspector was successfully implemented, tested, and demonstrated. The module monitors ARP traffic by counting requests and replies while allowing packets to continue through the network stack.
