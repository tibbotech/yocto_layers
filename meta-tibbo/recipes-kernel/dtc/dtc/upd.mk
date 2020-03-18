PATH=./:${PATH}

./mkimage -A arm -f ${1}.its ${1}.itb
md5sum ${1}.itb > ${1}.md5sum