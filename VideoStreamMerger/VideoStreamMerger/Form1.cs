using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using VideoSource;
using Tiger.Video.VFW;

namespace VideoStreamMerger
{
    public partial class Form1 : Form
    {
        Bitmap imageLinks, imageRechts;
        Background backgroundLinks = new Background();
        VideoInput video;

        public Form1()
        {
            InitializeComponent();

            //met video...
            VideoFileSource videoSource = new VideoFileSource();
            videoSource.VideoSource = "C:\\Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
            AchtergrondBepalen(videoSource);

            //met bitmaps...
          /*  FilesSelecteren();
            if (ImagesVergelijken() == false)
                MessageBox.Show("Geen vergelijking gevonden");*/
        }

        //programma blijft in deze methode hangen totdat er een background gevonden is
        private void AchtergrondBepalen(IVideoSource source)
        {
            label1.Text = "bezig met achtergrond...";
            video = new VideoInput(source);
            while (!video.BackgroundFound) { }
            imageLinks = video.backGround;
            label1.Text = "achtergrond gevonden !!!";

            //bitmaps selecteren
            
            pbImageLinks.Size = imageLinks.Size;
            pbImageLinks.Location = new Point(0, 0);
            pbImageLinks.Image = imageLinks;
        }

        #region Met Bitmaps
        private void FilesSelecteren()
        {
            //bitmaps selecteren
            imageLinks = new Bitmap("c://Links.bmp");
            imageRechts = new Bitmap("c://Rechts.bmp");

            //grootte van pictureboxen bepalen
            pbImageLinks.Size = imageLinks.Size;
            pbImageRechts.Size = imageRechts.Size;

            //locatie van picturebocen bepalen
            pbImageLinks.Location = new Point(0, 0);
            pbImageRechts.Location = new Point(imageLinks.Width, imageLinks.Height);

            //afbeeldingen in pictureboxen zetten
            pbImageLinks.Image = imageLinks;
            pbImageRechts.Image = imageRechts;
        }

        private bool ImagesVergelijken()
        {
            //5 kolommen pixels van de rechterbitmap pakken
            //hoe meer kolommen hoe kleiner de kans dat de 'verkeerde' reeks gepakt word
            int imageHeight = imageLinks.Height;
            int i, x=0, y=0, max = imageHeight*5;
            int[] kolomRechts;
            kolomRechts = new int[max];

            for (i = 0; i < max; i++, y++)
            {
                if (y == imageHeight)
                {
                    x++;
                    y = 0;
                }
                kolomRechts[i] = imageRechts.GetPixel(x, y).ToArgb();
            }

            //door blijven zoeken naar het goeie strookje
            //of totdat de hele bitmap afgezocht is
            int zoeknr=0;
            while(zoeknr-1 < (imageLinks.Width-5))
            {
                //een kolom pixels van de linkerbitmap pakken
                x = imageLinks.Width-(5 + zoeknr); y = 0; //de x verhogen met 1, elke keer als de kolommen niet hetzelfde zijn 
                int[] kolomLinks;
                kolomLinks = new int[max];

                for (i = 0; i < max; i++, y++)
                {
                    if (y == imageHeight)
                    {
                        x++;
                        y = 0;
                    }
                    kolomLinks[i] = imageLinks.GetPixel(x, y).ToArgb();
                }

                //kolommen vergelijken
                int totaal=0;
                for (i = 0; i < max; i++)
                    if (kolomLinks[i] == kolomRechts[i])
                        totaal++;

                //ff voor de lol de picturebox verplaatsen als het voor 95% hetzelfde is 
                float verschil = (float)totaal/(float)max;
                lbVerschillen.Items.Add(zoeknr + ": " + verschil);
                if (verschil > 0.95) //95% hetzelfde (zeer hoog!!!)
                {
                    pbImageRechts.Location = new Point(pbImageLinks.Width - (5 + zoeknr), 0);
                    return true;
                }
                zoeknr++;
            }
            return false;
        }

        private void ImagesSamenvoegen()
        {
        }
        #endregion
    }
}
