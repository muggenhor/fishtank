using System;
using System.Collections.Generic;
using System.Text;

using System.Net.Sockets;
using System.Net;

namespace VideoStreamMerger
{
    class TCPOut
    {
        //variabelen
        private Socket sock;
        private string ip;
        private int port;

        //constructor
        public TCPOut(string ip, int port)
        {
            this.ip = ip;
            this.port = port;
        }
        public TCPOut()
            : this("localhost", 1234) { }

        public bool Start()
        {
            try
            {
                IPAddress[] ips = Dns.GetHostAddresses(ip);
                sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                sock.Connect(new System.Net.IPEndPoint(ips[0], port));
                return true;
            }
            catch { return false; }
        }

        public bool Stop()
        {
            try
            {
                sock.Disconnect(false);
                sock.Close();
                return true;
            }
            catch { return false; }
        }

        /// <summary> wacht met verzenden totdat de ontvanger een berichtje terug gestuurd heeft dat die de data ontvangen heeft <summary>
        public bool Verzenden(byte[] data)
        {
            try { sock.Send(data); return true; }
            catch { return false; }
        }

        //proporties
        public string IpAddres { get { return ip; } }
        public int Poort { get { return port; } }
    }
}
