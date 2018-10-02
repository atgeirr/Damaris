#!/bin/sh

sed -e "s/PV_LIBS/$(<pv-config.out.2 sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' | tr -d '\n')/g" damaris.pc.in > damaris.pc
