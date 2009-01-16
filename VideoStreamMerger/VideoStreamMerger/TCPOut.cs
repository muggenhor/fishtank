using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net.Sockets;
using System.Net;

namespace VideoStreamMerger
{
    class TCPOut
    {
        //variabelen
        private Socket sock;

        //constructor
        public TCPOut(string ip, int port)
        {
            IPAddress[] ips = Dns.GetHostAddresses(ip);
            sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
            sock.Connect(new System.Net.IPEndPoint(ips[1], port));
        }
        public TCPOut()
            : this("localhost", 1234) { }

        public bool Verzenden(byte[] data)
        {
            try
            {
                sock.Send(data);
                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}
