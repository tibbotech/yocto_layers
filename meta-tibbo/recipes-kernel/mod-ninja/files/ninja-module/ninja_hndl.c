#include "ninja_hndl.h"

extern ninja_skbq_t ninja_skbq_newq;
extern ninja_skbq_t ninja_skbq_sent;

static unsigned char buf[ NINJA_BUFSZ_C + 1];
static unsigned char nbuf1[ NINJA_BUFSZ_C + 1];

int netcmd_proc( kthread_t *_tp, unsigned char *_in, int _inl) {
 int gots;
 // debug
// struct sockaddr_in *addr = ( struct sockaddr_in *)( _tp->peer_id);
 memset( nbuf1, 0, NINJA_BUFSZ_C + 1);
 // process network command
 if ( _in[ 0] == 'd') tcmd_proc( _tp, NINJA_CMD_CONFIRM, _in + 1, _inl - 1);
 if ( _in[ 0] == 'c') tcmd_proc( _tp, NINJA_CMD_CLEARP , _in + 1, _inl - 1);
 // debug
//KDBG( "'%s' received %d bytes:\n", _tp->thname, _inl);
//KDBG( "%s\n", _in);
//if ( addr) KDBG( "from %d.%d.%d.%d:%d\n", NIPQUAD( addr->sin_addr.s_addr), htons( addr->sin_port));
//print_hex_dump( KERN_DEBUG, "x ", DUMP_PREFIX_ADDRESS, 16, 1, _in, _inl, true);
 sprintf( nbuf1 + strlen( nbuf1), "pks:%u/%u", skb_queue_len( &( ninja_skbq_newq.skbs)), skb_queue_len( &( ninja_skbq_sent.skbs)));
 sprintf( nbuf1 + strlen( nbuf1), " bts:%lu/%lu", ninja_skbq_newq.q_len, ninja_skbq_sent.q_len);
 sprintf( nbuf1 + strlen( nbuf1), " dropped:%lu/%lu\n", ninja_skbq_newq.q_dropped, ninja_skbq_sent.q_dropped);
 gots = dv_s_W( _tp, nbuf1, strlen( nbuf1));
 if ( gots < 0) KINF( "sending '%s' data E=%d\n", _tp->thname, gots);
 if ( gots < 0) return( gots);
 return( 0); }

int dv_io_sendnjskb( kthread_t *_tp, struct sk_buff *_skb) {
 int off = 0, err = 0;
 unsigned char nl[ 2] = "\r\n";
 memset( buf, 0, NINJA_BUFSZ_C + 1);
 memcpy( ( buf + off), &( _skb->tstamp), sizeof( ktime_t));
 off += sizeof( ktime_t);
 memcpy( ( buf + off), ( _skb->dev->name), IFNAMSIZ);
 off += IFNAMSIZ;
 // other fields?
 err = dv_s_W( _tp, buf, off);
 if ( err < 0) return( err);
 err = dv_s_W( _tp, _skb->data, _skb->len);
// debugging
// memset( &buf, 0, NINJA_BUFSZ_C*4 + 1);
// sprintf( buf, "%d(%d)\r\n", skb_queue_len( &( ninja_skbq_newq.skbs)), ninja_skbq_newq.q_len);
// dv_s_W( _tp, buf, strlen( buf));
// memset( &buf, 0, NINJA_BUFSZ_C*4 + 1);
// sprintf( buf, "%d(%d)\r\n", _skb->len, _skb->data_len);
// dv_s_W( _tp, buf, strlen( buf));
// newline : remove later
 dv_s_W( _tp, nl, 2);
// debugging /
 return( err);  }

// _tp - current thread pointer
int dv_io_rw_dch( kthread_t *_tp) {
 th_buffs_t *bt = ( th_buffs_t *)_tp->th_data;
 int gots, err, ret;
 struct sk_buff *tskb;

 // allocate IN buffers if needed
 if ( bt == NULL && !( bt = dv_s_new_thdata( _tp))) return( -1);
 gots = dv_s_R( _tp, bt->b_d_in, NINJA_BUFSZ_D);
 // <0 means error - finish connection
 if ( gots < 0) KINF( "getting '%s' data E=%d\n", _tp->thname, gots);
 if ( gots < 0) return( -1);
 err = 0;
 // got some command from peer - handle it
 if ( gots > 0) err = netcmd_proc( _tp, bt->b_d_in, gots);
 if ( err != 0) return( -1);
 ret = 1;
 spin_lock_bh( &( ninja_skbq_newq.my_spinlock));
 // if no data to send - continue the external loop
 if ( skb_queue_len( &( ninja_skbq_newq.skbs)) < 1) {  ret = 0;  goto out_ok;  }
 // send 1 portion of data
 tskb = skb_peek( &( ninja_skbq_newq.skbs));
 if ( !tskb) goto out_ok;
 err = dv_io_sendnjskb( _tp, tskb);
 if ( err < 0) {
   KINF( "sending '%s' data E=%d\n", _tp->thname, err);
   ret = -1;  goto out_ok;  }
 // now remove packet from buffer and move it to the sent q
 tskb = skb_dequeue( &( ninja_skbq_newq.skbs));
 if ( !tskb) goto out_ok;
 ninja_skbq_newq.q_len -= tskb->truesize;
 spin_lock_bh( &( ninja_skbq_sent.my_spinlock));
 skb_queue_tail( &( ninja_skbq_sent.skbs), tskb);
 ninja_skbq_sent.q_len += tskb->truesize;
 spin_unlock_bh( &( ninja_skbq_sent.my_spinlock));
 //kfree_skb( tskb);	// do it later
 out_ok:
 spin_unlock_bh( &( ninja_skbq_newq.my_spinlock));
 return( ret);  }

// _tp - current thread pointer
int dv_io_rw_cch( kthread_t *_tp) {
 th_buffs_t *bt = ( th_buffs_t *)_tp->th_data;
 int gots, err;

 // allocate IN buffers if needed
 if ( bt == NULL && !( bt = dv_s_new_thdata( _tp))) return( -1);
 gots = dv_s_R( _tp, bt->b_c_in, NINJA_BUFSZ_C);
 // <0 means error - finish connection
 if ( gots < 0) KINF( "getting '%s' data E=%d\n", _tp->thname, gots);
 if ( gots < 0) return( -1);
 err = 0;
 // got some command from peer - handle it
 if ( gots > 0) err = netcmd_proc( _tp, bt->b_c_in, gots);
 if ( err != 0) return( -1);
 return( 0);  }
