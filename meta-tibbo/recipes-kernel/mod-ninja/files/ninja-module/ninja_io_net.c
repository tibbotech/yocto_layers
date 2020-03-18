#include "ninja_io_net.h"
#include "ninja_cmd.h"
#include "ninja_hndl.h"

extern uint net_port;

int isP2P( kthread_t *_tp) {
 if ( !_tp) return( 0);
 if ( !_tp->sock) return( 0);
 if ( _tp->sock->sk->sk_type == SOCK_STREAM) return( 1);
 return( 0);  }

// call this when connected to create IN buffers
th_buffs_t *dv_s_new_thdata( kthread_t *_tp) {
 th_buffs_t *bt = NULL;
 _tp->th_data = NULL;
 KDBG( "%s() th'%s'\n", __FUNCTION__, _tp->thname);
 if ( !( bt = kmalloc( sizeof( th_buffs_t), GFP_KERNEL))) return( NULL);
 memset( bt, 0, sizeof( th_buffs_t));
 if ( !( bt->b_d_in = kmalloc( NINJA_BUFSZ_D + 1, GFP_KERNEL))) return( NULL);
 if ( !( bt->b_c_in = kmalloc( NINJA_BUFSZ_C + 1, GFP_KERNEL))) return( NULL);
 _tp->th_data = bt;
 KDBG( "%s() th'%s' /\n", __FUNCTION__, _tp->thname);
 return( bt);  }

// call this when diconnected to destroy IN buffers
void dv_s_del_thdata( kthread_t *_tp) {
 th_buffs_t *bt = _tp->th_data;
 if ( bt == NULL) return;
 if ( bt->b_d_in) kfree( bt->b_d_in);
 return;  }

int dv_s_C( kthread_t *_tp) {
 if ( !_tp) return( 0);
 KDBG( "'%s' close 0\n", _tp->thname);
 if ( _tp->sock) sock_release( _tp->sock);
 KDBG( "'%s' close 1\n", _tp->thname);
 _tp->sock = NULL;
 // mark me as ready to put thread resources
 _tp->state = DV_THS_DELIST;
 if ( _tp->peer_id) kfree( ( ( struct sockaddr_in *)( _tp->peer_id)));
 // destroy IN buffers
 dv_s_del_thdata( _tp);
 return( 0);  }

int dv_s_R( kthread_t *_tp, unsigned char *_b, int _l) {
 int ret = 0;
 struct sockaddr_in *addr = ( struct sockaddr_in *)( _tp->peer_id);
 struct iovec iov = {
   .iov_base = _b,
   .iov_len = _l,
 };
 struct msghdr msg = {
   .msg_flags = MSG_DONTWAIT,
   .msg_control = NULL,
   .msg_controllen = 0,
   .msg_iov = &iov,
   .msg_iovlen = 1,
 };
 mm_segment_t oldfs;
 if ( !_tp->sock) return( 0);
 if ( addr) {
   msg.msg_name = addr;  msg.msg_namelen = sizeof( struct sockaddr_in);
 } else {
   msg.msg_name = NULL;  msg.msg_namelen = 0;  }
 oldfs = get_fs();
 set_fs( KERNEL_DS);
 ret = sock_recvmsg( _tp->sock, &msg, _l, msg.msg_flags);
 set_fs( oldfs);
 if ( ret == -ERESTARTSYS || ret == -EAGAIN) return( 0);
 // FIX: prevent getting from myself
 return( ret);  }

int dv_s_W( kthread_t *_tp, unsigned char *_b, int _l) {
 int ret = 0;
 struct sockaddr_in *addr = ( struct sockaddr_in *)( _tp->peer_id);
 struct iovec iov = {
   .iov_base = _b,
   .iov_len = _l,
 };
 struct msghdr msg = {
   .msg_flags = MSG_DONTWAIT,
   .msg_control = NULL,
   .msg_controllen = 0,
   .msg_iov = &iov,
   .msg_iovlen = 1,
 };
 mm_segment_t oldfs;
 if ( !_tp->sock) return( 0);
 if ( addr) {
   msg.msg_name = addr;  msg.msg_namelen = sizeof( struct sockaddr_in);
 } else {
   msg.msg_name = NULL;  msg.msg_namelen = 0;  }
 oldfs = get_fs();
 set_fs( KERNEL_DS);
 ret = sock_sendmsg( _tp->sock, &msg, _l);
 set_fs( oldfs);
 if ( ret == -ERESTARTSYS || ret == -EAGAIN) return( 0);
 return( ret);  }

