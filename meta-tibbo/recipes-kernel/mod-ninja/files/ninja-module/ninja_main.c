#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include "ninja.h"

#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>

#include "ninja_th.h"
#include "ninja_io_net.h"
#include "ninja_io_usb.h"
#include "ninja_procfs.h"
#include "ninja_netlink.h"

MODULE_LICENSE(M_LIC);
MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM", "M_ORG" "M_CPR);

int maxq_free = NINJA_MAXQ_FREE;
ulong maxq_mem = NINJA_MAXQ_MEM;
uint net_port = NINJA_NET_PORT;
int netlink_id = NINJA_NETLINK_ID;

ulong debug_mask;

module_param( maxq_free, int, 0644);
MODULE_PARM_DESC( maxq_free, "drop packets on memory limit(-1=head,0=noqnew,+1=tail)");
module_param( maxq_mem, ulong, 0644);
MODULE_PARM_DESC( maxq_mem, "max total packets mem size (KB)");
module_param( net_port, uint, 0644);
MODULE_PARM_DESC( net_port, "tcp and udp (data and commands) port");
module_param( netlink_id, int, 0644);
MODULE_PARM_DESC( netlink_id, "netlink preferred id");
module_param( debug_mask, ulong, 0644);
MODULE_PARM_DESC( debug_mask, "debug flags");

kthread_t ninja_threads;
int thread_counter;

kthread_t *thread_worker = NULL;

ninja_skbq_t ninja_skbq_newq;
ninja_skbq_t ninja_skbq_sent;

struct sock_fprog bpf_filter;

int pass_filter( struct sk_buff *_skb) {
 int res;
 if ( bpf_filter.len == 0) return( 1);
// res = sk_run_filter( _skb, bpf_filter.filter, bpf_filter.len);
 res = sk_run_filter( _skb, bpf_filter.filter);
// KDBG( "filter ret:%d (flen:%d)\n", res, bpf_filter.len);
 if ( !res) return( 0);
 return( 1);  }

int ismyown_skb( struct sk_buff *_skb) {
 struct iphdr *iph = ( struct iphdr *)skb_network_header( _skb);
 struct tcphdr *tcph;
 struct udphdr *udph;
 __be16 p_d = 0, p_s = 0;
 int off = ( iph->ihl << 2);	// we are exactly have IP packet there
// char *inout = "in";
 if ( _skb->pkt_type == PACKET_OUTGOING) off += 14;
 if ( iph->protocol == IPPROTO_TCP) {
   tcph = ( struct tcphdr *)( _skb->data + off);
   p_d = htons( tcph->dest);  p_s = htons( tcph->source);
 }
 if ( iph->protocol == IPPROTO_UDP) {
   udph = ( struct udphdr *)( _skb->data + off);
   p_d = htons( udph->dest);  p_s = htons( udph->source);
 }
 if ( _skb->pkt_type == PACKET_HOST && p_d == net_port) return( 1);
 if ( _skb->pkt_type == PACKET_OUTGOING && p_s == net_port) return( 1);
// if ( DBGM( NINJA_DBG_NET)) {
//   if ( _skb->pkt_type == PACKET_HOST) inout = "in";
//   if ( _skb->pkt_type == PACKET_OUTGOING) inout = "out";
//   KDBG( "got TCP|udp(%s,iplen:%d) s:%d.%d.%d.%d:%d d:%d.%d.%d.%d:%d\n", inout, htons( iph->tot_len), NIPQUAD(iph->saddr), p_s, NIPQUAD(iph->daddr), p_d);
//   print_hex_dump( KERN_DEBUG, "x ", DUMP_PREFIX_ADDRESS, 16, 1, tcph, sizeof( struct tcphdr), true);
// }
 return( 0); }

// _d1 - IN: orig dev, OUT: _d1 == _d0
int rcv_all( struct sk_buff *_skb, struct net_device *_d0, struct packet_type *_pt, struct net_device *_d1) {
 struct iphdr *iph;
 struct sk_buff *tskb;
 ulong tlN, tlS;
 // not module-generated packet type (!IP) - queue asap
 if ( ntohs( _skb->protocol) != ETH_P_IP) goto add_packet;
 if ( _skb->pkt_type != PACKET_HOST && 
      _skb->pkt_type != PACKET_OUTGOING
    ) goto add_packet;
 iph = ( struct iphdr *)skb_network_header( _skb);
 // not module-generated packet type (!UDP && !TCP) - queue asap
 if ( iph->protocol != IPPROTO_TCP && iph->protocol != IPPROTO_UDP) goto add_packet;
 // IP: slow check - parse header
 if ( ismyown_skb( _skb) == 0) goto add_packet;
 kfree_skb( _skb);
 return( NET_RX_SUCCESS);
 add_packet:
 // try filter there (if exist)
 if ( pass_filter( _skb) == 1) goto add_packet2;
 kfree_skb( _skb);
 return( NET_RX_SUCCESS);
 add_packet2:
 spin_lock_bh( &( ninja_skbq_newq.my_spinlock));
 tlN = ninja_skbq_newq.q_len;
 tlS = ninja_skbq_sent.q_len;
 // while no space in queue
 while ( ( tlN + tlS + _skb->truesize) >= maxq_mem) {
   ninja_skbq_newq.q_dropped++;
   // can't free such a big space - do not process new packet
   if ( tlN < _skb->truesize) {  kfree_skb( _skb);  goto go_out;  }
   // if 'nodrop' - do not process new packet
   if ( maxq_free == 0) {  kfree_skb( _skb);  goto go_out;  }
   // drop from head or from tail
   if ( maxq_free  < 0) tskb = skb_dequeue(      &( ninja_skbq_newq.skbs));
   if ( maxq_free  > 0) tskb = skb_dequeue_tail( &( ninja_skbq_newq.skbs));
   if ( !tskb) {
     KERR( "exception 0 at %s:%d\n", __FUNCTION__, __LINE__);
     kfree_skb( _skb);  goto go_out;  }
   if ( ninja_skbq_newq.q_len < tskb->truesize) {
     KERR( "exception 1 at %s:%d\n", __FUNCTION__, __LINE__);
     skb_queue_purge( &( ninja_skbq_newq.skbs));
     ninja_skbq_newq.q_len = 0;
     kfree_skb( _skb);  goto go_out;  }
   ninja_skbq_newq.q_len -= tskb->truesize;
   tlN = ninja_skbq_newq.q_len;
   // may be out queue already flushed by other thread - read it again
   tlS = ninja_skbq_sent.q_len;
   kfree_skb( tskb);
 }
 _skb->tstamp = ktime_get();
 ninja_skbq_newq.q_len += _skb->truesize;
#if 0
// tskb = skb_clone( _skb, GFP_ATOMIC);
// skb_queue_tail( &( ninja_skbq_newq.skbs), tskb);
// kfree_skb( _skb);
#else
// this new code is faster... but is it queue-safe?
// manual says, skb have to be in only one queue...
 skb_queue_tail( &( ninja_skbq_newq.skbs), _skb);
#endif
 go_out:
 spin_unlock_bh( &( ninja_skbq_newq.my_spinlock));
 return( NET_RX_SUCCESS);  }

