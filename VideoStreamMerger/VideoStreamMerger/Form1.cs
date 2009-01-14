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

using System.Drawing.Imaging;

namespace VideoStreamMerger
{
    public partial class Form1 : Form
    {
        private Socket sock;
        private Bitmap imageLinks, imageRechts, image;
        private VideoInput video1, video2;
        private int height, width, verschil;
        private byte[] dataImage=null, dataRechts=null, dataLinks=null;
        private int imLengte = 0;
        private MemoryStream msL = new MemoryStream(), msR = new MemoryStream();

        //variabelen die aan te passen zijn om zo de kwaliteit of de snelheid te verhogen
        private int kolom = 5; //aantal kolommen langs elkaar die vergeleken worden
        private int frames = 50; //aantal frames waaruit een achtergrond gehaald word
        private int pixels = 1; //de precisie waarmee de achtergrond bepaald word
        private float percentage = (float)0.95; //hoeveel procent de stukken hetzelfde moeten zijn
        private int temp = 1; //temp: hoeveel pixels er worden overgeslagen
  //      private int ver = 15; //hoe precies de pixels hetzelfde moeten zijn

        public Form1()
        {
            InitializeComponent();            
            
            //videobestanden en/of webcams laden en achtergrond bepalen
     //       VideoLadenEnAchtergrondBepalen();
            CaptureDeviceForm form = new CaptureDeviceForm();
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                // create video source
                CaptureDevice localSource = new CaptureDevice();
                localSource.VideoSource = form.Device;

                // open it
                video1 = new VideoInput(localSource, frames, pixels);
                while (!video1.BackgroundFound) { }
                label1.Text = "Frames voor optimalisatie: " + video1.Source.FramesReceived;
                imageLinks = video1.backGround;
                imageLinks.Save("c://Achtergrond_Links.bmp");          
            }
   //         pictureBox1.Size = imageLinks.Size;
  //          video1.frame += new VideoInput.EventHandler(video1_frame);

            imageRechts = new Bitmap("c://temp000.bmp");
            if (ImagesVergelijken())
            {
                NieuweImageInitialiseren();
                pictureBox1.Size = image.Size;
                pictureBox1.Image = image;
            }

            //kijken waar de bitmaps over elkaar gaan
            //als niets overeenkomt, achtergronden opslaan en niets doen
  /*          if (!ImagesVergelijken())
            {
                MessageBox.Show("Geen vergelijking gevonden, afbeeldingen opgeslagen (c:/Achtergrond_Rechts en c:/Achtergrond_Links)");
                imageLinks.Save("c://Achtergrond_Links.bmp");
                imageRechts.Save("c://Achtergrong_Rechts.bmp");
            }
            else
            {
   //             try
                {
   //                 IPAddress[] ips = Dns.GetHostAddresses("localhost");
                    //verbinding maken met server (nu local)
     //              sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
       //            sock.Connect(new System.Net.IPEndPoint(ips[1], 1234));
                    
                    //eerste keer moeten de afmetingen van de image bepaald worden
                    NieuweImageInitialiseren();
                    MemoryStream ms = new MemoryStream(dataImage);
                    image = new Bitmap(ms);
                    image.Save("c://test_achtergrond.bmp");

                    //2 bitmaps samenvoegen (voor altijd blijven doen)
                    video1.frame += new VideoInput.EventHandler(video1_frame);
                    video2.frame += new VideoInput.EventHandler(video2_frame);


                    //voor altijd de streams samenvoegen en doorsturen
                    while (true)
                    {
                        //wachten totdat er nieuwe frames zijn
                        if (imageLinks != null && imageRechts != null)
                        {
                            //images samenvoegen
                            ImagesSamenvoegen();
                            //image verzend
                            ImageVerzenden();
                            //wachten op nieuwe images
                            imageLinks = imageRechts = null;
                        }
                    }
                }
  //              catch
                {
                    MessageBox.Show("Geen verbinding met de Server.");
                }
            }*/

        }

