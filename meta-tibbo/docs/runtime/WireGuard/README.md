# WireGuard

[WireGuard](https://www.wireguard.com/) is the ultra-fast and lightweight 
secure tunnel software.

It requires iproute2 package (default in BASE image).

## Installation

```
# dnf install wireguard-tools
```

## Configuration

Checking the config file:
```
# cd /etc/wireguard
# cat ./wg0.conf
[Interface]
PrivateKey = CArN6JltFIgcLfP5KJA4vXJ97FrELlTpv3/EHznYWFc=
[Peer]
PublicKey = g+7+42dzk1vlJE62NJgPCfwfsDzf+qYtsgazpZKTKEw=
Endpoint = us-tx1.wg.ivpn.net:2049
AllowedIPs = 0.0.0.0/0, ::/0
```

## Starting the tunnel

Manually:
```
# wg setconf wg0 wg0.conf
# ip link set up dev wg0
```
OR
reboot, wait for network to be configured and run:
```
# systemctl restart wg-quick@wg0
```

## Testing

```
# dnf -y install nmap
```
then upload client scripts from:
https://github.com/WireGuard/wireguard-tools/tree/master/contrib/ncat-client-server
and run:
```
# ./client-quick.sh
```
then run the "demo" configuration:
```
# wg-quick up demo
```