static struct packet_type all_pt = {
 .type 	= __constant_htons(ETH_P_ALL),
 .dev 	= NULL,	// all devices
 .func 	= rcv_all,
};

// it's not really called from thread->C (destroy)
int th_wrkr_stop( kthread_t *_tp) {
 kthread_t *ttp;
 int err = 0, not = 0;		// not is the number of threads
 struct list_head *pos, *q;
 list_for_each_safe( pos, q, &( ninja_threads.list)) {
   ttp = list_entry( pos, kthread_t, list);
   // worker never ends, ->C (destroy) will be called explicitely!
   if ( ttp->is_worker) continue;
   KDBG( "%s for '%s'\n", __FUNCTION__, ttp->thname);
   not++;
   // sinal to stop
   if ( ttp->state != DV_THS_DELIST) dv_th_stop( ttp);
   err = th_state_handle( ttp);
   if ( err == -1) {  list_del( pos);  kfree( ttp);  }
 }
 return( not);  }

int th_wrkr_start( void *_tp) {
 kthread_t *tp = ( kthread_t *)_tp;
 kthread_t *ttp;
 struct list_head *pos, *q;
 int err = 0;
 tp->is_worker = 1;	// mark me not to kill
 while ( 1) {
   msleep( 10);
   if ( kthread_should_stop()) break;
   schedule();
   list_for_each_safe( pos, q, &( ninja_threads.list)) {
     ttp = list_entry( pos, kthread_t, list);
     // worker never ends, ->C (destroy) will be called explicitely!
     if ( ttp->is_worker) continue;
     err = th_state_handle( ttp);
     if ( err == -1) {  list_del( pos);  kfree( ttp);  }
   }
 }
 // at the module unloading
 KDBG( "%s end '%s'\n", __FUNCTION__, tp->thname);
 // wait till all of non-worker threads will be finished
 while ( th_wrkr_stop( tp) > 0);
 return( 0);  }


int init_module( void) {
 th_tasks_t task_main = {
    .O = &th_wrkr_start,
    .W = NULL,
    .C = &th_wrkr_stop
 };
 char *p = "noqnew";
 KINF( M_NAM", "M_ORG" "M_CPR);
 thread_counter = 0;
 memset( &ninja_skbq_newq, 0, sizeof( ninja_skbq_t));
 memset( &ninja_skbq_sent, 0, sizeof( ninja_skbq_t));
 skb_queue_head_init( &( ninja_skbq_newq.skbs));
 skb_queue_head_init( &( ninja_skbq_sent.skbs));
 spin_lock_init( &( ninja_skbq_newq.my_spinlock));
 spin_lock_init( &( ninja_skbq_sent.my_spinlock));
 // why /2 ? because! :) seems, packets take memory two times
 maxq_mem *= 1024/2;
 debug_mask = cpu_to_le32( debug_mask);
 KINF( "debug mask 0x%08x\n", debug_mask);
 KINF( "memory limit %lu MB\n", maxq_mem);
 if ( maxq_free < 0) p = "drop_from_tail";
 if ( maxq_free > 0) p = "drop_from_head";
 KINF( "memory barrier handler '%s'\n", p);
 INIT_LIST_HEAD( &( ninja_threads.list));
 dev_add_pack( &all_pt);
 thread_worker = dv_th_init_task( MNAME"/worker", task_main);
 dv_th_run( thread_worker);
 ninja_io_net_init();
 ninja_io_usb_init();
 ninja_procfs_init();
 ninja_netlink_init();
 return( 0);  }

void cleanup_module( void) {
 dev_remove_pack( &all_pt);
 // FIXME: race condition?
 skb_queue_purge( &( ninja_skbq_newq.skbs));
 skb_queue_purge( &( ninja_skbq_sent.skbs));
 dv_th_stop( thread_worker);
 ninja_io_net_clean();
 ninja_io_usb_clean();
 ninja_procfs_clean();
 ninja_netlink_clean();
 return; }