int dv_s_accept( kthread_t *_tp) {
 th_tasks_t task_tcpc = { .O = &dv_io, .W = &dv_io_rw_dch, .C = &dv_s_C };
 int err = 0;
 struct sock *_sk = _tp->sock->sk;
 struct socket *peer_sock = NULL;
 unsigned char thname[ NINJA_THNAME_LEN + 1];
 kthread_t *new_thread = NULL;
 err = sock_create_lite( _sk->sk_family, _sk->sk_type, _sk->sk_protocol, &peer_sock);
 if ( err < 0) {
   KERR( "'%s' create E=%d\n", _tp->thname, err);
   return( err);  }
 while ( 1) {
   msleep( 10);
   if ( kthread_should_stop()) {  err = -1;  _tp->state = DV_THS_DESTROY;  goto err_out;  }
   schedule();
   err = _tp->sock->ops->accept( _tp->sock, peer_sock, O_NONBLOCK);
   if ( err == -ERESTARTSYS || err == -EAGAIN) continue;
   if ( err < 0) {
     KERR( "%s  '%s' accept E=%d\n", __FUNCTION__, _tp->thname, err);
     _tp->state = DV_THS_RESTARTING;  // restart me
     goto err_out;  }
   // there new connection is accepted
   break;  }
 // set socket options
 peer_sock->ops = _tp->sock->ops;
 peer_sock->sk->sk_flags = SO_LINGER;
 peer_sock->sk->sk_lingertime = 3*HZ;
 // create new thread for client
 memset( thname, 0, NINJA_THNAME_LEN + 1);
 sprintf( thname, "%s/tcp-conn%d", MNAME, thread_counter);
 new_thread = dv_th_init_task( thname, task_tcpc);
 if ( new_thread == NULL) return( -1);
 KDBG( "%s '%s' out_ok\n", __FUNCTION__, _tp->thname);
 new_thread->sock = peer_sock;
// FIX: allocate peer_id ? now not needed
 dv_th_run( new_thread);
 return( 0);
 err_out:
 KDBG( "%s '%s' out_err\n", __FUNCTION__, _tp->thname);
 sock_release( peer_sock);
 return( err);  }

// _tp - current thread pointer
int dv_s_start( void *_tp) {
 int err;
 struct sockaddr_in bind_addr;
 kthread_t *tp = ( kthread_t *)_tp;

 // set socket options
 tp->sock->sk->sk_reuse = 1;	// SO_REUSEADDR
 tp->sock->sk->sk_flags = SO_LINGER;
 tp->sock->sk->sk_lingertime = 3*HZ;
 // set socket options
 memset( &bind_addr, 0, sizeof( struct sockaddr));
 bind_addr.sin_family = AF_INET;
 bind_addr.sin_addr.s_addr = htonl( INADDR_ANY);
 bind_addr.sin_port = htons( net_port);
 err = tp->sock->ops->bind( tp->sock, ( struct sockaddr *)&bind_addr, sizeof( struct sockaddr));
 if ( err < 0) {
   KERR( "bind '%s' socket, E=%d\n", tp->thname, -err);
   dv_s_C( tp);  return( 0);  }
 KDBG( "'%s' bind to port %d\n", tp->thname, NINJA_NET_PORT);
// FIX: allocate peer_id ? now not needed
 // if TCP server - listen and then accept cicle
 if (  isP2P( tp)) {
   err = tp->sock->ops->listen( tp->sock, 5);
   if ( err < 0) {
     KERR( "'%s' listen E=%d\n", tp->thname, err);
     return( 0);  }
   KDBG( "'%s' listening on port %d\n", tp->thname, NINJA_NET_PORT);
   while ( dv_s_accept( tp) == 0);
 }
 // if UDP server - just RW in the same thread till the end of RW
 if ( !isP2P( tp)) {
   // allocate peer_id in any case
   tp->peer_id = kmalloc( sizeof( struct sockaddr_in), GFP_KERNEL);
   if ( !( tp->peer_id)) {
     KERR( "alloc peer_id '%s' socket error\n", tp->thname);
     dv_s_C( tp);  return( 0);  }
   while ( dv_io( tp) == 0);
 }
 return( 0);  }

// _tp - current thread pointer
int dv_s_O_tcps( void *_tp) {
 int err;
 kthread_t *tp = ( kthread_t *)_tp;

 err = sock_create_kern( AF_INET, SOCK_STREAM, IPPROTO_TCP, &( tp->sock));
 if ( err < 0) {
   KERR( "create '%s' socket, E=%d\n", tp->thname, -ENXIO);
   tp->sock = NULL;  return( 0);  }
 return( dv_s_start( _tp));  }

// _tp - current thread pointer
int dv_s_O_udps( void *_tp) {
 int err;
 kthread_t *tp = ( kthread_t *)_tp;

 err = sock_create_kern( AF_INET, SOCK_DGRAM, IPPROTO_UDP, &( tp->sock));
 if ( err < 0) {
   KERR( "create '%s' socket, E=%d\n", tp->thname, -ENXIO);
   tp->sock = NULL;  return( 0);  }
 return( dv_s_start( _tp));  }


// ---------- main (exported) functions
void ninja_io_net_init( void) {
 kthread_t *neth;
 th_tasks_t tasks[] = {
  { .O = &dv_s_O_tcps, .W = &dv_io_rw_dch, .C = &dv_s_C },
  { .O = &dv_s_O_udps, .W = &dv_io_rw_cch, .C = &dv_s_C },
 };
 neth = dv_th_init_task( MNAME"/tcps", tasks[ 0]);
 dv_th_run( neth);
 neth = dv_th_init_task( MNAME"/udps", tasks[ 1]);
 dv_th_run( neth);
 return;  }

void ninja_io_net_clean( void) {
 KDBG( "%s\n", __FUNCTION__);
 return;  }
