#!/bin/bash

SERVICE="$1"
VERSION="$2"
PACKAGE="$SERVICE-$VERSION"

if [ "$1" == "" ]; then
  echo "update.sh <service> <version>"
  exit
fi
if [ "$2" == "" ]; then
  echo "update.sh <service> <version>"
  exit
fi

# package rpm
rpmbuild --define "_topdir `pwd`" -v -ba SPECS/$SERVICE.spec >& rpmbuild-$PACKAGE.log
