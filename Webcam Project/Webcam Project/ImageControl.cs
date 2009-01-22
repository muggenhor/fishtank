using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Drawing;
using System.Drawing.Imaging;

using VideoSource;
using Tiger.Video.VFW;

using System.IO;

namespace VideoStreamMerger
{
    class ImageControl
    {
        //variabelen die aan te passen zijn om zo de kwaliteit of de snelheid te verhogen
        private int kolom; //5 aantal kolommen langs elkaar die vergeleken worden
        private int frames; //50 aantal frames waaruit een achtergrond gehaald word
        private int pixels; //10 de precisie waarmee de achtergrond bepaald word
        private float percentage; //0.95 hoeveel procent de stukken hetzelfde moeten zijn
        private int ratio; //1 hoeveel pixels er worden overgeslagen
        private int ver; //15 hoe precies de pixels hetzelfde moeten zijn tijdens het vergelijken van 2 bitmaps
        private int verBack; //20 heo precies de pixels hetzelfde moeten zijn tijdens het bepalen van de achtergrond
        private int detVerschil; //20 het verschil dat er maximaal mag zijn bij het detecteren van beweging
        //standaard variabelen
        private Bitmap imageLinks=null, imageRechts=null, image = null;
        private VideoInput video1 = null, video2 = null;
        private int width=0, height=0, verschil=0, imLengte=0;
        private byte[] dataImage = null, dataLinks=null, dataRechts=null;
        private bool doorgaan=true;
        private TCPOut socket, socketMotion;
        private MotionDetection detector;
        private System.Timers.Timer timer;

        //constructors
        /// <summary>
        /// De class die alles doet (het bepalen van de achtergronden, 
        /// het vergelijken, het samenvoegen en het verzenden (via socket)
        /// </summary>
        /// <param name="aantalKolomVergelijken">het aantal kolommen dat vergeleken moet worden tussen de 2 videostreams (bij 1 word er maar 1 kolom vergeleken)), getal moet minmaal 1 zijn (5 is standaard)</param>
        /// <param name="aantalFramesVergelijken">het aantal frames dat van 1 videostream hetzelfde moet zijn om de achtergrond te bepalen (bij 1 word er maar naar 1 frame gekeken), getal moet minimaal 1 zijn (20 is standaard)</param>
        /// <param name="aantalPixelsBekijken">hoeveel pixels er worden overgeslagen met het bepalen van de achtergond (bij 1 worden alle pixels bekeken), moet minimaal 1 zijn (5 is standaard)</param>
        /// <param name="decPercentageHetzelfde">het percentage dat tussn de twee videostrams hetzelfde moet zijn om ze te kunnen samenvoegen (bij 1 moeten ze precies hetzelfde zijn), getal moet tussen 0 en 1 liggen(0.95 is standaard))</param>
        /// <param name="ratioInOut">hoe veel de uiteindelijk videostream kleiner moetn worden ten opzichte van de 2 input videostreams (bij 1 word er niets aangepast), getal moet minimaal 1 zijn (1 is standaard)</param>
        /// <param name="verschilImages">hoe precies de pixels hetzelfde moeten zijn tussen 2 videostreams tijden het vergelijken (bij 0 moeten ze precies hetzelfde zijn), getal tussen 0 en 255 liggen (20 is standaard)</param>
        /// <param name="verschilBackground">hoe precies de pixels hetzelde moeten zijn tijdens het bepalen van de achtergrond van een videstream (bij 0 moeten ze precies hetzelfde zijn), getal tussen de 0 en 255 liggen (20 is standaard)</param>
        /// <param name="socket">de socket die gebruikt word bij het verzenden van de videostream (standaard is ip "localhost" en port 1234)</param>
        /// <param name="detectieVerschil">het maximale verschil dat er mag zijn tussen 2 pixels bij het detecteren van beweging (bij 0 moeten ze precies hetzelde zijn), getal tussen de 0 en de 255 (20 is standaard)</param>
        public ImageControl(int aantalKolomVergelijken, int aantalFramesVergelijken, int aantalPixelsBekijken, 
            float decPercentageHetzelfde, int ratioInOut, int verschilImages, int verschilBackground, TCPOut socket, 
            int detectieVerschil, TCPOut socketMotion, int intervalMotionCheck)
        {
            this.kolom = aantalKolomVergelijken;
            this.frames = aantalFramesVergelijken;
            this.pixels = aantalPixelsBekijken;
            this.percentage = decPercentageHetzelfde;
            this.ratio = ratioInOut;
            this.ver = verschilImages;
            this.verBack = verschilBackground;
            this.socket = socket;
            this.detVerschil = detectieVerschil;
            this.socketMotion = socketMotion;
            this.timer = new System.Timers.Timer(intervalMotionCheck);
            timer.Enabled = false;
            timer.Elapsed += new System.Timers.ElapsedEventHandler(timer_Elapsed);
        }

