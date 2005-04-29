   
   # Create OpenSSL Configuration File (ssl.cnf)
   #
   if [ -f %PEGASUS_CONFIG_DIR/ssl.cnf ]
   then
      echo "WARNING: %PEGASUS_CONFIG_DIR/ssl.cnf already exists."
   else
      echo " Generating SSL Certificate..."
      echo "[ req ]" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "distinguished_name     = req_distinguished_name" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "prompt                 = no" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "[ req_distinguished_name ]" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "C                      = UK" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "ST                     = Berkshire" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "L                      = Reading" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "O                      = The Open Group" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "OU                     = The OpenPegasus Project" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "CN                     = `host \`hostname\`|cut -d\" \" -f1`" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      chmod 400 %PEGASUS_CONFIG_DIR/ssl.cnf
      chown root %PEGASUS_CONFIG_DIR/ssl.cnf
   fi

   %OPENSSL_BIN/openssl req -x509 -days 3650 -newkey rsa:2048 \
      -nodes -config %PEGASUS_CONFIG_DIR/ssl.cnf \
      -keyout %PEGASUS_PEM_DIR/key.pem -out %PEGASUS_PEM_DIR/cert.pem \
           2>>%PEGASUS_INSTALL_LOG

   cat %PEGASUS_PEM_DIR/key.pem > %PEGASUS_PEM_DIR/key-2048.pem
   chmod 400 %PEGASUS_PEM_DIR/key-2048.pem
   cat %PEGASUS_PEM_DIR/cert.pem > %PEGASUS_PEM_DIR/cert-2048.pem
   chmod 400 %PEGASUS_PEM_DIR/cert-2048.pem
   cat %PEGASUS_PEM_DIR/cert.pem > %PEGASUS_PEM_DIR/truststore-2048.pem
   chmod 400 %PEGASUS_PEM_DIR/truststore-2048.pem
   rm -f %PEGASUS_PEM_DIR/key.pem %PEGASUS_PEM_DIR/cert.pem

   if [ -f %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE ]
   then
   echo "WARNING: %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE SSL Certificate file already exists."
   else
      cp %PEGASUS_PEM_DIR/cert-2048.pem %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE
      chmod 444 %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE
      cp %PEGASUS_PEM_DIR/key-2048.pem %PEGASUS_PEM_DIR/%PEGASUS_SSL_KEY_FILE
      chmod 400 %PEGASUS_PEM_DIR/%PEGASUS_SSL_KEY_FILE
   fi

   if [ -f %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE ]
   then
       echo "WARNING: %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE SSL Certificate trust store already exists."
   else
       cp %PEGASUS_PEM_DIR/truststore-2048.pem %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE
       chmod 444 %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE
   fi
   if [[ -d %PEGASUS_PREV_REPOSITORY_DIR ]]
   then
     # Running Repository Upgrade utility
     %PEGASUS_SBIN_DIR/repupgrade %PEGASUS_PREV_REPOSITORY_DIR %PEGASUS_REPOSITORY_DIR 2>>%PEGASUS_INSTALL_LOG
   fi


   /usr/lib/lsb/install_initd /etc/init.d/tog-pegasus

   echo " To start Pegasus manually:"
   echo " /etc/init.d/tog-pegasus start"
   echo " Stop it:"
   echo " /etc/init.d/tog-pegasus stop"
   echo " To set up PATH and MANPATH in /etc/profile"
   echo " run /opt/tog-pegasus/sbin/settogpath."
