using System;
using System.Collections.Generic;
using System.Text;

namespace VideoStreamMerger
{
    class ShortSide
    {
        private VideoInput video;
        private TCPOut socket;

        public ShortSide(VideoSource.CaptureDevice webcam, string ip, int poort)
        {
            video = new VideoInput(webcam);
            socket = new TCPOut(ip, poort);
            
        }

        public void Start()
        {
            socket.Start();
            video.frame += new VideoInput.EventHandler(video_frame);
        }

        void video_frame(System.Drawing.Bitmap frame)
        {
            System.IO.MemoryStream ms = new System.IO.MemoryStream();
            frame.Save(ms, System.Drawing.Imaging.ImageFormat.Bmp);
            ms.Flush();
            byte[] data = ms.ToArray();
            socket.Verzenden(data);
            ms.Close();
            
        }

        public TCPOut Socket { get { return socket; } }
        public bool Stop { get { try { video.Close(); return true; } catch { return false; } } }
    }
}
