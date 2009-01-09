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

using System.Net.Sockets;
using System.IO;
using System.Net;

namespace VideoStreamMerger
{
    public partial class Form1 : Form
    {
        private Socket sock;
        private TcpClient tc;
        private StreamWriter sw;
        private NetworkStream ns;
        private System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf;
        private System.IO.MemoryStream ms;
        private Bitmap imageLinks, imageRechts, image;
        private VideoInput video1, video2;
        private int height, width, verschil;

        //variabelen die aan te passen zijn om zo de kwaliteit of de snelheid te verhogen
        private int kolom = 1; //aantal kolommen langs elkaar die vergeleken worden
        private int frames = 1; //aantal frames waaruit een achtergrond gehaald word
        private int pixels = 10; //de precisie waarmee de achtergrond bepaald word
        private float percentage = (float)0.95; //hoeveel procent de stukken hetzelfde moeten zijn
        private int temp = 2; //temp: hoeveel pixels er worden overgeslagen

        public Form1()
        {
            InitializeComponent();

            //linkervideo
            VideoFileSource videoSource = new VideoFileSource();
            videoSource.VideoSource = "C:\\Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
            video1 = new VideoInput(videoSource, frames, pixels);
            AchtergrondBepalen(videoSource);
            label1.Text = "Frames voor optimalisatie: " + video1.Source.FramesReceived;
            //achtergrond gevonden en de bitmap ff opslaan
            imageLinks = video1.backGround; 
            pbImageLinks.Size = imageLinks.Size;
            pbImageLinks.Location = new Point(0, 0);
            pbImageLinks.Image = imageLinks;
            //rechtervideo
            videoSource = new VideoFileSource();
            videoSource.VideoSource = "C:\\Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
            video2 = new VideoInput(videoSource, frames, pixels);
            while (!video2.BackgroundFound) { } 
            label2.Text = "Frames voor optimalistatie: " + video2.Source.FramesReceived;
            imageRechts = video2.backGround;

      /*    imageRechts = new Bitmap("c://Rechts.bmp");
            pbImageRechts.Size = imageRechts.Size;
            pbImageRechts.Image = imageRechts;*/

            //kijken waar de bitmaps over elkaar gaan
            if (!ImagesVergelijken())
                MessageBox.Show("Geen vergelijking gevonden");
            else
            {
                //tijdelijk
                ImagesSamenvoegen();

                //connectie maken met sockets om bitmaps daarover te versturen
                //     tc = new TcpClient("localhost", 1234);
                //   ns = tc.GetStream();
                // sw = new StreamWriter(ns);
                //      sock = tc.Client;

                IPAddress[] ips = Dns.GetHostAddresses("localhost");
                //tijdelijk (als er geen verbinding is)
                pictureBox1.Location = new Point(0, 0);
                pictureBox1.Size = image.Size;
                try
                {

                    sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);

                    sock.Connect(new System.Net.IPEndPoint(ips[1], 1234));

                    //2 bitmaps samenvoegen (voor altijd blijven doen)
                    video1.frame += new VideoInput.EventHandler(video1_frame);
                    video2.frame += new VideoInput.EventHandler(video2_frame);
                    while (true)
                    {
                        if (imageLinks != null && imageRechts != null)
                        {
                            //images samenvoegen
                            ImagesSamenvoegen();
                            //image verzend
                            ImageVerzenden();
                            //wachten op nieuwe images
                            imageLinks = imageRechts = null;
                            //              imageRechts = new Bitmap("c://Rechts.bmp"); //omdat het nu even een bitmap is, meteen weer vullen}
                            //            imageLinks = new Bitmap("c://tempServer.bmp");
                        }
                    }
                }
                catch
                {
                    // MessageBox.Show("Geen verbinding met de server (" + ips[1].ToString() + ")");
                    pictureBox1.Image = image;
                }
            }

        }

        //alleen een nieuwe fram accepteren als de oude verwerkt is?
        //alleen doen als het pogramma snel genoeg is...
        void video1_frame(Bitmap frame)
        {
            if (imageRechts == null)
                imageRechts = frame;
        }
        void video2_frame(Bitmap frame)
        {
            if (imageLinks == null)
                imageLinks = frame;
        }

        //programma blijft in deze methode hangen totdat er een background gevonden is
        private void AchtergrondBepalen(IVideoSource source)
        {
            video1 = new VideoInput(source, 2, 10);
            while (!video1.BackgroundFound) {}
        }

