 fw_setenv c_magic_upd "tps_md;if test \$? = 1; then \
 echo running Tibbo firmware update;\
 if test \$ipaddr = undefined; then \
 echo IP not defined;\
 if test ! \$serverip = undefined; then echo XXX:\$serverip; setenv s_bak \$serverip; fi; \
 dhcp; \
 if test ! \$s_bak = undefined; then echo XXX:\$s_bak; setenv serverip \$s_bak; fi; \
 fi; \
 tps_upd \$tps_upd_p:\$tps_upd_f; \
 fi; \
 b_mmc=echo;"
