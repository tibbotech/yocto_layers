# Journald

Current journald settings:
```
journalctl -u systemd-journald -o json-pretty
```

The journal is volatile by default. It is working on top of RAM-mounted 
filesystem for speed and size reasons.


Enabling persistent journal:
```
# mv /var/log /var/log.vol
# mkdir -p /var/log/journal
# systemd-tmpfiles --create --prefix /var/log/journal
# systemctl restart systemd-journald
```

Switch to volatile journal back:
```
# rm -rf /var/log
# mv -f /var/log.vol /var/log
# systemctl restart systemd-journald
```
