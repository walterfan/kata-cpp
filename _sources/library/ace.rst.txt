#################################
ACE
#################################


Example
=================================

* Get weather information

.. code-block:: c++

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "ace/OS_NS_sys_time.h"
    #include "ace/CDR_Stream.h"
    #include "ace/INET_Addr.h"
    #include "ace/SOCK_Connector.h"
    #include "ace/SOCK_Stream.h"
    #include "ace/Log_Msg.h"
    #include "ace/Log_Record.h"
    #include "ace/Truncate.h"
    #include "ace/OS_NS_unistd.h"
    #include "ace/OS_NS_stdlib.h"
    #include "ace/streams.h"

    #define BUFLEN 64
    using namespace std;

    const unsigned int BUFSIZE = 1024;

    int ace_sock_test(int argc, char *argv[])
    {
        //DateTimeServer* pSvr = new DateTimeServer();
        const char* path 
            = argc > 1? argv[1]: "/ig/api?weather=HeFei";
        const char* host
            = argc > 2? argv[2]: "www.google.com";
        int port = argc > 3?
            atoi(argv[3]):80;


        ACE_SOCK_Connector connector;
        ACE_SOCK_Stream peer;
        ACE_INET_Addr addr;

        ACE_Time_Value timeout(10);

        ACE_ASSERT(addr.set(port, host) != -1);

        ACE_ASSERT(connector.connect(peer, addr,&timeout) != -1);

        char buf[BUFSIZE];
        iovec iov[3];
        iov[0].iov_base="GET ";
        iov[0].iov_len = 4;
        iov[1].iov_base=(char*)path;
        iov[1].iov_len = strlen(path);
        iov[2].iov_base=" HTTP/1.0\r\n\r\n";
        iov[2].iov_len = 13;
        
        
        ACE_ASSERT(peer.sendv_n(iov, 3) != -1);

        
        while(true)
        {
            ssize_t n = peer.recv(buf, sizeof(buf), &timeout);
            if (n > 0) {
                ACE::write_n(ACE_STDOUT, buf, n);
            }
            else {
                break;
            }
        }
        return peer.close() == -1? -4:0;
    }


