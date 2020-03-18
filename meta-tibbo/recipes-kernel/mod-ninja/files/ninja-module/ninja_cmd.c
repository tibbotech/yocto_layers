#include "ninja_cmd.h"
#include "ninja.h"

extern ninja_skbq_t ninja_skbq_newq;
extern ninja_skbq_t ninja_skbq_sent;

extern struct sock_fprog bpf_filter;

static ninja_cmddesc_t ninja_cmddescs[] = {
 { NINJA_CMD_VERSION	, 0, 1, },
 { NINJA_CMD_INIT	, 0, 1, },
 { NINJA_CMD_CONFIRM	, 1, 1, },
 { NINJA_CMD_CLEARP	, 0, 1, },
 { NINJA_CMD_STATS	, 0, 1, },
 { NINJA_CMD_IFLST	, 0, 1, },
 { NINJA_CMD_IFSET	, 1, 0, },
 { NINJA_CMD_BPFSET	, 1, 0, },
 { NINJA_CMD_TESTMSG	, 0, 1, },
 { NINJA_CMD_ECHO	, 1, 1, },
};

ninja_cmddesc_t *tcmd_find_desc( NINJA_CMD_T _cmd) {
 int i;
 ninja_cmddesc_t *r = NULL, *tmpr;
 for ( i = 0; i < sizeof( ninja_cmddescs); i++) {
   tmpr = &ninja_cmddescs[ i];
   if ( _cmd == tmpr->cmd) {  r = tmpr;  break;  }
 }
 return( r);  }

void tcmd_stat2buf( unsigned char *_b, ninja_skbq_t *_s) {
 ninja_stats_t stats;
 memset( _b, 0, sizeof( ninja_stats_t));
 stats.pks_inq = skb_queue_len( &( _s->skbs));
 stats.bts_inq = _s->q_len;
 stats.pks_dropped = _s->q_dropped;
 memcpy( _b, &stats, sizeof( ninja_stats_t));
 return;  }

