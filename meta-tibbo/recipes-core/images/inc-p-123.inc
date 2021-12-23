inherit extrausers

# see http://www.devconsole.info/?p=211
# 123 in sha-512
#EXTRA_USERS_PARAMS = "usermod -p \$6\$4jZ2FV34JZQ01jT\$rnNDa7x6j4TwfZPyA2gL.8iUQ1rbm.eZeZUgKvL5evXEJRlA5FiUbo9Rb/cX4bypj.as8zf5dZ/vxVjBXO9Sg1 root;"
# 123 in md5
#EXTRA_USERS_PARAMS = "usermod -p `openssl passwd -1 -salt tibbo 123` root;"
#EXTRA_USERS_PARAMS += "usermod -P 123 root;"
# in Honister only -p supported
EXTRA_USERS_PARAMS += "usermod -p $(openssl passwd '123') root;"
