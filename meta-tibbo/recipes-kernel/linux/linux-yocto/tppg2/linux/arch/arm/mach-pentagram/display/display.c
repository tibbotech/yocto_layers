/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2018 by Sunplus Inc.                             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Inc. All rights are reserved by Sunplus Inc.                          *
 *  This software may only be used in accordance with the                 *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Inc. reserves the right to modify this software               *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus Inc.                                                          *
 *  19, Innovation First Road, Hsinchu Science Park                       *
 *  Hsinchu City 30078, Taiwan, R.O.C.                                    *
 *                                                                        *
 **************************************************************************/

/**
 * @file display.c
 * @brief display driver
 * @author PoChou Chen
 */
/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>

#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "reg_disp.h"
#include "hal_disp.h"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define DISP_LOG_DEBUG		KERN_DEBUG
#define DISP_LOG_ERR		KERN_ERR

#define DISP_DEBUG

#ifdef DISP_DEBUG
	#define DEBUG(fmt, args...)		printk(DISP_LOG_DEBUG "[DISP]"fmt, ##args)
#else
	#define DEBUG(...)
#endif
#define DERROR(fmt, args...)		printk(DISP_LOG_ERR "[DISP]"fmt, ##args)

#define DISP_ALIGN(x, n)		(((x) + ((n)-1)) & ~((n)-1))

#ifdef SUPPORT_DEBUG_MON
	#define MON_PROC_NAME		"disp_mon"
	#define MON_CMD_LEN			(256)
#endif

#define FAKE_DMA	//DMA driver for test

#define DMA_WIDTH	512
#define DMA_HEIGHT	300
#define AIO_DATA_OFFSET		(512*1024 - DMA_WIDTH*DMA_HEIGHT*2)

#if 0
#define DMA_MAX_BLOCK			(1920 * 2 * 32)		//yuy2, one line 1920, 32lines
#else
static int DMA_MAX_BLOCK = DMA_WIDTH*32*2;//(DMA_WIDTH*2*32);
#endif
static int DMA_id = 0;
static int DMA_pos = 0;
static int DMA_len = DMA_WIDTH*DMA_HEIGHT*2;
static int DMA_times = 0;

#define VPP_WIDTH	720//512//242
#define VPP_HEIGHT	480//300//255
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
// platform driver functions
static int _display_probe(struct platform_device *pdev);
static int _display_remove(struct platform_device *pdev);
static int _display_suspend(struct platform_device *pdev, pm_message_t state);
static int _display_resume(struct platform_device *pdev);

#ifdef SUPPORT_DEBUG_MON
static int _open_flush_proc(struct inode *inode, struct file *file);
static ssize_t _write_flush_proc(struct file *filp, const char __user *buffer, size_t len, loff_t *f_pos);
#endif

#ifdef FAKE_DMA
static int _dma_probe(struct platform_device *pdev);
#endif

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
DISPLAY_WORKMEM gDispWorkMem = {0};

static struct platform_device_id _display_devtypes[] = {
	{ .name = "sp_display" },
	{ /* sentinel */ }
};

