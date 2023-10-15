#!/bin/sh
#==============================================================================
# Copyright (C) 2013 - 2014 Allied Vision Technologies.  All Rights Reserved.
#
# Redistribution of this file, in original or modified form, without
# prior written consent of Allied Vision Technologies is prohibited.
#
#------------------------------------------------------------------------------
#
# File:        Uninstall.sh
#
# Description: Setup script for deleting a startup script that exports the
#	       GENICAM_GENTL32_PATH and GENICAM_GENTL64_PATH variable
#
#------------------------------------------------------------------------------
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
# NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
# DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED  
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#==============================================================================

CWD=$(dirname $(readlink -f $0))
UNAME=$(uname -m)

if [ ${UNAME} = amd64 ] || [ ${UNAME} = x86_64 ]
then
ARCH=x86
elif [ ${UNAME} = aarch64 ]
then
ARCH=arm
else
   echo "Error: Incompatible system architecture found." 1>&2
   exit 1
fi

# Make sure our script is only being run with root privileges
if [ "$(id -u)" != "0" ];
then
   echo "Error: This script must be run with root privileges." 1>&2
   exit 1
fi

TL_NAME=VimbaGigETL
PROFILE_FOLDER=/etc/profile.d

TL_SCRIPT_64BIT=${PROFILE_FOLDER}/${TL_NAME}_64bit.sh
if [ -f $TL_SCRIPT_64BIT ]; then
	echo "Unregistering GENICAM_GENTL64_PATH"
	rm $TL_SCRIPT_64BIT
else
	echo "Could not unregister $TL_NAME 64bit because it was not registered" 
fi

echo "Done"