        private void VideoLadenEnAchtergrondBepalen()
        {
            //linkervideo
            VideoFileSource videoSource = new VideoFileSource();
            videoSource.VideoSource = "c:\\Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
            video1 = new VideoInput(videoSource, frames, pixels);
            while (!video1.BackgroundFound) { }
            label1.Text = "Frames voor optimalisatie: " + video1.Source.FramesReceived;
            imageLinks = video1.backGround;
            
            //rechtervideo
            videoSource = new VideoFileSource();
            videoSource.VideoSource = "c:\\Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
            video2 = new VideoInput(videoSource, frames, pixels);
            while (!video2.BackgroundFound) { }
            label2.Text = "Frames voor optimalistatie: " + video2.Source.FramesReceived;
            imageRechts = video2.backGround;
        }

        //alleen een nieuwe frame accepteren als de oude verwerkt is?
        //alleen doen als het pogramma snel genoeg is...
        void video1_frame(Bitmap frame)
        {
            pictureBox1.Image = frame;
            if (imageRechts==null)
                imageRechts = frame;
        }
        void video2_frame(Bitmap frame)
        {
            if (imageLinks==null)
                imageLinks = frame;
        }

        //todo: nu moet de hele kolom hetzelfde zijn: dus als 1 webcam iets hoger staat, vind die al niets
        private bool ImagesVergelijken()
        {
            //alleen frames samenvoegen als ze even hoog zijn
            if (imageRechts.Height != imageLinks.Height)
                MessageBox.Show("de hoogtes van de images zijn niet gelijk");
            height = imageLinks.Height;

            //variabelen voor de rechter frame declareren
            int i, x=0, y=0, max = height*kolom;
            int[] kolomRechts;
            kolomRechts = new int[max];

            //data van de rechter frame invullen
            for (i = 0; i < max; i++, y++)
            {
                if (y == height)
                {
                    x++;
                    y = 0;
                }
                kolomRechts[i] = imageRechts.GetPixel(x, y).ToArgb();
            }

            //door blijven zoeken naar het goeie strookje
            //of totdat de hele bitmap afgezocht is
            int zoeknr=0;
            while (zoeknr - 1 < (imageLinks.Width-kolom))
            {
                //een kolom pixels van de linkerbitmap pakken
                x = imageLinks.Width-(kolom+zoeknr); y = 0; 
                int[] kolomLinks;
                kolomLinks = new int[max];

                //data van de linker frame laden
                for (i = 0; i < max; i++, y++)
                {
                    if (y == height)
                    {
                        x++;
                        y = 0;
                    }
                    kolomLinks[i] = imageLinks.GetPixel(x, y).ToArgb();
                }

                //kolommen vergelijken
                int totaal = 0; 
                //int temp;
                for (i = 0; i < max; i++)
                {
               //     temp = kolomLinks[i] - kolomRechts[i];
               //     if (temp < 0) temp *= -1;
               //     listBox1.Items.Add(i + ": " + temp);
              //      if (temp < ver)
                    if (kolomLinks[i] == kolomRechts[i])
                        totaal++;
                }

                //kijken hoeveel pixels overeenkomen, als het genoeg is, dan kunnen de frams samengevoegd worden
                float verschil = (float)totaal/(float)max;
                if (verschil > percentage)
                {
                    this.verschil = imageLinks.Width - (kolom + zoeknr);
                    width = this.verschil + imageRechts.Width;
                    return true;
                }
                zoeknr++;
            }
            return false;
        }

        private void NieuweImageInitialiseren()
        {
            //wat variabelen declareren voor de loops en if
            int tempx = 0, tempy = 0;
            int tempw = width / temp;
            int temph = height / temp;
            int tempv = verschil / temp;
            image = new Bitmap(tempw, temph, PixelFormat.Format24bppRgb);
            for (int x = 0; x < tempw; x++)
            {
                for (int y = 0; y < temph; y++)
                {
                    if (x < verschil) //< of <= maakt niet zoveel uit, het gedeelte word anders bij de else gepakt
                        image.SetPixel(x, y, imageLinks.GetPixel(x * temp, y * temp));
                    else
                    {
                        image.SetPixel(x, y, imageRechts.GetPixel(tempx * temp, tempy * temp));
                        tempy++;
                        if (tempy >= temph)
                        {
                            tempy = 0;
                            tempx++;
                        }
                    }
                }
            }
            image.Save("c://totaal.bmp");
            //de afbeelding in het geheugen zetten zodat het aangepast kan worden
            MemoryStream mstemp = new MemoryStream();
     //       image.SetResolution(96, 96);
            image.Save(mstemp, ImageFormat.Bmp);
            dataImage = mstemp.GetBuffer();
            mstemp.Flush();
            mstemp.Close();

            //variabelen invullen voor het samenvoegen van de streams
            imLengte = width * height * 3 + 54;
            verschil *= 3 * height;

            //test:
            imLengte += (width * 2);
        }