        void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (dataImage != null) 
                detector.BewegingZoeken(dataImage);
        }
        public ImageControl()
            : this(5, 20, 5, (float)0.95, 1, 15, 20, new TCPOut(), 20, new TCPOut("localhost", 1235), 2000) { }

        //methoden
        //video/webcam laden en achtergrond bepalen
        public bool VideoLadenEnAchtergrondBepalen(VideoFileSource videoLinks, VideoFileSource videoRechts)
        {
            try
            {
                //linkervideo
                video1 = new VideoInput(videoLinks, frames, pixels, verBack);
                while (!video1.BackgroundFound) { }
                imageLinks = video1.backGround;

                //rechtervideo
                video2 = new VideoInput(videoRechts, frames, pixels, verBack);
                while (!video2.BackgroundFound) { }
                imageRechts = video2.backGround;
    //            imageLinks = new Bitmap("c://Achtergrond_Links.bmp");
      //          imageRechts = new Bitmap("c://Achtergrond_Links.bmp");

                return true;
            }
            catch { return false; }
        }
        public bool WebcamLadenEnAchtergrondBepalen(CaptureDevice webcamLinks, CaptureDevice webcamRechts)
        {
            try
            {
                //linkerwebcam
                video1 = new VideoInput(webcamLinks, frames, pixels, verBack);
                while (!video1.BackgroundFound) { }
                imageLinks = video1.backGround;

                //rechterwebcam
                video2 = new VideoInput(webcamRechts, frames, pixels, verBack);
                while (!video2.BackgroundFound) { }
                imageRechts = video2.backGround;
               // imageRechts = new Bitmap("c://totaal.bmp");
                return true;
            }
            catch { return false; }
        }

        public bool WebcamLadenEnAchtergrondBepalen(CaptureDevice webcam, VideoFileSource video)//, CaptureDevice webcamRechts)
        {
            try
            {
                //linkerwebcam
                video1 = new VideoInput(webcam, frames, pixels, verBack);
                while (!video1.BackgroundFound) { }
                imageLinks = video1.backGround;

                //linkerwebcam
                video1 = new VideoInput(video, frames, pixels, verBack);
                while (!video1.BackgroundFound) { }
                imageLinks = video1.backGround;
                return true;
            }
            catch { return false; }
        }


        //todo: nu moet de hele kolom hetzelfde zijn: dus als 1 webcam iets hoger staat, vind die al niets
        public bool ImagesVergelijken()
        {
            //alleen frames samenvoegen als ze even hoog zijn
            if (imageRechts.Height != imageLinks.Height)
                return false;
            height = imageLinks.Height;

            //variabelen voor de rechter frame declareren
            int i, x = 0, y = 0, max = height * kolom;
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
                kolomRechts[i] = (imageRechts.GetPixel(x, y).R + imageRechts.GetPixel(x, y).G + imageRechts.GetPixel(x, y).B) / 3;
            }

            //door blijven zoeken naar het goeie strookje
            //of totdat de hele bitmap afgezocht is
            int zoeknr = 0; int temp;
            while (zoeknr - 1 < (imageLinks.Width - kolom))
            {
                //een kolom pixels van de linkerbitmap pakken
                x = imageLinks.Width - (kolom + zoeknr); y = 0;
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
                    kolomLinks[i] = (imageLinks.GetPixel(x, y).R + imageLinks.GetPixel(x, y).G + imageLinks.GetPixel(x, y).B) / 3;
                }

                //kolommen vergelijken
                int totaal = 0;
                for (i = 0; i < max; i++)
                {
                    temp = kolomLinks[i] - kolomRechts[i];
                    if (temp < 0) temp *= -1;
                    if (temp < ver)
                        totaal++;
                }

