rsync -rvz \
 --exclude=.*.html --exclude=*.sh --exclude=*-closed-*.rpm \
 --delete \
 ../rpm/ rsync://down@tibbo.com/down_ltps_repos