        //de twee achtergrond samen voegen en 1 afbeelding ervan maken en deze opslaan als c://temp.bmp
        //werkt gewoon xd
        private void ImagesSamenvoegen()
        {
            imageRechts.Save(msR, ImageFormat.Bmp);
            dataRechts = msR.GetBuffer();
            msR.Flush();

            imageLinks.Save(msL, ImageFormat.Bmp);
            dataLinks = msL.GetBuffer();
            msL.Flush();

            int totaal = (width * 3 + 2) * height+54;
            int i, x=0, y=0, x2=0;
            for (i=54; i<totaal; i++, x++)
            {
                //nieuwe regel
                if (x > (width * 3 + 3))
                {
                    x2 = 0;
                    x = -1;
                    y++;
           //         dataImage[i] = 0;
            //        i--;
             //       dataImage[i] = 0;
             //       i++;

                }
                else
                {
                    //links laden
                    if (x <= 957)
                    {
                        dataImage[i] = dataLinks[54 + x + (y * 962)];
                    }
                    //rechts laden
                    else
                    {
                        dataImage[i+2] = dataRechts[54 + x2 + (y * 962)];
                        x2++;
                    }
                }
            }

            MemoryStream ms2 = new MemoryStream(dataImage);
            image = new Bitmap(ms2);
            image.Save("c://test.bmp");
            ms2.Flush();

            //images samenvoegen
  /*          int tempx = 0, tempw=0;
            int temp = (width * 3 + 2) * height;// +108; //dataImage.Length-54;
            int templ=0, tempr = 0;
            int tempw2 = 1;
            int leng = 0;// 319;
            verschil += 249;
            int i;

  /*          for (i = 54; i < temp; i++, tempw++)
            {
                //einde van de rij beland (1 rij = 1917 aan bytes)
                if (tempw > width*3+1) //1918
                {
                    tempw = -1;
                    templ = 0;
                    tempr = 0; //of tempw = -1
                    leng++;
                    dataImage[i] = 0;
        //            dataImage[i++] = 0;

           //         for (int x = 0; x < 6; x++)
             //           dataImage[i + x] = 255;
           //         dataImage[i++] = 0;
                 }

                    //niet aankomen: een regel klopt nu!!!
                else
                {
                    //linker image laden
                    if (tempw <= 319*3)//960)
                    {
                        dataImage[i] = dataLinks[54 + templ + (leng * 322*3)];
                        templ++;
                    }

                    //rechter image laden (alles laden)
                    else
                    {
    /*                    if (tempr > 320*3)
                        {
                            for (int x = 0; x < 3; x++)
                            {
                                dataImage[i] = 0;
                                tempr++; i++;
                            }
                        }
*/
               //         if (i == 1012) 
/*                            dataImage[i-1] = dataRechts[(54 + tempr + (leng * 322 * 3))];
             //           else dataImage[i] = dataRechts[54 + tempr + (leng * 322 * 3)];
                       tempr++;
                    }
                }
                
          /*      tempw++;
                if (tempw == width)
                {
                    dataImage[i] = 0;
      //              dataImage[i + 1] = 0;
                    tempw = templ = tempr = 0;
                    tempw2++;
        //            i++;
                }
                else
                {
                    if (tempw < 319)//van linkerimage maar een deel laden
                     //   (i < verschil) dataImage[i] = dataLinks[i];
                    {
                        dataImage[i] = dataLinks[54 + templ * tempw2];
                        templ++;
                    }
                    else //alles van rechterimage laden
                    {
                        dataImage[i] = dataRechts[54 + tempr * tempw2];
                        tempr++;
                   //     return;
                    }
                }*/
 //           }
    //        MemoryStream ms2 = new MemoryStream(dataImage);
      //      image = new Bitmap(ms2);
        //    image.Save("c://test.bmp");
            
        }

        private void ImageVerzenden()
        {
            sock.Send(dataImage);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (video1 != null) video1.Close();
            if (video2 != null) video2.Close();
            if (sock != null) sock.Close();
        }
    }
}