//generic define
#ifndef NULL
#define NULL   0
#endif


//ethernet parameters
#define MAC_ADDR_BYTES   6		//MAC address is 6 bytes
#define MAC_MAX_MTU		2048		//2k memeory space for data
#define MAX_PKG_SIZE     1500


//Descritpor parameters
#define RX_DESC_QUEUE_NUM 2
#define TX_DESC_QUEUE_NUM 2
#define RX_DESC0_NUM 4
#define RX_DESC1_NUM 4
#define TX_DESC0_NUM 4
#define TX_DESC1_NUM 4
#define DESC_ALIGN_BYTES 8
#define OWN_BIT		 	(1<<31)	
#define ERR_CODE	 	(0xf<<26)	
#define FS_BIT		 	(1<<25)	
#define LS_BIT			(1<<24)
#define EOR_BIT		(1<<31)	
#define DESC_BUFF_SIZE  2047		//rx descriptor buffer size

#if (RX_DESC0_NUM==16)
	#define RXDESC0_ADDR_MSB 4
	#define RXDESC0_ADDR_MASK 0x0f
	#define RXDESC0_ADDR_MASK_MSB 0x1f
#elif (RX_DESC0_NUM==32)
	#define RXDESC0_ADDR_MSB 5
	#define RXDESC0_ADDR_MASK 0x1f
	#define RXDESC0_ADDR_MASK_MSB 0x3f
#elif (RX_DESC0_NUM==64)
	#define RXDESC0_ADDR_MSB 6
	#define RXDESC0_ADDR_MASK 0x3f
	#define RXDESC0_ADDR_MASK_MSB 0x7f
#elif (RX_DESC0_NUM==128)
	#define RXDESC0_ADDR_MSB 7
	#define RXDESC0_ADDR_MASK 0x7f
	#define RXDESC0_ADDR_MASK_MSB 0xff
#elif (RX_DESC0_NUM==256)
	#define RXDESC0_ADDR_MSB 8
	#define RXDESC0_ADDR_MASK 0xff
	#define RXDESC0_ADDR_MASK_MSB 0x1ff
#endif

#if (RX_DESC1_NUM==16)
	#define RXDESC1_ADDR_MSB 4
	#define RXDESC1_ADDR_MASK 0x0f
	#define RXDESC1_ADDR_MASK_MSB 0x1f
#elif (RX_DESC1_NUM==32)
	#define RXDESC1_ADDR_MSB 5
	#define RXDESC1_ADDR_MASK 0x1f
	#define RXDESC1_ADDR_MASK_MSB 0x3f
#elif (RX_DESC1_NUM==64)
	#define RXDESC1_ADDR_MSB 6
	#define RXDESC1_ADDR_MASK 0x3f
	#define RXDESC1_ADDR_MASK_MSB 0x7f
#elif (RX_DESC1_NUM==128)
	#define RXDESC1_ADDR_MSB 7
	#define RXDESC1_ADDR_MASK 0x7f
	#define RXDESC1_ADDR_MASK_MSB 0xff
#elif (RX_DESC1_NUM==256)
	#define RXDESC1_ADDR_MSB 8
	#define RXDESC1_ADDR_MASK 0xff
	#define RXDESC1_ADDR_MASK_MSB 0x1ff
#endif

//Interrupt status bits
#define MAC_INT_MC	 	(3<<30)	
#define MAC_INT_PSC	 	(1<<19)	
#define ETH0_LINK		(1<<24)
#define ETH1_LINK		(1<<25)
#define WDOG1_TMR_EXP	(1<<22)
#define WDOG0_TMR_EXP	(1<<21)
#define ETH0_RXL_DESC_FULL	(1<<7)
#define ETH0_RXH_DESC_FULL	(1<<6)
#define ETH0_RXL_DONE		(1<<5)
#define ETH0_RXH_DONE		(1<<4)
#define ETH1_RXL_DESC_FULL	(1<<7)
#define ETH1_RXH_DESC_FULL	(1<<6)
#define ETH1_RXL_DONE		(1<<5)
#define ETH1_RXH_DONE		(1<<4)
#define ETH0_TXL_DONE		(1<<3)
#define ETH0_TXH_DONE		(1<<2)
#define ETH1_TXL_DONE		(1<<3)
#define ETH1_TXH_DONE		(1<<2)

//others
#define LEN_MASK		0x000007FF

//user define function
#define HWREG_W(M, N) (ls2w_reg_base->M = N)
#define HWREG_R(M) (ls2w_reg_base->M)
#define NEXT_RX(mac, QUEUE, N)  	\
		(N = ((N)+1 == (mac)->rx_desc_num[QUEUE]) ? 0 : (N)+1)
#define NEXT_TX(mac, QUEUE, N)		\
		(N = ((N)+1 == (mac)->tx_desc_num[QUEUE]) ? 0 : (N)+1)

//l2sw descriptors
struct __attribute__ ((packed)) l2sw_desc{
	volatile u32 cmd1;
	volatile u32 cmd2;
	volatile u32 addr1;
	volatile u32 addr2;
};

struct l2sw_mng{
	struct l2sw_desc *rx_desc[RX_DESC_QUEUE_NUM];
	struct l2sw_desc *tx_desc[TX_DESC_QUEUE_NUM];

	u32	rx_proc_pos[RX_DESC_QUEUE_NUM];
	u32	rx_desc_num[RX_DESC_QUEUE_NUM];
	u32	tx_desc_num[TX_DESC_QUEUE_NUM];
	u32	temp_receiv_queue;
	u32	temp_receiv_pos;
	u32	tx_done_pos[TX_DESC_QUEUE_NUM];
	u32	tx_pos[TX_DESC_QUEUE_NUM];
	u32	tx_desc_full[TX_DESC_QUEUE_NUM];
	u32	   status;

	char 		*rx_desc_buff_start_addr;
	char 		*tx_desc_buff_start_addr;
	char   	   	mac_addr[6];
	char		cpu_port;
	char		vlan_id;
	char		vlan_memset;
	char		link_status;
};

