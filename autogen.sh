#! /bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PROJECT=Unique
TEST_TYPE=-d
FILE=unique

test ${TEST_TYPE} ${FILE} || {
        echo "You must run this script in the top-level ${PROJECT} directory"
        exit 1
}

GTKDOCIZE=`which gtkdocize`
if test -z $GTKDOCIZE; then
        echo "*** No gtk-doc support ***"
        echo "EXTRA_DIST =" > gtk-doc.make
else
        gtkdocize || exit $?
fi

which gnome-autogen.sh || {
    echo "*** You need to install gnome-common from GNOME SVN:"
    echo "***  svn co http://svn.gnome.org/svn/gnome-common/trunk gnome-common"
    exit 1
}

REQUIRED_AUTOMAKE_VERSION=1.8 . gnome-autogen.sh
