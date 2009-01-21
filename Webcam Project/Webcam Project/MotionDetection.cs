using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace VideoStreamMerger
{
    class MotionDetection
    {
        private int verschil;
        private byte[] backGround;
        private int lengte;
        private TCPOut socket;

        public MotionDetection(int verschil, byte[] achtergrond, int lengte, TCPOut socket)
        {
            this.verschil = verschil;
            this.lengte = lengte;
            this.backGround = achtergrond;
            this.socket = socket;
            this.socket.Start();
        }

        public bool BewegingZoeken(byte[] frame)
        {
            try
            {
                int max = backGround.Length;
                int ver = 0, tx = 0, ty = 0, aantal = 0, x = 0, y = 0;
                for (int i = 54; i < max; i++, tx++)
                {
                    if (tx == lengte)
                    {
                        tx = -1;
                        ty++;
                    }
                    else
                    {
                        ver = backGround[i] - frame[i];
                        if (ver < 0) ver *= -1;
                        if (ver > verschil)
                        {
                            aantal++;
                            x += tx;
                            y += ty;
                        }
                    }
                }
                //de positie opsturen waarin 0 helemaal links is en 255 helemaal rechts
                byte[] temp = new byte[2];
                temp[0] = (byte)((lengte / 255) * (x / aantal));
                temp[1] = (byte)((lengte / 255) * (y / aantal));
                return socket.Verzenden(temp);
            }
            catch { return false; }
        }

        public TCPOut Socket { get { return socket; } }
    }
}
