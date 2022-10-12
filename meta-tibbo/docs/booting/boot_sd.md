Booting from SD card directly
-----------------------------

1. After the Yocto build go to <builddir>/images/tppg2/ directory and find 
<myimagename>.wic
files there.

2. Write your WIC file to SD card.
- for Linux:
```
dd if=./myimage.wic of=/dev/sd<X> bs=1M
```
- for Windows please use any of 
[the following tools](https://www.makeuseof.com/tag/10-tools-make-bootable-usb-iso-file/)

3. Insert your SD card back to the PC, open its first (FAT) partition and
[copy this file](https://archives.tibbo.com/downloads/LTPS/FW/LTPPg2/ISPBOOOT.BIN-SD)
to your 1'st SD card partiton.

4. Rename ISPBOOOT.BIN-SD to ISPBOOOT.BIN

5. Sync and unmount (In windows Eject) the SD card from your PC.

6. Insert SD card to LTPP3G2 device, short CN10 and CN11 jumpers.

7. Reset the board

