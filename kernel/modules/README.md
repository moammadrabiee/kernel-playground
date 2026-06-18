# M8 – ARP Traffic Inspector (Basic Level)

## Overview

This project implements the **ARP Traffic Inspector** assignment using a Linux Kernel Module based on the Netfilter framework.

The implementation is derived from the Netfilter example provided in the *kernel-playground* repository. The original example inspected ICMPv6 traffic, while this version has been modified to inspect ARP traffic and count both:

* ARP Requests
* ARP Replies

The module does not modify, block, or drop packets. All packets are accepted and continue through the Linux networking stack normally.

---

## Project Objectives

The objectives of this project are:

* Understand Linux Kernel Module development.
* Learn how Netfilter hooks work inside the Linux kernel.
* Inspect ARP packets at kernel level.
* Count ARP Requests and ARP Replies.
* Validate packet inspection through practical experiments.

---

## Implementation Details

### Netfilter Hooks

The module registers two Netfilter hooks:

```c
NF_ARP_OUT
```

Used to inspect outgoing ARP packets (typically ARP Requests).

```c
NF_ARP_IN
```

Used to inspect incoming ARP packets (typically ARP Replies).

Using both hooks allows the module to count ARP Requests and ARP Replies during the same execution.

### Packet Processing

For every ARP packet:

1. The packet is accessed through the `sk_buff` structure.
2. The ARP header is extracted using:

```c
arp_hdr(skb);
```

3. The ARP operation field (`ar_op`) is checked.
4. The corresponding counter is updated.

### Packet Types Detected

#### ARP Request

```c
ARPOP_REQUEST
```

#### ARP Reply

```c
ARPOP_REPLY
```

### Counters

The module maintains two independent counters:

```c
arp_requests
arp_replies
```

### Packet Forwarding

The callback always returns:

```c
NF_ACCEPT
```

Therefore, packets are inspected but never blocked or modified.

---

## Source Files

Main implementation:

```text
kernel/modules/snf_lkm.c
```

Compiled module:

```text
kernel/modules/snf_lkm.ko
```

---

## Build Instructions

Move to the module directory:

```bash
cd kernel/modules
```

Compile the module:

```bash
make clean
make
```

---

## Running the Experiment

### 1. Load the Module

```bash
sudo insmod snf_lkm.ko
```

Verify successful loading:

```bash
sudo dmesg | tail -20
```

Expected output:

```text
ARP IN and OUT hooks registered
lkm netfilter module registered
```

---

### 2. Generate ARP Traffic

Clear the ARP cache:

```bash
sudo ip neigh flush all
```

Generate network traffic:

```bash
ping -c 1 8.8.8.8
```

---

### 3. Observe the Results

Display ARP-related logs:

```bash
sudo dmesg | grep ARP
```

Example output:

```text
ARP Request received (total=1)
ARP Reply received (total=1)
```

---

### 4. Unload the Module

```bash
sudo rmmod snf_lkm
```

Verify successful removal:

```bash
sudo dmesg | tail -20
```

Expected output:

```text
ARP IN and OUT hooks unregistered
lkm netfilter module unregistered
```

---

## Experimental Results

The module successfully detected and counted both ARP Requests and ARP Replies.

During testing:

```text
ARP Request received (total=1)
ARP Reply received (total=1)
```

This confirms that both Netfilter hooks are functioning correctly.

---

## Screenshots

The repository includes the following screenshots:

1. Module compilation (`build.png`)
2. Module loading (`module_loaded.png`)
3. Traffic generation (`generate_traffic.png`)
4. ARP detection (`arp_detected.png`)
5. Module removal (`module_removed.png`)

---

## Design Choices

The following design decisions were made:

* Reused the official Netfilter example provided in the course repository.
* Replaced ICMPv6 packet inspection with ARP packet inspection.
* Used two Netfilter hooks (`NF_ARP_IN` and `NF_ARP_OUT`) to observe both directions of ARP traffic.
* Extracted ARP headers using `arp_hdr()`.
* Maintained separate counters for Requests and Replies.
* Returned `NF_ACCEPT` for every packet to ensure non-intrusive monitoring.

---

## Conclusion

This project demonstrates how Linux Kernel Modules can be combined with Netfilter hooks to inspect network traffic at kernel level.

Through this implementation, I gained practical experience with:

* Linux kernel programming
* Netfilter packet processing
* ARP protocol inspection
* Kernel logging with `printk`
* Building and testing custom kernel modules

The final implementation successfully fulfills the requirements of the **ARP Traffic Inspector** assignment by counting both ARP Requests and ARP Replies without interfering with normal network operation.