        #region test vergelijken
        //todo: werkt niet en is nu zeer, zeer, zeer lang bezig (zeker 10x langzamer)
        private bool ImagesVergelijken2()
        {
            if (imageRechts.Height != imageLinks.Height)
                MessageBox.Show("de hoogtes van de images zijn niet gelijk");

            //5 kolommen pixels van de rechterbitmap pakken en de helft van een kolom pakken
            //hoe meer kolommen hoe kleiner de kans dat de 'verkeerde' reeks gepakt word
            int imageHeight = imageLinks.Height;
            int i, x = 0, y = 0, max = (int)(imageHeight * 2.5); //2.5: 5 kolommen, elke de helft
            int[] kolomRechts;
            kolomRechts = new int[max];

            int kolomonder = (int)(imageHeight * 0.25);
            int kolomboven = (int)(imageHeight * 0.75);

            y = kolomonder;
            for (i = 0; i < max; i++, y++)
            {
                if (y == kolomboven)
                {
                    x++;
                    y = kolomonder;
                }
                kolomRechts[i] = imageRechts.GetPixel(x, y).ToArgb();
            }

            //door blijven zoeken naar het goeie strookje
            //of totdat de hele bitmap afgezocht is
            int zoeknr = 0; 
            int halvekolom = (int)imageHeight/2;
            while (zoeknr - 1 < (imageLinks.Width - 5))
            {
                //de hoogte van de kolom aanpassen
                for (int kolomnr=0; kolomnr<halvekolom; kolomnr++ )
                {
                    //een kolom pixels van de linkerbitmap pakken
                    x = imageLinks.Width - (5 + zoeknr); y = 0; //de x verhogen met 1, elke keer als de kolommen niet hetzelfde zijn 
                    int[] kolomLinks;
                    kolomLinks = new int[max];

                    for (i = 0; i < max; i++, y++)
                    {
                        if (y == halvekolom + kolomnr)
                        {
                            x++;
                            y = kolomnr;
                        }
                        kolomLinks[i] = imageLinks.GetPixel(x, y).ToArgb();
                    }

                    //kolommen vergelijken
                    int totaal = 0;
                    for (i = 0; i < max; i++)
                        if (kolomLinks[i] == kolomRechts[i])
                            totaal++;

                    //ff voor de lol de picturebox verplaatsen als het voor 95% hetzelfde is 
                    float verschil = (float)totaal / (float)max;
                    lbVerschillen.Items.Add(zoeknr + "/" + kolomnr + ": " + verschil);
                    if (verschil > 0.95) //95% hetzelfde (zeer hoog!!!)
                    {
                        this.verschil = pbImageLinks.Width - (5 + zoeknr);
                        pbImageRechts.Location = new Point(this.verschil, 0);
                        return true;
                    }
                }
                zoeknr++;
            }
            return false;
        }
        #endregion

        //todo: nu moet de hele kolom hetzelfde zijn: dus als 1 webcam iets hoger staat, vind die al niets
        private bool ImagesVergelijken()
        {
            if (imageRechts.Height != imageLinks.Height)
                MessageBox.Show("de hoogtes van de images zijn niet gelijk");
            height = imageLinks.Height;

            //5 kolommen pixels van de rechterbitmap pakken
            //hoe meer kolommen hoe kleiner de kans dat de 'verkeerde' reeks gepakt word
            int imageHeight = imageLinks.Height;
            int i, x=0, y=0, max = imageHeight*kolom; //imageHeight*5
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
            while (zoeknr - 1 < (imageLinks.Width-kolom)) //imageLinks.Width-5
            {
                //een kolom pixels van de linkerbitmap pakken
                x = imageLinks.Width-(kolom+zoeknr); y = 0; //de x verhogen met 1, elke keer als de kolommen niet hetzelfde zijn 
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
  //              if (verschil > percentage) //95% hetzelfde (zeer hoog!!!)
                {
                    this.verschil = imageLinks.Width - (kolom + zoeknr); //5 + zoeknr
                    pbImageRechts.Location = new Point(this.verschil, 0);
                    width = imageLinks.Width - (kolom + zoeknr) + imageRechts.Width; //5 + zoeknr
                    return true;
                }
                zoeknr++;
            }
            return false;
        }

        //de twee achtergrond samen voegen en 1 afbeelding ervan maken en deze opslaan als c://temp.bmp
        //werkt gewoon xd
        private void ImagesSamenvoegen()
        {
            int tempx=0, tempy=0;
            //ff snel een bitmap opslaan (om te kijken of het mogelijk is)
            image = new Bitmap(width, height);
            for (int x=0; x<width/temp; x++)
                for (int y=0; y<height/temp; y++)
                {   
                    if (x <= verschil/temp)
                        image.SetPixel(x,y, imageLinks.GetPixel(x*temp,y*temp));
                    else
                    {
                        image.SetPixel(x,y, imageRechts.GetPixel(tempx*temp,tempy*temp));
                        tempy++;
                        if (tempy >= height/temp)
                        {
                            tempy = 0;
                            tempx++;
                        }
                    }
                }
          //  image.Save("c://temp.bmp");
        }

        private void ImageVerzenden()
        {
            //serializeren
            //image.Save("C:\\tempje.bmp");
            bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
            ms = new System.IO.MemoryStream(); //indien nodig
            bf.Serialize(ms, image);
            //verzenden
            sock.Send(ms.ToArray());
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (sw != null) sw.Close();
            if (ns != null) ns.Close();
            if (video1 != null) video1.Close();
            if (video2 != null) video2.Close();
            if (sock != null) sock.Close();
        }
    }
}
