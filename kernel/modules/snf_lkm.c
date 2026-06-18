#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>

#include <linux/netfilter.h>
#include <linux/netfilter_arp.h>

#include <linux/if_arp.h>
#include <linux/if_ether.h>

#include <net/netns/generic.h>

static unsigned int lkm_net_id;

static unsigned int arp_requests = 0;
static unsigned int arp_replies = 0;

struct lkm_netns_data {
	struct nf_hook_ops nf_hops;
};

static unsigned int nf_callback(void *priv, struct sk_buff *skb,
                                const struct nf_hook_state *state)
{
    struct arphdr *arph;

    if (!skb)
        return NF_ACCEPT;

    arph = arp_hdr(skb);

    if (!arph)
        return NF_ACCEPT;


    if (arph->ar_op == htons(ARPOP_REQUEST)) {
        arp_requests++;
        printk(KERN_INFO "ARP Request received (total=%u)\n",
               arp_requests);
    }
    else if (arph->ar_op == htons(ARPOP_REPLY)) {
        arp_replies++;
        printk(KERN_INFO "ARP Reply received (total=%u)\n",
               arp_replies);
    }

    return NF_ACCEPT;
}


static const struct nf_hook_ops lkm_nf_hook_ops_template = {
	.hook		= nf_callback,
	.hooknum 	= NF_ARP_OUT,
	.pf		= NFPROTO_ARP,
	.priority       = 0,
};

static struct nf_hook_ops *lkm_nf_hook_ops(struct net *net)
{
	struct lkm_netns_data *netns_data = net_generic(net, lkm_net_id);

	return &netns_data->nf_hops;
}

static int __net_init netns_init(struct net *net)
{
	struct nf_hook_ops *ops = lkm_nf_hook_ops(net);
	int rc;

	/* Technically, it isn't necessary because we can use the
	 * lkm_nf_hook_ops_template directly. However, we demonstrate how to
	 * allocate storage for each network namespace and initialize it,
	 * primarily for documentation purposes.
	 */
	memcpy(ops, &lkm_nf_hook_ops_template, sizeof(*ops));

	rc = nf_register_net_hook(net, ops);
	if (rc) {
		printk("cannot register netfilter hook\n");
		return rc;
	}

	printk("netfilter hook registered\n");
	return 0;
}

static void __net_exit netns_exit(struct net *net)
{
	struct nf_hook_ops *ops = lkm_nf_hook_ops(net);

	nf_unregister_net_hook(net, ops);

	printk("netfilter hook unregistered\n");
}

static struct pernet_operations lkm_netns_ops = {
	.init = netns_init,
	.exit = netns_exit,
	.id = &lkm_net_id,
	.size = sizeof(struct lkm_netns_data),
};

static int __init lkm_init(void)
{
	int rc;

	rc = register_pernet_subsys(&lkm_netns_ops);
	if (rc) {
		printk("cannot register the pernet ops\n");
		return rc;
	}

	printk("lkm netfilter module registered\n");
	return 0;
}

static void __exit lkm_exit(void)
{
	unregister_pernet_subsys(&lkm_netns_ops);

	printk("lkm netfilter module unregistered\n");
}

module_init(lkm_init);
module_exit(lkm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohammad Rabiee");
MODULE_DESCRIPTION("Simple Linux kernel Netfilter Module for counting ARP requests and replies");
MODULE_VERSION("1.0.0");

