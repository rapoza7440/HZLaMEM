#!/bin/sh -e

if [ "$1" = "1" ] 
then 
svnversion() {

	manualchanges=`LC_ALL=C svn status | awk '/^M/ { printf("%s, ",$2) } END {print ""}'`
    svnrevision=`LC_ALL=C svn info | awk '/^Revision:/ {print $2}'`
    svndate=`LC_ALL=C svn info | awk '/^Last Changed Date:/ {print $4,$5}'`

    now=`date`
    
    optmode=$PETSC_ARCH
	
    cat <<EOF > Version.h

// Do not edit!  This file was autogenerated
//      by $0
//      on $now
//
// svnrevision and svndate are as reported by svn at that point in time,
// compiledate and compiletime are being filled gcc at compilation
#ifndef VERSION_H_
#define VERSION_H_



#include <stdlib.h>
static const char* __SVNMANCHANGES__ = "$manualchanges";
static const char* __SVNREVISION__ = "$svnrevision";
static const char* __SVNDATE__ = "$svndate";
static const char* __OPTMODE__= "$optmode";
//static const char* compiletime = __TIME__;
//static const char* compiledate = __DATE__;


#endif
EOF
}

test -f Version.h || svnversion

else
    svnrevision="< unknown >"
    svndate="< unknown > "

    now=`date`

    optmode=$PETSC_ARCH

    cat <<EOF > Version.h

// Do not edit!  This file was autogenerated
//      by $0
//      on $now
//
// svnrevision and svndate are as reported by svn at that point in time,
// compiledate and compiletime are being filled gcc at compilation
#ifndef VERSION_H_
#define VERSION_H_



#include <stdlib.h>

static const char* __SVNREVISION__ = "$svnrevision";
static const char* __SVNDATE__ = "$svndate";
static const char* __OPTMODE__= "$optmode";
//static const char* compiletime = __TIME__;
//static const char* compiledate = __DATE__;


#endif
EOF

fi
