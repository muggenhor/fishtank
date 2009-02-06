using System;
using System.Collections.Generic;
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
                //i 3 verhogen om zodat rgb samegevoegd kunnen worden tot een gem kleur
                for (int i = 54; i < max; i+=3, tx+=3)
                {
                    if (tx >= lengte)
                    {
                        tx = -1;
                        ty++;
                    }
                    else
                    {
                        int gemAchter = (backGround[i] + backGround[i + 1] + backGround[i + 2]) / 3;
                        int gemFrame = (frame[i] + frame[i + 1] + frame[i + 1]) / 3;
                   //     int ver = backGround[i] - frame[i]; // (int)backGround[i] - (int)frame[i];
                        int ver = gemAchter - gemFrame;
                        if (ver < 0) ver = -ver;
                        if (ver > 50)
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

                //nieuwe frame als achtergrond gebruiken bij het zoeken van beweging
                backGround = (byte[])frame.Clone();

                //indien er te weinig beweging is niets doorsturen
                if (aantal < 1000)
                {
                    return null;
                }
                else
                {
                    
                    if (x > 0)
                    {
                        temp[0] = (byte)((double)x / (double)aantal / (double)lengte * 100);
                        temp[0] = (byte)(-(int)temp[0] + 100); //positie omdraaien omdat alles in spiegelbeeld staat
                    }
                    if (y > 0)
                    {
                        temp[1] = (byte)((double)y / (double)aantal / (double)ty * 100);
                    }
                    if ((int)temp[0] == 0 && (int)temp[1] == 0)
                        return null;//geen goeie coordinaten dus doen alsof er geen beweging was xd
                    
                    //        return socket.Verzenden(temp);
                    return temp;
                }
            }
            catch { return null; }
        }

        public TCPOut Socket { get { return socket; } }
   //     public byte[] Achtergrond { get { return backGround; } set { backGround = value; } }
    }
}
