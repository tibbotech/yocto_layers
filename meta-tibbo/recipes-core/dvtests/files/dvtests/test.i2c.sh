# test i2c audio NAK
while true; do i2cset -f -y 1 0x18 0x07 0x8a; done