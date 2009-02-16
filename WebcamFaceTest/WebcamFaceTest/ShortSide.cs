using System;
using System.Collections.Generic;
using System.Text;

using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace VideoStreamMerger
{
    class ShortSide
    {
        private VideoInput video;
        private TCPOut socket;
        private int links, rechts, onder, boven;
        private byte[] data;
        private int hoogte, breedte, totaal, imgLen;
        

        public ShortSide(VideoSource.CaptureDevice webcam, string ip, int poort)
        {
            video = new VideoInput(webcam);
            socket = new TCPOut(ip, poort);           
        }

        public void Start(int links, int rechts, int onder, int boven)
        {
            this.links = links;
            this.rechts = rechts;
            this.boven = boven;
            this.onder = onder;
            socket.Start();
            video.frame += new VideoInput.EventHandler(video_init);
        }

        void video_init(System.Drawing.Bitmap frame)
        {
            video.frame -= video_init;
            if (NieuweImageInitialiserenAlleen(frame))
                video.frame += new VideoInput.EventHandler(video_frame);
            else
                video.frame += new VideoInput.EventHandler(video_init);
        }

        void video_frame(System.Drawing.Bitmap frame)
        {
            ImageVervormen(frame);
        }

        private bool NieuweImageInitialiserenAlleen(Bitmap imageOrigineel)
        {
            Console.WriteLine("start initiseren");

            hoogte = imageOrigineel.Height - onder - boven;
            breedte = imageOrigineel.Width - links - rechts;
            int x2 = links;
            int y2 = onder;
            Bitmap imageFinal = new Bitmap(breedte, hoogte, PixelFormat.Format24bppRgb);
            for (int y = 0; y < hoogte; y++, y2++)
            {
                for (int x = 0; x < breedte; x++, x2++)
                {
                    imageFinal.SetPixel(x, y, imageOrigineel.GetPixel(x2, y2));
                }
                x2 = links;
            }

            Console.WriteLine("start opslaan");
            imageFinal.Save("c://temp.bmp");

            MemoryStream ms = new MemoryStream();
            imageFinal.Save(ms, ImageFormat.Bmp);
            ms.Flush();
            data = ms.ToArray();
            ms.Close();

            hoogte = imageOrigineel.Height - onder - boven;
            breedte = imageOrigineel.Width - rechts;
            totaal = hoogte * ((breedte - links) * 3) + 54;

            breedte *= 3;
            links *= 3;

            imgLen = imageOrigineel.Width * 3;

            return true;
        }

        private bool ImageVervormen(Bitmap image)
        {
            Console.WriteLine("start vervormen");

            MemoryStream ms = new MemoryStream();
            image.Save(ms, ImageFormat.Bmp);
            ms.Flush();
            byte[] data2 = ms.ToArray();
            ms.Close();

            int x = links;
            int y = onder;

            int i;
            for (i = 54; i < totaal; i++, x++)
            {
                if (x >= breedte - 2)
                {
                    x = links;
                    y++;

                    data[i++] = 0;
                    data[i++] = 0;
                }
                else
                    data[i] = data2[x + 54 + (y * imgLen)];
            }

            Console.WriteLine("start opslaan");
            Bitmap bm = (Bitmap)Bitmap.FromStream(new MemoryStream(data));
            bm.Save("c://temp2.bmp");

            socket.Verzenden(data);

            return true;
        }


        public TCPOut Socket { get { return socket; } }
        public bool Stop { get { try { video.Close(); return true; } catch { return false; } } }
    }
}
