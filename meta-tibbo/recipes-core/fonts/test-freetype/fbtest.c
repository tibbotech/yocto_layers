#include <linux/fb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
 
const char *FB_NAME = "/dev/fb0";
void*   m_FrameBuffer;
struct  fb_fix_screeninfo m_FixInfo;
struct  fb_var_screeninfo m_VarInfo;
int 	m_FBFD;

int main() {

    int iFrameBufferSize;
    /* Open the framebuffer device in read write */
    m_FBFD = open(FB_NAME, O_RDWR);
    if (m_FBFD < 0) {
    	printf("Unable to open %s.\n", FB_NAME);
    	return 1;
    }
    /* Do Ioctl. Retrieve fixed screen info. */
    if (ioctl(m_FBFD, FBIOGET_FSCREENINFO, &m_FixInfo) < 0) {
    	printf("get fixed screen info failed: %s\n",
    		  strerror(errno));
    	close(m_FBFD);
    	return 1;
    }
    /* Do Ioctl. Get the variable screen info. */
    if (ioctl(m_FBFD, FBIOGET_VSCREENINFO, &m_VarInfo) < 0) {
    	printf("Unable to retrieve variable screen info: %s\n",
    		  strerror(errno));
    	close(m_FBFD);
    	return 1;
    }

    /* Calculate the size to mmap */
//    iFrameBufferSize = m_FixInfo.line_length * m_VarInfo.yres;
    iFrameBufferSize = m_FixInfo.smem_len;
    printf("Line length %d\n", m_FixInfo.line_length);
    /* Now mmap the framebuffer. */
    m_FrameBuffer = mmap(NULL, iFrameBufferSize, PROT_READ | PROT_WRITE,
    				 MAP_SHARED, m_FBFD,0);
    if (m_FrameBuffer == NULL) {
    	printf("mmap failed:\n");
    	close(m_FBFD);
    	return 1;
    }

    printf( "FBf id:%s\n", m_FixInfo.id);
    printf( "FBf smem_start:%p len:%d\n", m_FixInfo.smem_start, m_FixInfo.smem_len);
    printf( "FBf accel:%d\n", m_FixInfo.accel);
    printf( "FBf line_length:%d\n", m_FixInfo.line_length);

    printf( "FB bits_per_pixel:%d\n", m_VarInfo.bits_per_pixel);
    printf( "FB grayscale:%d\n", m_VarInfo.grayscale);
    printf( "FB %dx%d V %dx%d\n", m_VarInfo.xres, m_VarInfo.yres, m_VarInfo.xres_virtual, m_VarInfo.yres_virtual);
    printf( "FB offX:%d offY:%d\n", m_VarInfo.xoffset, m_VarInfo.yoffset);

    printf( "FB red   off:%d len:%d msb:%d\n", m_VarInfo.red.offset, m_VarInfo.red.length, m_VarInfo.red.msb_right);
    printf( "FB green off:%d len:%d msb:%d\n", m_VarInfo.green.offset, m_VarInfo.green.length, m_VarInfo.green.msb_right);
    printf( "FB blue  off:%d len:%d msb:%d\n", m_VarInfo.blue.offset, m_VarInfo.blue.length, m_VarInfo.blue.msb_right);
    printf( "FB trans off:%d len:%d msb:%d\n", m_VarInfo.transp.offset, m_VarInfo.transp.length, m_VarInfo.transp.msb_right);
    
    {
        u_int32_t *p = (int *) m_FrameBuffer;
        int x,y,t=0, poff;
        while(1) {
            for (y=0; y<m_VarInfo.yres; y++) {
                for (x=0; x<m_VarInfo.xres; x++) {
                    poff = y * m_VarInfo.xres + x;
//                    if ( x == 0) printf( "poff1(%dx%d):%d\n", x, y, poff);
//                    poff *= ( m_VarInfo.bits_per_pixel/8);
//                    if ( x == 0) printf( "poff2(%dx%d):%d\n", x, y, poff);
//                    p[poff] = 0x00100 * y + x + t;
                    // transp | red | green | blue
                    p[poff] =  0xFF<<24 | t<<16 | 0x00 << 8 | 0x00;
                }
            }
//if ( t % 200 == 0) printf( "t:%d\n", t);
            t++;
        }
    }   
    

}
