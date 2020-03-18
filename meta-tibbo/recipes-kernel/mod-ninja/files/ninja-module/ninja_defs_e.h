#ifndef NINJA_DEFS_E_H
#define NINJA_DEFS_E_H

// ///////////////
// gadget definitions export
// ///////////////

#define NINJA_VERSION_PROTOCOL 1

// command and data ports is the same. command is UDP, data is TCP
#define NINJA_NET_PORT 1001

// usb gadget vendor and device IDs
#define NINJA_USB_VID 0x0005
#define NINJA_USB_DID 0x0001

// command packet maximum size. it's safe to use 256*1 in any cases.
// It could issue an error for > 256 values with some USB udc's
#define NINJA_BUFSZ_C 256*4
// tested with sam9m10g45

// data bulk IN only packet maximum size.
// SKB_MAX_ALLOC it better, but not for export to Windows.
//#define NINJA_BUFSZ_D SKB_MAX_ALLOC
// Captain Obvious says it's usually
#define NINJA_BUFSZ_D 1024*64

#define NINJA_IFNAMESIZ 16

// *** Query types (usb and udp)
typedef enum NINJA_CMD {
// get version
 NINJA_CMD_VERSION	= 0x00,
// device initialization (connect data in/out), for USB only
 NINJA_CMD_INIT		= 0x01,
// confirm
 NINJA_CMD_CONFIRM	= 0x02,
// clear packets queue
 NINJA_CMD_CLEARP	= 0x03,
// show stats
 NINJA_CMD_STATS	= 0x04,
// show interfaces list and states
 NINJA_CMD_IFLST	= 0x05,
// change interfaces state command
 NINJA_CMD_IFSET	= 0x06,
// clear/set BPF rule, compiled by libpcap
 NINJA_CMD_BPFSET	= 0x07,
// test message: for any query of this type it reply with "testmessage"
 NINJA_CMD_TESTMSG	= 0xFE,
// echo: ping-pong
 NINJA_CMD_ECHO		= 0xFF,
} NINJA_CMD_T;

struct ninja_stats_T {
 uint32_t pks_inq;
 uint32_t bts_inq;
 uint32_t pks_dropped;
};
typedef struct ninja_stats_T ninja_stats_t;


typedef enum NINJA_IFT {
 // device visible by ifconfig
 NINJA_IFT_ETHER	= 0x00,
 // ttyS...
 NINJA_IFT_TTY		= 0x01,
} NINJA_IFT_T;


typedef union {
 struct {
   unsigned short type;
   unsigned int flags;
 } e;
  struct {
  unsigned int flags;
 } t;
} desc_data_t;

struct ninja_ifdesc_T {
 char name[ NINJA_IFNAMESIZ];
 NINJA_IFT_T type;
 desc_data_t desc;
// char padding[]
};
typedef struct ninja_ifdesc_T ninja_ifdesc_t;

#endif // NINJA_DEFS_E_H
