using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using VideoSource;

namespace VideoStreamMerger
{
    public partial class temp : Form
    {
        Bitmap imageLinks=null;
        Background backgroundLinks = new Background();
        VideoInput video;

        public temp()
        {
            InitializeComponent();

            //met video...
            CaptureDeviceForm form = new CaptureDeviceForm();
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                // create video source
                CaptureDevice localSource = new CaptureDevice();
                localSource.VideoSource = form.Device;

                // open it
                AchtergrondBepalen(localSource);
            }
            else
            {
                VideoFileSource videoSource = new VideoFileSource();
                videoSource.VideoSource = "C:\\Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
                AchtergrondBepalen(videoSource);
            }

            pictureBox1.Size = imageLinks.Size;
            pictureBox1.Location = new Point(0, 0);
            pictureBox1.Image = imageLinks;
        }

        //programma blijft in deze methode hangen totdat er een background gevonden is
        private void AchtergrondBepalen(IVideoSource source)
        {
            video = new VideoInput(source);
            while (!video.BackgroundFound) { }
            imageLinks = video.backGround;
        }
    }
}
