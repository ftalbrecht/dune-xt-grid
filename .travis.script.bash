#!/bin/bash
#
# ~~~
# This file is part of the dune-xt-grid project:
#   https://github.com/dune-community/dune-xt-grid
# Copyright 2009-2018 dune-xt-grid developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2017 - 2018)
# ~~~

# ****** THIS FILE IS AUTOGENERATED, DO NOT EDIT **********
# this file is treated as a jinja2 template

set -e
set -x

WAIT="${SUPERDIR}/scripts/bash/travis_wait_new.bash 45"
source ${SUPERDIR}/scripts/bash/retry_command.bash

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} configure
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD}
if [ x"${TESTS}" == x ] ; then
    ${WAIT} ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} test_binaries
else
    ${WAIT} ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} test_binaries_builder_${TESTS}
fi

source ${OPTS}
CTEST="ctest -V --timeout ${DXT_TEST_TIMEOUT:-300} -j ${DXT_TEST_PROCS:-2}"

if [ x"${TESTS}" == x ] ; then
    ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${CTEST}
    ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} headercheck
else
    # with binning headercheck is included in building tests
    ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${CTEST} -L "^builder_${TESTS}$"
fi

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} install | grep -v "Installing"
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} package_source

# ****** THIS FILE IS AUTOGENERATED, DO NOT EDIT **********