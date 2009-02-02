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

        public MotionDetection(int verschil, int lengte, byte[] achtergrond, TCPOut socket)
        {
            this.verschil = verschil;
            this.lengte = lengte;
            this.backGround = achtergrond;
  //          Bitmap.FromStream(new System.IO.MemoryStream(achtergrond)).Save("c://achtergrond.bmp");
            this.socket = socket;
            this.socket.Start();
        }

        public byte[] BewegingZoeken(byte[] frame)
        {
            try
            {
       //         Bitmap.FromStream(new System.IO.MemoryStream(backGround)).Save("c://achtergrond.bmp");
       //         Bitmap.FromStream(new System.IO.MemoryStream(frameIn)).Save("c://frame.bmp");
                int max = backGround.Length;
                int tx = 0, ty = 0, aantal = 0, x = 0, y = 0;
                for (int i = 54; i < max; i++, tx++)
                {
                    if (tx == lengte)
                    {
                        tx = -1;
                        ty++;
                    }
                    else
                    {
                        int ver = backGround[i] - frame[i]; // (int)backGround[i] - (int)frame[i];
                        if (ver < 0) ver = -ver;
                        if (ver > verschil)
                        {
                            aantal++;
                            x += tx;
                            y += ty;
                        }
                    }
                }
                //de positie opsturen waarin 0 helemaal links is en 100 helemaal rechts
                byte[] temp = new byte[2];
    /*            double test = x / aantal;
                test /= (double)lengte;
                test *= 100;*/
                if (x > 0) temp[0] = (byte)((double)x/(double)aantal/(double)lengte*100); else temp[0] = 0;
                if (y > 0) temp[1] = (byte)((double)y/(double)aantal/(double)ty*100); else temp[1] = 0;
        //        return socket.Verzenden(temp);
                return temp;
            }
            catch { return null; }
        }

        public TCPOut Socket { get { return socket; } }
   //     public byte[] Achtergrond { get { return backGround; } set { backGround = value; } }
    }
}
