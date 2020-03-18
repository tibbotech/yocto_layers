#include "twg_sysfs.h"

static ssize_t twg_sop_name_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%s\n", _p->name));  }

static ssize_t twg_sop_stat_done_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%d\n", _p->stat_done));  }
static ssize_t twg_sop_stat_over_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%d\n", _p->stat_over));  }

static ssize_t twg_sop_mode_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%d\n", _p->mode));  }

static ssize_t twg_sop_mode_W( struct device *_d, struct device_attribute *_a, const char *_b, size_t _c) {
 int x;
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 if ( kstrtoint( _b, 10, &x) < 0) return( -EIO);
 change_mode( _p, x);
 return( _c);  }

static ssize_t twg_sop_out0_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 if ( !_p->pin_oc) return( -EIO);
 return( sprintf( _b, "%d\n", gpio_get_value( _p->pin_oc)));  }

static ssize_t twg_sop_out0_W( struct device *_d, struct device_attribute *_a, const char *_b, size_t _c) {
 int x;
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 if ( !_p->pin_oc) return( -EIO);
 if ( kstrtoint( _b, 10, &x) < 0) return( -EIO);
 gpio_set_value( _p->pin_oc, x);
 return( _c);  }

static ssize_t twg_sop_dbgi_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%d\n", _p->debug));  }

static ssize_t twg_sop_dbgi_W( struct device *_d, struct device_attribute *_a, const char *_b, size_t _c) {
 int x;
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 if ( kstrtoint( _b, 10, &x) < 0) return( -EIO);
 _p->debug = x;
 return( _c);  }

static ssize_t twg_sop_conr_R( struct device *_d, struct device_attribute *_a, char *_b) {
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%d\n", _p->clear_on_read));  }

static ssize_t twg_sop_conr_W( struct device *_d, struct device_attribute *_a, const char *_b, size_t _c) {
 int x;
 twg_pdata_t *_p = ( twg_pdata_t *)_d->platform_data;
 if ( kstrtoint( _b, 10, &x) < 0) return( -EIO);
 _p->clear_on_read = x;
 return( _c);  }

//static ssize_t twg_sop_data_R( struct device *_d, struct bin_attribute *_a, char *_b) {
static ssize_t twg_sop_data_R(
 struct file *filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t off, size_t count) {
 int ret = -ENXIO;
 uint16_t x;
 twg_pdata_t *_p = NULL;
 struct device *_pdev = container_of( _k, struct device, kobj);
 if ( !_pdev) return( ret);
 if ( !( _p = ( twg_pdata_t *)_pdev->platform_data)) return( ret);
///printk( KERN_ERR "%s X0\n", _p->name);
 if ( !_p->d_done) return( ret);
 x = cpu_to_le16( _p->d.blen);
printk( KERN_ERR "%s X1:%d\n", _p->name, x);
 memcpy( _b, &( x), sizeof( x));
 memcpy( _b+sizeof(x), &( _p->d.buff), sizeof( _p->d.buff));
 if ( _p->debug > 2) printk( KERN_ERR "%s %s() ret:%d bits\n", _p->name, __FUNCTION__, x);
// memcpy( _b, &( _p->d), sizeof( _p->d));
 if ( _p->clear_on_read) clean_buff( _p);
 return( sizeof( _p->d));  }

static struct device_attribute twg_sysfs_attrsD[] = {
 __ATTR(     name,0444,twg_sop_name_R,     NULL),
 __ATTR(     mode,0664,twg_sop_mode_R,     twg_sop_mode_W),
 __ATTR(stat_done,0444,twg_sop_stat_done_R,NULL),
 __ATTR(stat_over,0444,twg_sop_stat_over_R,NULL),
 __ATTR(     out0,0664,twg_sop_out0_R,     twg_sop_out0_W),
 __ATTR(     dbgi,0664,twg_sop_dbgi_R,     twg_sop_dbgi_W),
 __ATTR(     conr,0664,twg_sop_conr_R,     twg_sop_conr_W),
};

static struct bin_attribute twg_sysfs_attrsB[] = {
 __BIN_ATTR(     data,0664,twg_sop_data_R,     NULL, TWG_MAX_BUF/8),
};

// ---------- main (exported) functions
void twg_sysfs_init( struct platform_device *_pdev) {
 int i, ret;
 twg_pdata_t *_p = ( twg_pdata_t *)_pdev->dev.platform_data;
 for ( i = 0; i < ARRAY_SIZE( twg_sysfs_attrsD); i++) {
   ret = device_create_file( &( _pdev->dev), &twg_sysfs_attrsD[i]);
   if ( ret) printk( KERN_ERR "%s createD[%d] error\n", _p->name, i);
 }
 for ( i = 0; i < ARRAY_SIZE( twg_sysfs_attrsB); i++) {
   ret = device_create_bin_file( &( _pdev->dev), &twg_sysfs_attrsB[i]);
   if ( ret) printk( KERN_ERR "%s createB[%d] error\n", _p->name, i);
 }
 return;  }

void twg_sysfs_clean( struct platform_device *_pdev) {
 int i;
 for ( i = 0; i < ARRAY_SIZE( twg_sysfs_attrsD); i++) {
   device_remove_file( &( _pdev->dev), &twg_sysfs_attrsD[i]);
 }
 for ( i = 0; i < ARRAY_SIZE( twg_sysfs_attrsB); i++) {
   device_remove_bin_file( &( _pdev->dev), &twg_sysfs_attrsB[i]);
 }
 return;  }
