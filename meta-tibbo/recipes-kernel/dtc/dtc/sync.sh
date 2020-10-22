#rsync -rvz \
# --exclude=.*.html --exclude=*.sh --exclude=*-closed-*.rpm \
# --delete \
# ../../rpm/ rsync://down@tibbo.com/down_ltps_repos
rsync -rvzz \
 --exclude=.*.html --exclude=*.sh --exclude=*-closed-*.rpm \
 --exclude=*c103* \
 --delete \
 ../../rpm/ rsync://down@tibbo.com/down_ltps_repos_tppg2