#!/bin/sh
HOST='192.168.0.107'
USER='nemftp'
PASSWD='a'
FILE='app.tgz'

ftp -n $HOST <<END_SCRIPT
quote USER $USER
quote PASS $PASSWD
bi
cd OpenGLCross
lcd tgt
put $FILE

quit
END_SCRIPT
exit 0
