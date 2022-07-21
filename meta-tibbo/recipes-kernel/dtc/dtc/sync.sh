#rsync -rvz \
# --exclude=.*.html --exclude=*.sh --exclude=*-closed-*.rpm \
# --delete \
# ../../rpm/ rsync://down@tibbo.com/down_ltps_repos
rsync -rvzz \
 --exclude=.*.html --exclude=*.sh --exclude=*-closed-*.rpm \
 --exclude=*c103* --exclude=*nativesdk* --exclude=*dummy_target* \
 --delete \
 ../../rpm/ rsync://down@tibbo.net/down_ltps_repos_tppg2