int tcmd_proc( kthread_t *_tp, NINJA_CMD_T _cmd, unsigned char *_in, int _inl) {
 ktime_t donet;
 bool fl;
 int cnt = 0, off = 0, err;
 struct sk_buff *tskb;
 ninja_ifdesc_t ninja_ifdesc;
 struct net_device *dev;
 if ( DBGM( NINJA_DBG_CMD)) KDBG( "%s(%d,%d)\n", __FUNCTION__, _cmd, _inl);
 switch ( _cmd) {
   case NINJA_CMD_VERSION:
     cnt = NINJA_VERSION_PROTOCOL;
     memcpy( _in, &cnt, sizeof( cnt));
     off += sizeof( cnt);
     return( off);
     break;
   case NINJA_CMD_CONFIRM:
     if ( _inl < sizeof( ktime_t)) break;
     // done till ... timestamp - remove from sent queue
     // form is : d<ktime_t>
     memcpy( &donet, _in, sizeof( ktime_t));
     fl = true;
     spin_lock( &( ninja_skbq_sent.my_spinlock));
     while ( ( tskb = skb_dequeue( &( ninja_skbq_sent.skbs))) && fl) {
       if ( memcmp( &( tskb->tstamp), &donet, sizeof( ktime_t)) == 0) fl = false;
       ninja_skbq_sent.q_len -= tskb->truesize;  cnt++;
       kfree_skb( tskb);  }
     spin_unlock( &( ninja_skbq_sent.my_spinlock));
     if ( DBGM( NINJA_DBG_CMD)) KDBG( "%d packets confirmed\n", cnt);
     return( 0);
     break;
   case NINJA_CMD_CLEARP:
     if ( _inl > 0 && _in[ 0] == 'N') {
       spin_lock( &( ninja_skbq_newq.my_spinlock));
       skb_queue_purge( &( ninja_skbq_newq.skbs));
       ninja_skbq_newq.q_len = 0;
       spin_unlock( &( ninja_skbq_newq.my_spinlock));
       return( 0);  }
     if ( _inl > 0 && _in[ 0] == 'S') {
       spin_lock( &( ninja_skbq_sent.my_spinlock));
       skb_queue_purge( &( ninja_skbq_sent.skbs));
       ninja_skbq_sent.q_len = 0;
       spin_unlock( &( ninja_skbq_sent.my_spinlock));
       return( 0);  }
     // by default clear all queues
     spin_lock( &( ninja_skbq_newq.my_spinlock));
     spin_lock( &( ninja_skbq_sent.my_spinlock));
     skb_queue_purge( &( ninja_skbq_newq.skbs));
     skb_queue_purge( &( ninja_skbq_sent.skbs));
     ninja_skbq_newq.q_len = ninja_skbq_sent.q_len = 0;
     spin_unlock( &( ninja_skbq_sent.my_spinlock));
     spin_unlock( &( ninja_skbq_newq.my_spinlock));
     break;
   // dump queue stats
   case NINJA_CMD_STATS:
     tcmd_stat2buf( _in + 0*sizeof( ninja_stats_t), &ninja_skbq_newq);
     tcmd_stat2buf( _in + 1*sizeof( ninja_stats_t), &ninja_skbq_sent);
     return( 2*sizeof( ninja_stats_t));
     break;
   // interfaces list
   case NINJA_CMD_IFLST:
     for_each_netdev( &init_net, dev) {
       if ( dev->flags & IFF_LOOPBACK) continue;
       memset( &ninja_ifdesc, 0, sizeof( ninja_ifdesc));
       memcpy( ninja_ifdesc.name, dev->name, IFNAMSIZ);
       ninja_ifdesc.type = NINJA_IFT_ETHER;
       ninja_ifdesc.desc.e.type = dev->type;
       ninja_ifdesc.desc.e.flags = dev->flags;
       memcpy( _in + off, &ninja_ifdesc, sizeof( ninja_ifdesc));
       off += sizeof( ninja_ifdesc);
     }
     return( off);
     break;
   // interface set flags
   case NINJA_CMD_IFSET:
     rtnl_lock();
     for ( cnt = 0; cnt < _inl/sizeof( ninja_ifdesc); cnt++) {
       memcpy( &ninja_ifdesc, _in + off, sizeof( ninja_ifdesc));
       if ( ninja_ifdesc.type != NINJA_IFT_ETHER) continue;
       dev = dev_get_by_name( &init_net, ninja_ifdesc.name);
       if ( !dev) continue;
       // +NETIF_F_RXALL_BIT ?
       err = dev_change_flags( dev, ninja_ifdesc.desc.e.flags);
       off += sizeof( ninja_ifdesc);  }
     rtnl_unlock();
     return( 0);
     break;
   // interface set flags
   case NINJA_CMD_BPFSET:
     if ( DBGM( NINJA_DBG_CMD)) KDBG( "new BPF len:%d\n", _inl);
     bpf_filter.len = _inl / sizeof( struct sock_filter);
     if ( ( _inl / sizeof( struct sock_filter)) < 1) {
       if ( bpf_filter.filter) kfree( bpf_filter.filter);
       bpf_filter.filter = NULL;  bpf_filter.len = 0;
       if ( DBGM( NINJA_DBG_CMD)) KDBG( "BPF cleared\n");
       return( 0);  }
     err = sk_chk_filter( ( struct sock_filter *)_in, ( _inl / sizeof( struct sock_filter)));
     if ( err != 0) {
       KERR( "BPF check error:%d\n", err);
       return( 0);  }
     bpf_filter.filter = ( struct sock_filter *)kmalloc( _inl, GFP_KERNEL);
     memcpy( bpf_filter.filter, _in, _inl);
     bpf_filter.len = _inl / sizeof( struct sock_filter);
     if ( DBGM( NINJA_DBG_CMD)) KDBG( "BPF set\n");
     if ( DBGM( NINJA_DBG_CMD)) print_hex_dump( KERN_DEBUG, "x ", DUMP_PREFIX_ADDRESS, 16, 1, bpf_filter.filter, _inl, true);
     return( 0);
     break;
   default:
     KINF( "cmd type %d unhandled in %s\n", _cmd, __FUNCTION__);
     break;
 } // switch /
 return( 0); }