static struct of_device_id _display_ids[] = {
	{ .compatible = "sunplus,sp-display"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, _display_ids);

// platform driver
static struct platform_driver _display_driver = {
	.probe		= _display_probe,
	.remove		= _display_remove,
	.suspend	= _display_suspend,
	.resume		= _display_resume,
	.id_table	= _display_devtypes,
	.driver		= {
		.name	= "sp_display",
		.of_match_table	= of_match_ptr(_display_ids),
	},
};
module_platform_driver(_display_driver);

#ifdef FAKE_DMA
static struct platform_device_id _dma_devtypes[] = {
	{ .name = "sp_dma" },
	{ /* sentinel */ }
};

static struct of_device_id _dma_ids[] = {
	{ .compatible = "sunplus,sp-dma0"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, _dma_ids);

// platform driver
static struct platform_driver _dma_driver = {
	.probe		= _dma_probe,
	.remove		= _display_remove,
	.suspend	= _display_suspend,
	.resume		= _display_resume,
	.id_table	= _dma_devtypes,
	.driver		= {
		.name	= "sp_dma",
		.of_match_table	= of_match_ptr(_dma_ids),
	},
};
module_platform_driver(_dma_driver);
#endif

#ifdef SUPPORT_DEBUG_MON
static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.open = _open_flush_proc,
	.write = _write_flush_proc,
	.read = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
};

struct proc_dir_entry *entry = NULL;
#endif

/**************************************************************************
 *                     S T A T I C   F U N C T I O N                      *
 **************************************************************************/
static volatile int mac_test = 0;
static volatile UINT32 *G12;
static volatile UINT32 *aio;

int getstc(void)
{
	return ((G12[1] & 0xffff) << 16) | (G12[0] & 0xffff);
}

static void bio_wreg(UINT32 index, UINT32 value)
{
	DISPLAY_WORKMEM *pDispWorkMem = &gDispWorkMem;
	volatile UINT32 *bio = (UINT32 *)pDispWorkMem->bio;

	*(bio+index) = value;
	//DEBUG("bio[%d] = 0x%x (%d)\n", index, *(bio+index), *(bio+index));
}

volatile UINT32 aio_lreg(UINT32 index)
{
	return *(aio+index);
}

static void aio_wreg(UINT32 index, UINT32 value)
{
	//DEBUG("aio[%d](0x%x) = 0x%x (%d)\n", index, (UINT32)aio + index*4, value, value);
	*(aio+index) = value;
	//DEBUG("aio[%d](0x%x) = 0x%x (%d)\n", index, (UINT32)aio + index*4, *(aio+index), *(aio+index));
}

static irqreturn_t _display_irq_field_start(int irq, void *param)
{
	if (mac_test == 1)
		DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
	if (mac_test == 11)
	{
		bio_wreg(1, 0x80000000 | (1<<19) | (1<<17));
		aio_wreg(0, (DMA_WIDTH*DMA_HEIGHT*2/16)<<17);
		aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET);
		aio_wreg(1, 0x80000001);
		//DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
	}
	if (mac_test == 14)
	{
		//int ttt = DMA_times;
		DMA_pos = 0;
		DMA_len = DMA_WIDTH*DMA_HEIGHT*2;
		DMA_times = 0;

		if (DMA_len % 16)
		{
			//DERROR("%s:%d mac dma len must 16 align\n", __FUNCTION__, __LINE__);
			DMA_len = DISP_ALIGN(DMA_WIDTH*DMA_HEIGHT*2, 16);
		}
		bio_wreg(1, 0x80000000 | (1<<19) | (1<<17));

		//first DMA buffer
		if ((DMA_len - DMA_pos) >= DMA_MAX_BLOCK)
		{
			aio_wreg(0, (DMA_MAX_BLOCK/16)<<17);
			aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += DMA_MAX_BLOCK;
		}
		else
		{
			aio_wreg(0, ((DMA_len - DMA_pos)/16)<<17);
			aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += (DMA_len - DMA_pos);
		}
		aio_wreg(1, 0x80000001);
		DMA_id = 0;
		DMA_times += 1 << (0>>1);

		//second DMA buffer
		if ((DMA_len - DMA_pos) >= DMA_MAX_BLOCK)
		{
			aio_wreg(0+32, (DMA_MAX_BLOCK/16)<<17);
			aio_wreg(2+32, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += DMA_MAX_BLOCK;
		}
		else
		{
			aio_wreg(0+32, ((DMA_len - DMA_pos)/16)<<17);
			aio_wreg(2+32, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += (DMA_len - DMA_pos);
		}
		aio_wreg(1+32, 0x80000001);
		DMA_id = 1;
		DMA_times += 1 << (32>>1);

	}
	if (mac_test == 12 || mac_test == 13)
	{
		//int ttt = DMA_times;
		DMA_pos = 0;
		DMA_len = DMA_WIDTH*DMA_HEIGHT*2;
		DMA_times = 0;

		if (DMA_len % 16)
		{
			DERROR("%s:%d mac dma len must 16 align\n", __FUNCTION__, __LINE__);
		}
		bio_wreg(1, 0x80000000 | (1<<19) | (1<<17));

		//first DMA buffer
		if ((DMA_len - DMA_pos) >= DMA_MAX_BLOCK)
		{
			aio_wreg(0, (DMA_MAX_BLOCK/16)<<17);
			aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += DMA_MAX_BLOCK;
		}
		else
		{
			aio_wreg(0, ((DMA_len - DMA_pos)/16)<<17);
			aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += (DMA_len - DMA_pos);
		}
		aio_wreg(1, 0x80000001);
		DMA_id = 0;
		DMA_times += 1 << (0>>1);

#if 0
		//second DMA buffer
		if ((DMA_len - DMA_pos) >= DMA_MAX_BLOCK)
		{
			aio_wreg(0+32, (DMA_MAX_BLOCK/16)<<17);
			aio_wreg(2+32, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += DMA_MAX_BLOCK;
		}
		else
		{
			aio_wreg(0+32, ((DMA_len - DMA_pos)/16)<<17);
			aio_wreg(2+32, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += (DMA_len - DMA_pos);
		}
		aio_wreg(1+32, 0x80000001);
#endif
		//if (DMA_pos != DMA_len)
		//	DERROR("%s:%d mac dma error\n", __FUNCTION__, __LINE__);

		//DERROR("%s:%d mac test times:0x%x(%d) line:%d %d\n", __FUNCTION__, __LINE__, ttt, ttt, DRV_TGEN_GetLineCntNow(), getstc());
	}

	return IRQ_HANDLED;
}

static irqreturn_t _display_irq_field_end(int irq, void *param)
{
	if (mac_test == 2)
		DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
#if 0
	if (mac_test == 10)
	{
		//DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
		aio_wreg(1, 0x80000001);
		mac_test = 0;
	}
#endif
	if ((mac_test == 10) || (mac_test == 11) || (mac_test == 12) || (mac_test == 13))
	{
		//aio_wreg(32+32+12, 0x40000 | (1<<8));

		//bio_wreg(1, 0x00000000 | (1<<19) | (1<<17));

		if (mac_test == 10 || mac_test == 13)
			mac_test = 0;
		if (mac_test == 12)
			mac_test = 13;
		//DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
	}
	//if (mac_test == 14)
	//	DERROR("%s:%d mac dma times=0x%x\n", __FUNCTION__, __LINE__, DMA_times);

	aio_wreg(32+32+12, 0x40000 | (1<<8));
	bio_wreg(1, 0x00000000 | (1<<19) | (1<<17));

	return IRQ_HANDLED;
}

static irqreturn_t _display_irq_field_int1(int irq, void *param)
{
	if (mac_test == 3)
		DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
	return IRQ_HANDLED;
}

static irqreturn_t _display_irq_field_int2(int irq, void *param)
{
	if (mac_test == 4)
		DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
	if (mac_test == 10)
	{
		bio_wreg(1, 0x80000000 | (1<<19) | (1<<17));
		aio_wreg(0, (DMA_WIDTH*DMA_HEIGHT*2/16)<<17);
		aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET);
		aio_wreg(1, 0x80000001);
		//DERROR("%s:%d mac test line:%d %d\n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
	}
	return IRQ_HANDLED;
}

#ifdef SUPPORT_DEBUG_MON
typedef struct _BMP_HEADER
{
	unsigned short reserved0;
	unsigned short identifier;
	unsigned int file_size;
	unsigned int reserved1;
	unsigned int data_offset;
	unsigned int header_size;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bpp;
	unsigned int compression;
	unsigned short reserved;
} BMP_HEADER;

#if 0
static void _initKernelEnv(mm_segment_t *oldfs)
{
	*oldfs = get_fs();
	set_fs(KERNEL_DS);
}

static void _deinitKernelEnv(mm_segment_t *oldfs)
{
	set_fs(*oldfs);
}

static int _readFile(struct file *fp, char *buf, int readlen)
{
	if (fp->f_op && fp->f_op->read)
		return fp->f_op->read(fp, buf, readlen, &fp->f_pos);
	else
		return -1;
}

static int _closeFile(struct file *fp)
{
	return filp_close(fp, NULL);
}
#endif
#if 0
void _rgb_to_yuv_by_pixel(unsigned int rgb, unsigned int *y, unsigned int *u, unsigned int *v)
{
	int r, g, b, y_t, u_t, v_t;

	r = (rgb >> 16) & 0xff;
	g = (rgb >> 8) & 0xff;
	b = (rgb >> 0) & 0xff;

	y_t = ((r*66 + g*129 + b*25 + 128)>>8) + 16;
	u_t = ((-r*38 - g*74 + b*112 + 128)>>8) + 128;
	v_t = ((r*112 - g*94 - b*18 + 128)>>8) + 128;

	if (y_t < 0)
		y_t = 0;
	if (y_t > 255)
		y_t = 255;
	if (u_t < 0)
		u_t = 0;
	if (u_t > 255)
		u_t = 255;
	if (v_t < 0)
		v_t = 0;
	if (v_t > 255)
		v_t = 255;

	*y = y_t & 0xff;
	*u = u_t & 0xff;
	*v = v_t & 0xff;
}

void _rgb_to_yuv420_NV12(unsigned int *rgb, unsigned char *luma_addr, unsigned char *chroma_addr, unsigned int width, unsigned int height)
{
	int i, j;
	unsigned int y, u, v;

	for (j = 0; j < height; ++j)
	{
		for (i = 0; i < width; ++i)
		{
			_rgb_to_yuv_by_pixel(rgb[width * j + i], &y, &u, &v);

			luma_addr[j * width + i] = y;

			if (!(j & 0x1) && !(i & 0x1))
			{
				chroma_addr[(width * (j >> 1)) + ((i >> 1) << 1)] = 0;
				chroma_addr[(width * (j >> 1)) + ((i >> 1) << 1) + 1] = 0;
			}
			chroma_addr[(width * (j >> 1)) + ((i >> 1) << 1)] += u / 4;
			chroma_addr[(width * (j >> 1)) + ((i >> 1) << 1) + 1] += v / 4;
		}
	}
}
#endif

#if 0
static void _Load_Bmpfile(char *filename, int *rgb_ptr, int *w, int *h, int *rgb_linepitch)
{
	char			*tmpbuf = NULL;
	int				*tmpbuf2 = NULL;
	struct file		*fp = NULL;
	mm_segment_t	oldfs;
	BMP_HEADER		bmp = {0};
	int				pixel_len = 32 >> 3;

	_initKernelEnv(&oldfs);
	fp = filp_open(filename, O_RDONLY, 0);

	if (!IS_ERR(fp))
	{
		int line_pitch = 0, bpp = 0;
		int x, y;
		int line_index;
		unsigned int color;

		if (_readFile(fp, (char *)&bmp.identifier, sizeof(BMP_HEADER) - sizeof(unsigned short)) <= 0)
		{
			DERROR("read file:%s error\n", filename);
			goto Failed;
		}

		DEBUG("Image info: size %dx%d, bpp %d, compression %d, offset %d, filesize %d\n", bmp.width, bmp.height, bmp.bpp, bmp.compression, bmp.data_offset, bmp.file_size);

		if (!(bmp.bpp == 24 || bmp.bpp == 32))
		{
			DERROR("only support 24 / 32 bit bitmap\n");
			goto Failed;
		}

		if (bmp.bpp == 24)
			line_pitch = DISP_ALIGN(bmp.width, 2);
		else
			line_pitch = bmp.width;

		if (bmp.width > 1024)
		{
			DERROR("bmp_width >= device width, %d >= %d\n", bmp.width, 1024);
			goto Failed;
		}

		if (bmp.height > 600)
		{
			DERROR("bmp_height >= device height, %d >= %d\n", bmp.height, 600);
			goto Failed;
		}

		*w = bmp.width;
		*h = bmp.height;
		*rgb_linepitch = DISP_ALIGN(line_pitch, 128);
		bpp = bmp.bpp >> 3;

		//tmpbuf = sp_chunk_malloc_nocache(0, 0, line_pitch * bmp.height * bpp);
		if (IS_ERR(tmpbuf))
		{
			DERROR("chunk_malloc error\n");
			goto Failed;
		}

		//tmpbuf2 = sp_chunk_malloc_nocache(0, 0, *rgb_linepitch * bmp.height * pixel_len);
		if (IS_ERR(tmpbuf2))
		{
			DERROR("chunk_malloc error\n");
			goto Failed;
		}

		*rgb_ptr = tmpbuf2;
		fp->f_op->llseek(fp, bmp.data_offset, 0);
		if (_readFile(fp, tmpbuf, line_pitch * bmp.height * bpp) <= 0)
		{
			DERROR("read file:%s error\n", filename);
			goto Failed;
		}

		for (y = bmp.height - 1; y >= 0; y--)
		{
			for (x = 0; x < bmp.width; x++)
			{
				line_index = (line_pitch * y + x) * bpp;

				color = ((bmp.bpp == 24)?0xff:tmpbuf[line_index + 3]) << 24;
				color |= (tmpbuf[line_index + 2]) << 16;
				color |= (tmpbuf[line_index + 1]) << 8;
				color |= (tmpbuf[line_index]);

#if 0
				DEBUG("B:%x G:%x R:%x A:%x",
					tmpbuf[line_index],
					tmpbuf[line_index + 1],
					tmpbuf[line_index + 2],
					(bmp.bpp == 24)?0xff:tmpbuf[line_index + 3]);
#endif
				*(tmpbuf2++) = color;
			}
			tmpbuf2 += (*rgb_linepitch - bmp.width);
		}
	}
	else
	{
		DERROR("Can't open %s\n", filename);
		goto Failed;
	}

Failed:
	if (!IS_ERR(fp))
		_closeFile(fp);

	if (!IS_ERR(tmpbuf) && tmpbuf != NULL)
		;//sp_chunk_free(tmpbuf);

	_deinitKernelEnv(&oldfs);

	return ;
}
#endif

static int _show_flush_proc(struct seq_file *m, void *v)
{
	seq_printf(m, "mac read proc test\n");

	return 0;
}

static int _open_flush_proc(struct inode *inode, struct file *file)
{
	return single_open(file, _show_flush_proc, NULL);
}

static char* _mon_skipspace(char *p)
{
	if (p == NULL)
		return NULL;
	while (1)
	{
		int c = p[0];
		if (c == ' ' || c == '\t' || c == '\v')
			p++;
		else
			break;
	}
	return p;
}

static char* _mon_readint(char *p, int *x)
{
	int base = 10;
	int cnt, retval;

	if (x == NULL)
		return p;

	*x = 0;

	if (p == NULL)
		return NULL;

	p = _mon_skipspace(p);

	if (p[0] == '0' && p[1] == 'x')
	{
		base = 16;
		p += 2;
	}
	if (p[0] == '0' && p[1] == 'o')
	{
		base = 8;
		p += 2;
	}
	if (p[0] == '0' && p[1] == 'b')
	{
		base = 2;
		p += 2;
	}

	retval = 0;
	for (cnt = 0; 1; cnt++)
	{
		int c = *p++;
		int val;

		// translate 0~9, a~z, A~Z
		if (c >= '0' && c <= '9')
			val = c - '0';
		else if (c >= 'a' && c <= 'z')
			val = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			val = c - 'A' + 10;
		else
			break;
		if (val >= base)
			break;

		retval = retval * base + val;
	}

	if (cnt == 0)
		return p;			// no translation is done??

	*(unsigned int *) x = retval;		// store it
	return p;
}

//static int gluma_addr = 0;
//static int gchroma_addr = 0;

static void _debug_cmd(char *tmpbuf)
{
	tmpbuf = _mon_skipspace(tmpbuf);

	if (!strncasecmp(tmpbuf, "bmp", 3))
	{
#if 0
		int w, h, line_pitch;
		int rgb_ptr;
		unsigned char *luma_addr, *chroma_addr;

		tmpbuf = _mon_skipspace(tmpbuf + 3);

		_Load_Bmpfile(tmpbuf, &rgb_ptr, &w, &h, &line_pitch);

		luma_addr = sp_chunk_malloc_nocache(0, 0, line_pitch * h);
		chroma_addr = sp_chunk_malloc_nocache(0, 0, line_pitch * h / 2);

		_rgb_to_yuv420_NV12((unsigned int *)rgb_ptr, luma_addr, chroma_addr, line_pitch, h);

		sp_chunk_free((void *)rgb_ptr);

		ddfch_setting(sp_chunk_pa(luma_addr), sp_chunk_pa(chroma_addr), w, h);
		vpost_setting(w, h, (int)1024, (int)600);

		if (gluma_addr != 0)
			sp_chunk_free((void *)gluma_addr);
		if (gchroma_addr != 0)
			sp_chunk_free((void *)gchroma_addr);

		gluma_addr = (int)luma_addr;
		gchroma_addr = (int)chroma_addr;
#endif
	}
	else if (!strncasecmp(tmpbuf, "pirq", 4))
	{
		tmpbuf = _mon_skipspace(tmpbuf + 4);

		tmpbuf = _mon_readint(tmpbuf, (int *)&mac_test);

		if (mac_test)
			DERROR("enable intr debug\n");
		else
			DERROR("disable intr debug\n");
	}
	else if (!strncasecmp(tmpbuf, "mac", 3))
	{
		extern void vpost_dma(void);
	DERROR("dma debug\n");

		{
			bio_wreg(1, 0x80000000 | (1<<19) | (1<<17));
			aio_wreg(0, (DMA_WIDTH*DMA_HEIGHT*2/16)<<17);
			aio_wreg(2, 0x9ea00000 + AIO_DATA_OFFSET);
			//aio_wreg(1, 0x00000001);
			//aio_wreg(1, 0x80000001);

			//vpost_dma();
		}
	DERROR("dma debug finish\n");
	}
	else if (!strncasecmp(tmpbuf, "block", 5))
	{
		tmpbuf = _mon_readint(tmpbuf + 5, (int *)&DMA_MAX_BLOCK);
		DERROR("dma block %d\n", DMA_MAX_BLOCK);
	}
	else if (!strncasecmp(tmpbuf, "dma", 3))
	{
		//mac_test = 10;
		tmpbuf = _mon_readint(tmpbuf + 3, (int *)&mac_test);
	}
	else if (!strncasecmp(tmpbuf, "fc", 2))
	{
		int mode = 0;
		UINT32 ret;
		DRV_SetTGEN_t SetTGEN;

		tmpbuf = _mon_readint(tmpbuf + 2, (int *)&mode);

		DRV_DVE_SetMode(mode);

		switch (mode)
		{
			default:
			case 0:
				SetTGEN.fmt = DRV_FMT_480P;
				SetTGEN.fps = DRV_FrameRate_5994Hz;
				break;
			case 1:
				SetTGEN.fmt = DRV_FMT_576P;
				SetTGEN.fps = DRV_FrameRate_50Hz;
				break;
			case 2:
				SetTGEN.fmt = DRV_FMT_720P;
				SetTGEN.fps = DRV_FrameRate_5994Hz;
				break;
			case 3:
				SetTGEN.fmt = DRV_FMT_720P;
				SetTGEN.fps = DRV_FrameRate_50Hz;
				break;
			case 4:
				SetTGEN.fmt = DRV_FMT_1080P;
				SetTGEN.fps = DRV_FrameRate_5994Hz;
				break;
			case 5:
				SetTGEN.fmt = DRV_FMT_1080P;
				SetTGEN.fps = DRV_FrameRate_50Hz;
				break;
			case 6:
				SetTGEN.fmt = DRV_FMT_1080P;
				SetTGEN.fps = DRV_FrameRate_24Hz;
				break;
			case 7:
				diag_printf("user mode unsupport\n");
				break;
		}

		ret = DRV_TGEN_Set(&SetTGEN);
		if (ret != DRV_SUCCESS)
			diag_printf("TGEN Set failed, ret = %d\n", ret);
	}
	else if (!strncasecmp(tmpbuf, "osd", 3))
	{
		tmpbuf = _mon_skipspace(tmpbuf + 3);

		if (!strncasecmp(tmpbuf, "info", 4))
			DRV_OSD_Info();
		else if (!strncasecmp(tmpbuf, "hdrs", 4))
			DRV_OSD_HDR_Show();
		else if (!strncasecmp(tmpbuf, "hdrw", 4))
		{
			int offset, value;
			tmpbuf = _mon_readint(tmpbuf + 4, (int *)&offset);
			tmpbuf = _mon_readint(tmpbuf, (int *)&value);
			DRV_OSD_HDR_Write(offset, value);
		}
	}
#if 0
	else if (!strncasecmp(tmpbuf, "test", 4))
	{
		unsigned char *ptr;
		int w, h;

#if 0
		w = VPP_SRC_W;
		h = VPP_SRC_H;
#else
		w = 1024;
		h = 256;
#endif
		ptr = sp_chunk_malloc_nocache(0, 0, w * h * 2);

		linec0 = _lreg(203, 13) & 0x7ff;
		stc0 =	(_lreg(12, 1) << 16) | _lreg(12, 0);

		bchip_dma(5, sp_chunk_pa((void *)ptr), 0, 0, w * h * 2);
		
		achip_dma(5, 0x9E800000, w * h * 2);

		sp_chunk_free((void *)ptr);
	}
	else if (!strncasecmp(tmpbuf, "dma", 3))
	{
		extern void update_display(void);
		int count = 0;
		int a, b;

		update_display();

		while(1)
		{
			b = bchip_dma(5, 0, 1, 0, VPP_SRC_W * VPP_SRC_H *2);

			DERROR("turn on VPOST DMA\n");


			if (b > 0)
				a = achip_dma(5, 0x9E800000, VPP_SRC_W * VPP_SRC_H *2);
			else
				continue;
			count++;
			if (count > 900)
				break;
		}
	}
	else if (!strncasecmp(tmpbuf, "start", 5))
	{
		extern void update_display(void);
		int start = 0;
		tmpbuf = _mon_readint(tmpbuf + 5, &start);

		update_display();

		if (start)
			dma_start = 1;
		else
			dma_start = 0;
		
		DERROR("turn on interrupt DMA\n");
	}
	else if (!strncasecmp(tmpbuf, "format", 6))
	{
		extern void display_format(int pal);
		int format = 0;

		tmpbuf = _mon_readint(tmpbuf + 6, &format);
		if (format)
			display_format(1);
		else
			display_format(0);
	}
#endif
	else
		DERROR("unknown command:%s\n", tmpbuf);

	(void)tmpbuf;
}

static ssize_t _write_flush_proc(struct file *filp, const char __user *buffer, size_t len, loff_t *f_pos)
{
	char pbuf[MON_CMD_LEN];
	char *tmpbuf = pbuf;

	if (len > sizeof(pbuf))
	{
		DERROR("intput error len:%d!\n", (int)len);
		return -ENOSPC;
	}

	if (copy_from_user(tmpbuf, buffer, len))
	{
		DERROR("intput error!\n");
		return -EFAULT;
	}

	if (len == 0)
		tmpbuf[len] = '\0';
	else
		tmpbuf[len - 1] = '\0';

	if (!strncasecmp(tmpbuf, "dispmon", 7))
	{
		_debug_cmd(tmpbuf + 7);
	}

	return len;
}

#endif

#ifdef FAKE_DMA
static irqreturn_t _dma_irq_jobdown0(int irq, void *param)
{
#if 1
	volatile UINT32 dma_buf0_st, dma_buf1_st;
	int dma_offset = 1;

	if (!((mac_test == 12) || (mac_test == 14)))
		return IRQ_HANDLED;
	if (DMA_len != DMA_pos)
	{
		dma_buf0_st = aio_lreg(1) & (1<<31);
		dma_buf1_st = aio_lreg(1+32) & (1<<31);

		if ((DMA_len - DMA_pos) % 16)
			DERROR("%s:%d mac dma len must 16 align\n", __FUNCTION__, __LINE__);

		//if (dma_buf0_st && dma_buf1_st)
		//	DERROR("%s:%d mac dma ping-pong buffer can't run\n", __FUNCTION__, __LINE__);
#if 0
		else if (dma_buf0_st == 0)
			dma_offset = 0;
		else
			dma_offset = 32;
#else
		if (DMA_id == 0)
		{
			dma_offset = 32;
			DMA_id = 1;
		}
		else
		{
			dma_offset = 0;
			DMA_id = 0;
		}
#endif

		DMA_times += 1 << (dma_offset>>1);

		if ((DMA_len - DMA_pos) >= DMA_MAX_BLOCK)
		{
			aio_wreg(0 + dma_offset, (DMA_MAX_BLOCK/16)<<17);
			aio_wreg(2 + dma_offset, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += DMA_MAX_BLOCK;
		}
#if 1
		else
		{
			aio_wreg(0 + dma_offset, ((DMA_len - DMA_pos)/16)<<17);
			aio_wreg(2 + dma_offset, 0x9ea00000 + AIO_DATA_OFFSET + DMA_pos);
			DMA_pos += (DMA_len - DMA_pos);
		}
#endif
		aio_wreg(1 + dma_offset, 0x80000001);
	}
	//DERROR("%s:%d mac dma offset %d, test line:%d %d \n", __FUNCTION__, __LINE__, dma_offset, DRV_TGEN_GetLineCntNow(), getstc());
#else
	DERROR("%s:%d mac test line:%d %d \n", __FUNCTION__, __LINE__, DRV_TGEN_GetLineCntNow(), getstc());
#endif

	return IRQ_HANDLED;
}

static int _dma_init_irq(struct platform_device *pdev)
{
	int irq;
	int num_irq = of_irq_count(pdev->dev.of_node);
	UINT32 ret;

	DERROR("[%s:%d] irq num:%d\n", __FUNCTION__, __LINE__, num_irq);

	irq = platform_get_irq(pdev, 0);
	if (irq != -ENXIO) {
		ret = devm_request_irq(&pdev->dev, irq, _dma_irq_jobdown0, IRQF_TRIGGER_RISING, "sp_dma_jobdown0", pdev);
		if (ret) {
			DERROR("request_irq error: %d, irq_num: %d\n", ret, irq);
			goto ERROR;
		}
	} else {
		DERROR("platform_get_irq error\n");
		goto ERROR;
	}

	return 0;
ERROR:
	return -1;
}

static int _dma_probe(struct platform_device *pdev)
{
	const struct of_device_id *match;

	DERROR("[%s:%d] in\n", __FUNCTION__, __LINE__);
	if (pdev->dev.of_node) {
		match = of_match_node(_dma_ids, pdev->dev.of_node);
		if (match == NULL) {
			DERROR("Error: %s, %d\n", __func__, __LINE__);
			return -ENODEV;
		}
	}

	if (_dma_init_irq(pdev)) {
		DERROR("Error: %s, %d\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}
#endif

#if 0
char *_gdisp_irq_name[] = {
	"sp_disp_irq_fs",
	"sp_disp_irq_fe",
	"sp_disp_irq_int1",
	"sp_disp_irq_int2",
};
#endif

static int _display_init_irq(struct platform_device *pdev)
{
	int irq;
	int num_irq = of_irq_count(pdev->dev.of_node);
	UINT32 ret;

	DERROR("irq num:%d\n", num_irq);

	irq = platform_get_irq(pdev, 0);
	if (irq != -ENXIO) {
		ret = devm_request_irq(&pdev->dev, irq, _display_irq_field_start, IRQF_TRIGGER_RISING, "sp_disp_irq_fs", pdev);
		if (ret) {
			DERROR("request_irq error: %d, irq_num: %d\n", ret, irq);
			goto ERROR;
		}
	} else {
		DERROR("platform_get_irq error\n");
		goto ERROR;
	}

	irq = platform_get_irq(pdev, 1);
	if (irq != -ENXIO) {
		ret = devm_request_irq(&pdev->dev, irq, _display_irq_field_end, IRQF_TRIGGER_RISING, "sp_disp_irq_fe", pdev);
		if (ret) {
			DERROR("request_irq error: %d, irq_num: %d\n", ret, irq);
			goto ERROR;
		}
	} else {
		DERROR("platform_get_irq error\n");
		goto ERROR;
	}

	irq = platform_get_irq(pdev, 2);
	if (irq != -ENXIO) {
		ret = devm_request_irq(&pdev->dev, irq, _display_irq_field_int1, IRQF_TRIGGER_RISING, "sp_disp_irq_int1", pdev);
		if (ret) {
			DERROR("request_irq error: %d, irq_num: %d\n", ret, irq);
			goto ERROR;
		}
	} else {
		DERROR("platform_get_irq error\n");
		goto ERROR;
	}

	irq = platform_get_irq(pdev, 3);
	if (irq != -ENXIO) {
		ret = devm_request_irq(&pdev->dev, irq, _display_irq_field_int2, IRQF_TRIGGER_RISING, "sp_disp_irq_int2", pdev);
		if (ret) {
			DERROR("request_irq error: %d, irq_num: %d\n", ret, irq);
			goto ERROR;
		}
	} else {
		DERROR("platform_get_irq error\n");
		goto ERROR;
	}

	return 0;
ERROR:
	return -1;
}
char yuy2_array[768*480*2] __attribute__((aligned(1024)))= {
//	#include "vpp_pattern/YUY2_720x480.h"
};
UINT32 rgb_array[768*480] __attribute__((aligned(1024)));
char yuv444_array[768*480*3] __attribute__((aligned(1024)));
char yuv422_array[768*480*2] __attribute__((aligned(1024))) = {
	//#include "vpp_pattern/yuv422_NV16_720x480.h"
};
char yuv420_array[1024*600*3/2] __attribute__((aligned(1024))) = {
	#include "vpp_pattern/yuv420_NV12_720x480.h"
//	#include "vpp_pattern/vpp_test_512x300_420.h"
//	#include "vpp_pattern/vpp_test_1024x600_420.h"
};

//#include "vpp_pattern/disp_patgen.in"

static int _display_probe(struct platform_device *pdev)
{
	DISPLAY_WORKMEM *pDispWorkMem = &gDispWorkMem;
	DISP_REG_t *pTmpRegBase = NULL;
	struct resource *res_mem;
	const struct of_device_id *match;

	DEBUG("[%s:%d] in 2\n", __FUNCTION__, __LINE__);
	if (pdev->dev.of_node) {
		match = of_match_node(_display_ids, pdev->dev.of_node);
		if (match == NULL) {
			DERROR("Error: %s, %d\n", __func__, __LINE__);
			return -ENODEV;
		}
	}

	if (_display_init_irq(pdev)) {
		DERROR("Error: %s, %d\n", __func__, __LINE__);
		return -1;
	}

	{
		G12 = ioremap(0x9c000600, 32*4);
		aio = ioremap(0x9ec00580, 3*32*4);
	}
	
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (IS_ERR(res_mem)) {
		DERROR("Error: %s, %d\n", __func__, __LINE__);
		return PTR_ERR(res_mem);
	}

	//Initial HW module
	pTmpRegBase = devm_ioremap_resource(&pdev->dev, res_mem);
	if (IS_ERR((const void *)pTmpRegBase)) {
		DERROR("regbase error\n");
		return PTR_ERR(pTmpRegBase);
	}

	{
		res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		if (IS_ERR(res_mem)) {
			DERROR("Error: %s, %d\n", __func__, __LINE__);
			return PTR_ERR(res_mem);
		}

		//Initial HW module
		pDispWorkMem->bio = devm_ioremap_resource(&pdev->dev, res_mem);
		if (IS_ERR((const void *)pDispWorkMem->bio)) {
			DERROR("regbase error\n");
			return PTR_ERR(pDispWorkMem->bio);
		}
	}
	pDispWorkMem->pHWRegBase = pTmpRegBase;

	//HDMI init
	{
		//extern void hdmi_tx_init(void);
		//hdmi_tx_init();
	}
	//DMIX must first init
	DRV_DMIX_Init((void *)&pTmpRegBase->dmix);
	DRV_TGEN_Init((void *)&pTmpRegBase->tgen);
	DRV_DVE_Init((void *)&pTmpRegBase->dve);
	DRV_VPP_Init((void *)&pTmpRegBase->vpost, (void *)&pTmpRegBase->ddfch);
	DRV_OSD_Init((void *)&pTmpRegBase->osd, (void *)&pTmpRegBase->gpost);

	{
		int mode = 0;
		// TGEN setting
		UINT32 ret;
		DRV_SetTGEN_t SetTGEN;

		DRV_DVE_SetMode(mode);
		//DRV_DVE_SetColorbar(ENABLE);

		switch (mode)
		{
			default:
			case 0:
				SetTGEN.fmt = DRV_FMT_480P;
				SetTGEN.fps = DRV_FrameRate_5994Hz;
				break;
			case 1:
				SetTGEN.fmt = DRV_FMT_576P;
				SetTGEN.fps = DRV_FrameRate_50Hz;
				break;
			case 2:
				SetTGEN.fmt = DRV_FMT_720P;
				SetTGEN.fps = DRV_FrameRate_5994Hz;
				break;
			case 3:
				SetTGEN.fmt = DRV_FMT_720P;
				SetTGEN.fps = DRV_FrameRate_50Hz;
				break;
			case 4:
				SetTGEN.fmt = DRV_FMT_1080P;
				SetTGEN.fps = DRV_FrameRate_5994Hz;
				break;
			case 5:
				SetTGEN.fmt = DRV_FMT_1080P;
				SetTGEN.fps = DRV_FrameRate_50Hz;
				break;
			case 6:
				SetTGEN.fmt = DRV_FMT_1080P;
				SetTGEN.fps = DRV_FrameRate_24Hz;
				break;
			case 7:
				diag_printf("user mode unsupport\n");
				break;
		}


		ret = DRV_TGEN_Set(&SetTGEN);
		if (ret != DRV_SUCCESS)
			diag_printf("TGEN Set failed, ret = %d\n", ret);
	}

	// DMIX setting
	/****************************************
	* BG: PTG
	* L1: VPP0
	* L6: OSD0
	*****************************************/
	DRV_DMIX_Layer_Init(DRV_DMIX_BG, DRV_DMIX_AlphaBlend, DRV_DMIX_PTG);
	DRV_DMIX_Layer_Init(DRV_DMIX_L1, DRV_DMIX_AlphaBlend, DRV_DMIX_VPP0);
	DRV_DMIX_Layer_Init(DRV_DMIX_L6, DRV_DMIX_Transparent, DRV_DMIX_OSD0);

#if 0
	{
		void __iomem *achipwm = ioremap_nocache(0x9EA00000 + AIO_DATA_OFFSET, 512*1024 - AIO_DATA_OFFSET);
		diag_printf("gen rgb\n");
		_gen_checkerboard(rgb_array, DMA_WIDTH, DMA_HEIGHT, ALIGN(DMA_WIDTH, 128));
		diag_printf("gen yuv444\n");
		_rgb_to_yuv444_planar(rgb_array, yuv444_array, DMA_WIDTH, DMA_HEIGHT, ALIGN(DMA_WIDTH, 128));
		diag_printf("gen yuv422\n");
		_yuv444_to_yuv422_NV16(yuv444_array, yuv422_array, DMA_WIDTH, DMA_HEIGHT, ALIGN(DMA_WIDTH, 128));
		diag_printf("gen yuv422 packed\n");
		_yuv422_NV16_to_packed(yuv422_array, yuy2_array, DMA_WIDTH, DMA_HEIGHT, DMA_WIDTH);
		diag_printf("copy yuy2 to achip workmemory\n");
		memcpy(achipwm, yuy2_array, DMA_WIDTH * DMA_HEIGHT * 2);
		diag_printf("copy yuy2 to achip workmemory finish\n");

	}
#endif

	{
	extern void DRV_OSD_InitTest(void);
	extern void vpost_dma(void);

	DRV_OSD_InitTest();

	vpost_setting(0, 0, VPP_WIDTH, VPP_HEIGHT, 720, 480);
	//vpost_dma();

	ddfch_setting(virt_to_phys(yuv420_array), virt_to_phys((yuv420_array + ALIGN(VPP_WIDTH, 128)*VPP_HEIGHT)), VPP_WIDTH, VPP_HEIGHT, 0);
	}
	return 0;
}

static int _display_remove(struct platform_device *pdev)
{
	DEBUG("[%s:%d]\n", __FUNCTION__, __LINE__);

	return 0;
}

static int _display_suspend(struct platform_device *pdev, pm_message_t state)
{
	DEBUG("[%s:%d]\n", __FUNCTION__, __LINE__);

	return 0;
}

static int _display_resume(struct platform_device *pdev)
{
	DEBUG("[%s:%d]\n", __FUNCTION__, __LINE__);

	return 0;
}

static int __init _display_init(void)
{
	DEBUG("[%s:%d] in %s testtest\n", __FUNCTION__, __LINE__, "32");

#ifdef SUPPORT_DEBUG_MON
	entry = proc_create(MON_PROC_NAME, S_IRUGO, NULL, &proc_fops);
#endif

	return 0;
}

static void __exit _display_exit(void)
{
	DEBUG("[%s:%d]\n", __FUNCTION__, __LINE__);

#ifdef SUPPORT_DEBUG_MON
	if (entry)
		remove_proc_entry(MON_PROC_NAME, NULL);
#endif

	return;
}

/**************************************************************************
 *                     P U B L I C   F U N C T I O N                      *
 **************************************************************************/

module_init(_display_init);
module_exit(_display_exit);
MODULE_LICENSE("GPL");

