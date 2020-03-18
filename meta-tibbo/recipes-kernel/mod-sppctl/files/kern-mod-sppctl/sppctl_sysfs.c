/*
 * Driver for SunPlus/Tibbo SC7021 additional pin multiplexing controller
 *
 * Copyright (C) 2019 Dvorkin Dmitry <dvorkin@tibbo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "sppctl_sysfs.h"

static ssize_t sppctl_sop_name_R( struct device *_d, struct device_attribute *_a, char *_b) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%s\n", _p->name));  }

static ssize_t sppctl_sop_dbgi_R( struct device *_d, struct device_attribute *_a, char *_b) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%d\n", _p->debug));  }

static ssize_t sppctl_sop_dbgi_W( struct device *_d, struct device_attribute *_a, const char *_b, size_t _c) {
 int x;
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_d->platform_data;
 if ( kstrtoint( _b, 10, &x) < 0) return( -EIO);
 _p->debug = x;
 return( _c);  }

static ssize_t sppctl_sop_fwname_R( struct device *_d, struct device_attribute *_a, char *_b) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_d->platform_data;
 return( sprintf( _b, "%s", _p->fwname));  }

static ssize_t sppctl_sop_fwname_W( struct device *_d, struct device_attribute *_a, const char *_b, size_t _c) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_d->platform_data;
 strcpy( _p->fwname, _b);
 sppctl_loadfw( _d, _p->fwname);
 return( _c);  }

static ssize_t sppctl_sop_list_funcs_R(
 struct file *filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t off, size_t count) {
 int i = -1, ret = 0, pos = off;
 char *tmpp;
 sppctl_pdata_t *_p = NULL;
 struct device *_pdev = container_of( _k, struct device, kobj);
 if ( !_pdev) return( -ENXIO);
 if ( !( _p = ( sppctl_pdata_t *)_pdev->platform_data)) return( -ENXIO);
 while ( ( tmpp = list_funcs[ ++i])) {
   if ( pos > 0) {  pos -= ( strlen( tmpp) + 1);  continue;  }
   sprintf( _b + ret, "%s\n", tmpp);
   ret += strlen( tmpp) + 1;
   if ( ret > SPPCTL_MAX_BUF - SPPCTL_MAX_NAM) break;
 }
 return( ret);  }

static ssize_t sppctl_sop_txt_map_R(
 struct file *filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t off, size_t count) {
 int i = -1, ret = 0, pos = off;
 char *tmpp, tmps[ SPPCTL_MAX_NAM + 3];
 uint8_t pin = 0;
 sppctl_pdata_t *_p = NULL;
 struct device *_pdev = container_of( _k, struct device, kobj);
 if ( !_pdev) return( -ENXIO);
 if ( !( _p = ( sppctl_pdata_t *)_pdev->platform_data)) return( -ENXIO);
 while ( ( tmpp = list_funcs[ ++i])) {
   pin = sppctl_fun_get( _p, i);
   memset( tmps, 0, SPPCTL_MAX_NAM + 3);
   sprintf( tmps, "%03d %s", pin, tmpp);
   if ( pos > 0) {  pos -= ( strlen( tmps) + 1);  continue;  }
   sprintf( _b + ret, "%s\n", tmps);
   ret += strlen( tmps) + 1;
   if ( ret > SPPCTL_MAX_BUF - SPPCTL_MAX_NAM) break;
 }
 return( ret);  }

static ssize_t sppctl_sop_func_R(
 struct file *_filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t _off, size_t _count) {
 struct device *_pdev = NULL;
 sppctl_sdata_t *sdp = NULL;
 sppctl_pdata_t *_p = NULL;
 if ( _off > 0) return( 0);
 if ( !( _pdev = container_of( _k, struct device, kobj))) return( -ENXIO);
 if ( !( _p = ( sppctl_pdata_t *)_pdev->platform_data)) return( -ENXIO);
 sdp = ( sppctl_sdata_t *)_a->private;
 if ( !( sdp = ( sppctl_sdata_t *)_a->private)) return( -ENXIO);
 _b[ 0] = sppctl_fun_get( _p, sdp->i);
 _b[ 1] = 0x00;
 if ( _p->debug) KINF( "%s(%s,i:%d) _b:%d\n", __FUNCTION__, _a->attr.name, sdp->i, _b[ 0]);
 return( 1);  }

static ssize_t sppctl_sop_func_W(
 struct file *_filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t _off, size_t _count) {
 struct device *_pdev = NULL;
 sppctl_sdata_t *sdp = NULL;
 sppctl_pdata_t *_p = NULL;
 if ( _off > 0) return( 0);
 if ( !( _pdev = container_of( _k, struct device, kobj))) return( -ENXIO);
 if ( !( _p = ( sppctl_pdata_t *)_pdev->platform_data)) return( -ENXIO);
 sdp = ( sppctl_sdata_t *)_a->private;
 if ( !( sdp = ( sppctl_sdata_t *)_a->private)) return( -ENXIO);
 sppctl_pin_set( _p, _b[ 0], sdp->i);
 if ( _p->debug) KINF( "%s(%s,i:%d) _b:%d\n", __FUNCTION__, _a->attr.name, sdp->i, _b[ 0]);
 return( _count);  }

static ssize_t sppctl_sop_fw_R(
 struct file *filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t _off, size_t _count) {
 int i = -1, ret = 0, pos = _off;
 char *tmpp;
 uint8_t pin = 0;
 sppctl_pdata_t *_p = NULL;
 struct device *_pdev = container_of( _k, struct device, kobj);
 if ( !_pdev) return( -ENXIO);
 if ( !( _p = ( sppctl_pdata_t *)_pdev->platform_data)) return( -ENXIO);
 while ( ( tmpp = list_funcs[ ++i])) {
   pin = sppctl_fun_get( _p, i);
   if ( pos > 0) {  pos -= sizeof( pin);  continue;  }
   _b[ ret] = pin;
   ret += sizeof( pin);
   if ( ret > SPPCTL_MAX_BUF - SPPCTL_MAX_NAM) break;
 }
 return( ret);  }

static ssize_t sppctl_sop_fw_W(
 struct file *filp, struct kobject *_k, struct bin_attribute *_a,
 char *_b, loff_t _off, size_t _count) {
 int i = _off - 1;
 sppctl_sdata_t *sdp = NULL;
 sppctl_pdata_t *_p = NULL;
 struct device *_pdev = container_of( _k, struct device, kobj);
 if ( _off + _count < sizeof_listF - 1) {
   KERR( "%s() fw size %lld < %d\n", __FUNCTION__, _off + _count, sizeof_listF - 1);
   return( -ENXIO);  }
 if ( !_pdev) return( -ENXIO);
 if ( !( _p = ( sppctl_pdata_t *)_pdev->platform_data)) return( -ENXIO);
 sdp = ( sppctl_sdata_t *)_a->private;
 while ( list_funcs[ ++i] && i < _count) sppctl_pin_set( _p, _b[ i], sdp->i);
 return( i);  }

static struct device_attribute sppctl_sysfs_attrsD[] = {
 __ATTR(     name,0444,sppctl_sop_name_R,       NULL),
 __ATTR(     dbgi,0664,sppctl_sop_dbgi_R,       sppctl_sop_dbgi_W),
 __ATTR(   fwname,0664,sppctl_sop_fwname_R,     sppctl_sop_fwname_W),
};

static struct bin_attribute sppctl_sysfs_attrsB[] = {
 __BIN_ATTR( list_funcs,0444,sppctl_sop_list_funcs_R, NULL, SPPCTL_MAX_BUF),
 __BIN_ATTR( txt_map   ,0444,sppctl_sop_txt_map_R,    NULL, SPPCTL_MAX_BUF),
 __BIN_ATTR( fw        ,0444,sppctl_sop_fw_R,         sppctl_sop_fw_W, SPPCTL_MAX_BUF),
};

struct bin_attribute *sppctl_sysfs_Fap;

// ---------- main (exported) functions
void sppctl_sysfs_init( struct platform_device *_pdev) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_pdev->dev.platform_data;
 sppctl_sdata_t *sdp = NULL;
 int i, ret;
 char *tmpp;
 for ( i = 0; i < ARRAY_SIZE( sppctl_sysfs_attrsD); i++) {
   ret = device_create_file( &( _pdev->dev), &sppctl_sysfs_attrsD[i]);
   if ( ret) KERR( "createD[%d] error\n", i);
 }
 for ( i = 0; i < ARRAY_SIZE( sppctl_sysfs_attrsB); i++) {
   ret = device_create_bin_file( &( _pdev->dev), &sppctl_sysfs_attrsB[i]);
   if ( ret) KERR( "createB[%d] error\n", i);
 }
 i = -1;
 sppctl_sysfs_Fap = ( struct bin_attribute *)kzalloc( ( sizeof_listF)*sizeof( struct bin_attribute), GFP_KERNEL);
 sdp = ( sppctl_sdata_t *)kzalloc( ( sizeof_listF)*sizeof( sppctl_sdata_t), GFP_KERNEL);
 while ( ( tmpp = list_funcs[ ++i])) {
   sdp[ i].i = i;
   sdp[ i].pdata = _p;
   sysfs_bin_attr_init( sppctl_sysfs_Fap[ i]);
   sppctl_sysfs_Fap[ i].attr.name = tmpp;
   sppctl_sysfs_Fap[ i].attr.mode = 0664;
   sppctl_sysfs_Fap[ i].read  = sppctl_sop_func_R;
   sppctl_sysfs_Fap[ i].write = sppctl_sop_func_W;
   sppctl_sysfs_Fap[ i].size = SPPCTL_MAX_BUF;
   sppctl_sysfs_Fap[ i].private = &( sdp[ i]);
   ret = device_create_bin_file( &( _pdev->dev), &( sppctl_sysfs_Fap[ i]));
   if ( ret) KERR( "createF[%d,%s] error\n", i, tmpp);
 }
 _p->sysfs_sdp = sdp;
 return;  }

void sppctl_sysfs_clean( struct platform_device *_pdev) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_pdev->dev.platform_data;
 int i;
 char *tmpp;
 for ( i = 0; i < ARRAY_SIZE( sppctl_sysfs_attrsD); i++) {
   device_remove_file( &( _pdev->dev), &sppctl_sysfs_attrsD[i]);
 }
 for ( i = 0; i < ARRAY_SIZE( sppctl_sysfs_attrsB); i++) {
   device_remove_bin_file( &( _pdev->dev), &sppctl_sysfs_attrsB[i]);
 }
 i = 0;
 while ( ( tmpp = list_funcs[ i++])) {
   device_remove_bin_file( &( _pdev->dev), &( sppctl_sysfs_Fap[ i]));
 }
 kfree( sppctl_sysfs_Fap);
 kfree( ( sppctl_sdata_t *)( _p->sysfs_sdp));
 return;  }
