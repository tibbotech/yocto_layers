Welcome to LTPS Applications home!

Each subdirectory here is dedicated to it's on app. Predefined structure is:
<APPNAME>/
    conf/ (1)
    site/ (2)

(2) application files: index.html or anything
(1) application settings files:

nginx.conf
// if you need some NGINX settings for your app.
// need to reload Nginx to accept settings from this file.

napp-<APPNAME>.service
// SystemD service description file.
// can be created by WebInterface application settings editor.
// https://rocketeer.be/articles/deploying-node-js-with-systemd/

.htpasswd 
// it's recommended to have password files and so on 
// out of the appliclication webroot.
