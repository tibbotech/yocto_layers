# test_c_api_gpio.c 
------------------
The user-space app that does <RATE> (see line 14) GPIO switches per second.
it uses new user-space gpio API.
Seems, it is runs mostly accurate till 200 KGZ.
It uses busy-wait cycle, polling current time case there is no accurate microsecond sleep function in user-space.
Better results (accurately up to 1MZ I think) could be done in kernel as the module, case there are microsecond sleep function exists in kernel space. But for user space with 10KGZ it should be more then enough.

