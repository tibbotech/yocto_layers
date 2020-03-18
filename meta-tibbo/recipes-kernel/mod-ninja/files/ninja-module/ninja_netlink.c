#include "ninja_netlink.h"
#include "ninja_cmd.h"

extern int netlink_id;

struct sock *nl_sk = NULL;

static void dv_nl_in( struct sk_buff *_in) {
 struct nlmsghdr *h_rx, *h_tx;
 struct sk_buff *sk_rx, *sk_tx;
 int rx_data_len, tx_data_len, ret;
 char *in_req;
// KDBG( "%s()\n", __FUNCTION__);
 if ( !_in) {  KDBG( "%s _in is NULL /\n", __FUNCTION__);  return;  }
 if ( ( _in->len < NLMSG_SPACE( 0))) return;
 sk_rx = skb_get( _in);
 h_rx = nlmsg_hdr( sk_rx);
 in_req = ( char *)NLMSG_DATA( h_rx);
 rx_data_len = h_rx->nlmsg_len - NLMSG_HDRLEN;
//print_hex_dump( KERN_INFO, "nlin ", DUMP_PREFIX_ADDRESS, 16, 1, in_req, rx_data_len, true);
//KINF( "%s sizeof:%d\n", __FUNCTION__, NLMSG_HDRLEN);
//KINF( "%s got payload: %s (seq:%d,in->len:%d,nlmsg_len:%d)\n", __FUNCTION__, in_req, h_rx->nlmsg_seq, _in->len, h_rx->nlmsg_len);
 // form a reply
// FIXME
 ret = tcmd_proc( NULL, in_req[ 0], ( in_req + 1), rx_data_len - 1);
// KDBG( "%s() rx_data_len:%d ret:%d\n", __FUNCTION__, rx_data_len, ret);
 if ( ret < 1) goto l_out;
 tx_data_len = ret;
 if ( !( sk_tx = alloc_skb( NLMSG_SPACE( tx_data_len), GFP_KERNEL))) goto l_out;
 h_tx = nlmsg_put( sk_tx, h_rx->nlmsg_pid, h_rx->nlmsg_seq, NLMSG_DONE, tx_data_len, 0);
 memcpy( NLMSG_DATA( h_tx), in_req + 1, tx_data_len);
//print_hex_dump( KERN_DEBUG, "x ", DUMP_PREFIX_ADDRESS, 16, 1, NLMSG_DATA( h_tx), tx_data_len, true);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
 NETLINK_CB( sk_tx).portid = 0;
#else
 NETLINK_CB( sk_tx).pid = 0;
#endif
 netlink_unicast( nl_sk, sk_tx, h_rx->nlmsg_pid, MSG_DONTWAIT);
 l_out:
 kfree_skb( sk_rx);
// KDBG( "%s() /\n", __FUNCTION__);
}

// ---------- main (exported) functions
void ninja_netlink_init( void) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
 struct netlink_kernel_cfg cfg = {  .input = dv_nl_in,  };
 nl_sk = netlink_kernel_create( &init_net, netlink_id, &cfg);
#else
 nl_sk = netlink_kernel_create( &init_net, netlink_id, 0, dv_nl_in, NULL, THIS_MODULE);
#endif
 if ( !nl_sk) {
   KERR( "can't create netlink id:%d\n", netlink_id);
   netlink_id = -1;
   return;  }
 KDBG( "netlink socket registered ID:%d\n", netlink_id);
 return;  }

void ninja_netlink_clean( void) {
 KDBG( "%s()\n", __FUNCTION__);
 if ( nl_sk) netlink_kernel_release( nl_sk);
 KDBG( "%s() /\n", __FUNCTION__);
 return;  }