                //kijken hoeveel pixels overeenkomen, als het genoeg is, dan kunnen de frams samengevoegd worden
                float verschil = (float)totaal / (float)max;
                if (verschil >= percentage)
                {
                    //iets lopms doen maar het scheelt me wel tijd als het werkt
         //           if ((imageLinks.Width / 4).ToString().Contains(",")) //width is niet deelbaar door 4
           //             if ((imageLinks.Width / 4).ToString().

                    this.verschil = imageLinks.Width - (kolom + zoeknr);// + 2);
                    width = this.verschil + imageRechts.Width;
                    return true;
                }
                zoeknr += 4;
        //        zoeknr++;
            }
            //niets gevonden dus linkerimage gebruiken
            image = imageLinks;
            this.verschil = 0;
            width = imageLinks.Width;
            return false;
        }

        public bool NieuweImageInitialiseren()
        {
            try
            {
                //wat variabelen declareren voor de loops en if
                // /ratio als de gebruiker de uiteindelijk image kleiner wil hebben als het orgineel
                int tempx = 0, tempy = 0;
                int tempw = width / ratio;
                int temph = height / ratio;
                int tempv = verschil / ratio;
                image = new Bitmap(tempw, temph, PixelFormat.Format24bppRgb);
                for (int x = 0; x < tempw; x++)
                {
                    for (int y = 0; y < temph; y++)
                    {
                        if (x < tempv) //verschil) //< of <= maakt niet zoveel uit, het gedeelte word anders bij de else gepakt
                            image.SetPixel(x, y, imageLinks.GetPixel(x * ratio, y * ratio));
                        else
                        {
                            image.SetPixel(x, y, imageRechts.GetPixel(tempx * ratio, tempy * ratio));
                            tempy++;
                            if (tempy >= temph)
                            {
                                tempy = 0;
                                tempx++;
                            }
                        }
                    }
                }
                //de afbeelding in het geheugen zetten zodat het aangepast kan worden
                MemoryStream mstemp = new MemoryStream();
                image.Save(mstemp, ImageFormat.Bmp);
                mstemp.Flush();
                dataImage = mstemp.ToArray();
                mstemp.Close();

                //variabelen invullen voor het samenvoegen van de streams
                imLengte = width * height * 3 + 54;
                verschil *= 3;

                //test:
                imLengte += (width * 2);

                //alles terugzetten naar default en true retourneren
                imageLinks = imageRechts = null;  
                return true;
            }
            catch
            {
                return false;
            }
        }

        //de twee achtergrond samen voegen en 1 afbeelding ervan maken
        private byte[] ImagesSamenvoegen()
        {
            try
            {
                MemoryStream msL = new MemoryStream();
                imageLinks.Save(msL, ImageFormat.Bmp);
                msL.Flush();
                dataLinks = msL.ToArray();

                MemoryStream msR = new MemoryStream();
                imageRechts.Save(msR, ImageFormat.Bmp);
                msR.Flush();
                dataRechts = msR.ToArray();

                int totaal = (width * 3 + 2) * height + 54; //de totaal aantal benodigde bytes voor de bitmap (24bits)
                int i, x = 0, y = 0, xl = 54, xr = 54; //x,y de positie in de array, xl,xr is beginpositie in de array (54 ivm headers)
                int imgWdL = imageLinks.Width * 3; //lengte van een rij van linkerimage
                int imgWdR = imageRechts.Width * 3; //lengte van een rij van rechterimage
                int maxWidth = width * 3; //de width van de image van een rij (alleen van de rgb waardes)
                for (i = 53; i < totaal; i++, x++)
                {
                    //nieuwe regel
                    if (x > maxWidth)
                    {
                        xl = xr = 54;
                        x = 0;
                        y++;
                        dataImage[i++] = 0;
                        dataImage[i++] = 0;
                    }
                    //vergaan met huidige regel
                    else
                    {
                        //links laden
                        if (x <= verschil)
                            dataImage[i] = dataLinks[xl++ + (y * imgWdL)];
                        //rechts laden
                        else
                            dataImage[i] = dataRechts[xr++ + (y * imgWdR)];
                    }
                }
                //klaar dus alles weer terugzetten naar default en de data voor videostream retourneren
                imageLinks = imageRechts = null;
                return dataImage;
            }
            catch
            {
                return null;
            }
        }

        public bool StreamsSamenvoegen()
        {
            try
            {
                //verbinding maken, indien mislukt false retourneren
                if (!socket.Start())
                    return false;

                //motion detection
                MotionDetectionInitialiseren();
             //   timer.Enabled = true;

                //2 bitmaps samenvoegen (voor altijd blijven doen)
                video1.frame += new VideoInput.EventHandler(video1_frame);
                video2.frame += new VideoInput.EventHandler(video2_frame);

                //vliegt eruit als er iets mis gaat met verzenden of imagesSamenvoegen
                while (doorgaan)
                    if (imageRechts != null && imageLinks != null)
                    {
                        if (!socket.Verzenden(ImagesSamenvoegen()))
                            throw new Exception();
                    }
                if (video1 != null) video1.Close();
                if (video2 != null) video2.Close();
            }
            catch
            {
                //error: videstreams afsluiten en false retourneren
                if (video1 != null) video1.Close();
                if (video2 != null) video2.Close();
                return false;
            }
            //gebruiker heeft afgesloten en alles is goed gegaan, true retourneren
            return true;
        }

        //alleen een nieuwe frame accepteren als de oude verwerkt is?
        //alleen doen als het pogramma snel genoeg is...
        void video1_frame(Bitmap frame)
        {
            if (imageLinks == null)
                imageLinks = frame;
        }
        void video2_frame(Bitmap frame)
        {
            if (imageRechts==null)
                imageRechts = frame;
        }

        private void MotionDetectionInitialiseren()
        {
            detector = new MotionDetection(detVerschil, dataImage, (image.Width * 3 + 2), socketMotion);
        }

        //proporties
        public bool BlijfSamenvoegen { set { doorgaan = value; } }
        public TCPOut SocketStream { get { return socket; } }
        public TCPOut SocketMotion { get { return detector.Socket; } }
    }
}
