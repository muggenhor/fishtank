using System;
using System.Collections.Generic;
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
        private int hoogteKolom; //80 hoogte van een kolom bij vergelijken
        //standaard variabelen
        private Bitmap imageLinks=null, imageRechts=null, imageMidden = null, image = null; //de 4 afbeedlignen (3 vd webcams en 1 uiteindelijke)
        private VideoInput videoL = null, videoR = null, videoM = null; //de input van de webcams
        private byte[] dataImage = null, dataLinks=null, dataRechts=null, dataMidden=null; //de 4 dataArrys (streams) 3 vd webcams, 1 uiteindelijke
        private bool doorgaan=true, motionZoeken=false; //doorgaan: doorgaan met streamen, motionZoeken: beweging zoeken
        private TCPOut socket, socketMotion, sockFace; //de sockets voor het verzenden van de stream en coordinaten van de motion
        private MotionDetection detector; //de bewegingsdetector
        private System.Timers.Timer timer; //de timer voor de bewegingdetector
        private ShortSide kleineZijde; //de webcambeelden voor dekleine kant

        private int eindLinks, begMidden, eindMidden, begRechts; //de start/eind posities in de streams voor het samenveogen van de streams toto 1
        private int width = 0, height = 0, totaal, imgLen; // de grootte van het uiteindelijk afbeeldingen in aantal bytes (imgLen is de lengte van 1 rij van een stream van een webcam)

        //tijdelijke variabelen voor debug
        //tempverschillen: de verschillen tussen de beelden van 2 webcams (ligt tussen 0 en 1)
        private List<float> tempverschillenRM = null;
        private List<float> tempverschillenLM = null;

        //constructors
        /// <summary>
        /// De class die alles doet (het bepalen van de achtergronden, 
        /// het vergelijken, het samenvoegen en het verzenden (via socket)
        /// </summary>
        /// <param name="hoogteKolomVergelijken">de hoogte van die kolom die vergeleken moet worden (als je niet wilt dat de randen meegenomen moeten worden), getal moet tussen 0 en 100 zijn (80 is standaard)</param>
        /// <param name="aantalKolomVergelijken">het aantal kolommen dat vergeleken moet worden tussen de 2 videostreams (bij 1 word er maar 1 kolom vergeleken)), getal moet minmaal 1 zijn (5 is standaard)</param>
        /// <param name="aantalFramesVergelijken">het aantal frames dat van 1 videostream hetzelfde moet zijn om de achtergrond te bepalen (bij 1 word er maar naar 1 frame gekeken), getal moet minimaal 1 zijn (20 is standaard)</param>
        /// <param name="aantalPixelsBekijken">hoeveel pixels er worden overgeslagen met het bepalen van de achtergond (bij 1 worden alle pixels bekeken), moet minimaal 1 zijn (5 is standaard)</param>
        /// <param name="decPercentageHetzelfde">het percentage dat tussn de twee videostrams hetzelfde moet zijn om ze te kunnen samenvoegen (bij 1 moeten ze precies hetzelfde zijn), getal moet tussen 0 en 1 liggen(0.95 is standaard))</param>
        /// <param name="ratioInOut">hoe veel de uiteindelijk videostream kleiner moetn worden ten opzichte van de 2 input videostreams (bij 1 word er niets aangepast), getal moet minimaal 1 zijn (1 is standaard)</param>
        /// <param name="verschilImages">hoe precies de pixels hetzelfde moeten zijn tussen 2 videostreams tijden het vergelijken (bij 0 moeten ze precies hetzelfde zijn), getal tussen 0 en 255 liggen (20 is standaard)</param>
        /// <param name="verschilBackground">hoe precies de pixels hetzelde moeten zijn tijdens het bepalen van de achtergrond van een videstream (bij 0 moeten ze precies hetzelfde zijn), getal tussen de 0 en 255 liggen (20 is standaard)</param>
        /// <param name="socketStream">de socket die gebruikt word bij het verzenden van de videostream (standaard is ip "localhost" en port 7779)</param>
        /// <param name="socketMotion">de socket die gebruikt word bij het verzenden van de coordinaten van beweging (standaard is "localhost" en 7780) </param>
        /// <param name="detectieVerschil">het maximale verschil dat er mag zijn tussen 2 pixels bij het detecteren van beweging (bij 0 moeten ze precies hetzelde zijn), getal tussen de 0 en de 255 (20 is standaard)</param>
        /// <param name="socketGezichtsHerkenning">de socket die gebruikt word bij het verzenden van de coordianten van een gezicht (standaard is ip "localhost" en port 7781)</param>
        /// <param name="intervalMotionCheck">het aantal miliseconden dat er op beweging gekeken moet worden (500ms is standaard)</param>
        public ImageControl(int hoogteKolomVergelijken, int aantalKolomVergelijken, int aantalFramesVergelijken, int aantalPixelsBekijken, 
            float decPercentageHetzelfde, int ratioInOut, int verschilImages, int verschilBackground, TCPOut socketStream, TCPOut socketMotion,
            int detectieVerschil, int intervalMotionCheck, TCPOut socketGezichtsHerkenning)
        {
            this.hoogteKolom = hoogteKolomVergelijken;
            this.kolom = aantalKolomVergelijken;
            this.frames = aantalFramesVergelijken;
            this.pixels = aantalPixelsBekijken;
            this.percentage = decPercentageHetzelfde;
            this.ratio = ratioInOut;
            this.ver = verschilImages;
            this.verBack = verschilBackground;
            this.socket = socketStream;
            this.detVerschil = detectieVerschil;
            this.socketMotion = socketMotion;
            this.sockFace = socketGezichtsHerkenning;
            this.timer = new System.Timers.Timer(intervalMotionCheck);
            timer.Enabled = false;
            timer.Elapsed += new System.Timers.ElapsedEventHandler(timer_Elapsed);
        }

        void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            //alleen gaan zoeken als er nog niet word gezocht
            if (!motionZoeken)
            {
                motionZoeken = true;
                if (dataImage != null)
                {
                    byte[] data;
                    if ((data = detector.BewegingZoeken((byte[])dataImage.Clone())) != null)
                    {
                        detector.Socket.Verzenden(data);
              //          Console.WriteLine("Coordinaten:" + data[0] + ":" + data[1]);
                    }
            //        else
            //            Console.WriteLine("Geen beweging gedetecteerd");
                }
                motionZoeken = false;
            }
        }
        public ImageControl()
            : this(80, 5, 20, 5, (float)0.95, 1, 15, 20, new TCPOut("127.0.0.1", 7779), new TCPOut("127.0.0.1", 7780), 20, 500, new TCPOut("127.0.0.1", 7781)) { }

        //methoden
        //webcam voor de kleine zijde laden
        public bool WebcamVoorKleineZijdeLaden(CaptureDevice webcam, TCPOut socket)
        {
            try
            {
                kleineZijde = new ShortSide(webcam, socket.IpAddres, socket.Poort);
                return true;
            }
            catch
            {
                return false;
            }
        }


        //video/webcam laden en achtergrond bepalen
        public bool VideoLadenEnAchtergrondBepalen(VideoFileSource videoLinks, VideoFileSource videoRechts, VideoFileSource videoMidden)
        {
     //       try
            {
                //linkervideo
                videoL = new VideoInput(videoLinks, frames, pixels, verBack);
                while (!videoL.BackgroundFound) { }
                imageLinks = videoL.backGround;
                Console.WriteLine("links gevonden");

                //middenvideo
                videoM = new VideoInput(videoMidden, frames, pixels, verBack);
                while (!videoM.BackgroundFound) { }
                imageMidden = videoM.backGround;
                Console.WriteLine("midden gevonden");

                //rechtervideo
                videoR = new VideoInput(videoRechts, frames, pixels, verBack);
                while (!videoR.BackgroundFound) { }
                imageRechts = videoR.backGround;
                Console.WriteLine("rechts gevonden");
    //            imageLinks = new Bitmap("c://Achtergrond_Links.bmp");
      //          imageRechts = new Bitmap("c://Achtergrond_Links.bmp");


                //hoogte van image bepalen
                height = imageMidden.Height;
                return true;
            }
      //      catch { return false; }
        }
        public bool WebcamLadenEnAchtergrondBepalen(CaptureDevice webcamLinks, CaptureDevice webcamRechts, CaptureDevice webcamMidden)
        {
         //   try
            {
                //linkerwebcam
                videoL = new VideoInput(webcamLinks, frames, pixels, verBack);
                while (!videoL.BackgroundFound) { }
                imageLinks = videoL.backGround;
                imageLinks.Save("c://achtergrond_links.bmp");
                Console.WriteLine(" Achtergrond gevonden van linkerwebcam,");
                Console.WriteLine("  afbeelding opgeslagen in c://achtergrond_links.bmp");
                //middenwebcam
                videoM = new VideoInput(webcamMidden, frames, pixels, verBack);
                while (!videoM.BackgroundFound) { }
                imageMidden = videoM.backGround;
                imageMidden.Save("c://achtergrond_midden.bmp");
                Console.WriteLine(" Achtergrond gevonden van middenwebcam,");
                Console.WriteLine("  afbeelding opgeslagen in c://achtergrond_midden.bmp");
                //rechterwebcam
                videoR = new VideoInput(webcamRechts, frames, pixels, verBack);
                while (!videoR.BackgroundFound) { }
                imageRechts = videoR.backGround;
                imageRechts.Save("c://achtergrond_rechts.bmp");
                Console.WriteLine(" Achtergrond gevonden van rechterwebcam,");
                Console.WriteLine("  afbeelding opgeslagen in c://achtergrond_rechts.bmp");

                //hoogte van image bepalen
                height = imageMidden.Height;
                return true;
            }
        //    catch { return false; }
        }

        public bool ImagesVergelijkenLinksMidden()
        {
            tempverschillenLM = new List<float>();

            //alleen frames samenvoegen als ze even hoog zijn (bij camera's is dit altijd het geval)
            if (imageMidden.Height != imageLinks.Height)
                return false;

            //variabelen voor de midden frame declareren
            int i, x = 0, y = 0;
            int max = (int)((double)height * (double)kolom * ((double)hoogteKolom / (double)100));
            int[] kolomMidden;
            kolomMidden = new int[max];

            //variabelen van de linker frame declareren
            int[] kolomLinks;
            kolomLinks = new int[max];

            int boven = (int)(height * (double)((100 - hoogteKolom) / 2 ) / (double)100);
            int onder = height - boven;
            //data van de rechter frame invullen
            for (i = boven; i < max; i++, y++)
            {
                if (y == onder)
                {
                    x++;
                    y = 0;
                }
                kolomMidden[i] = (imageMidden.GetPixel(x, y).R + imageMidden.GetPixel(x, y).G + imageMidden.GetPixel(x, y).B) / 3;
            }

            //door blijven zoeken naar het goeie strookje
            //of totdat de hele bitmap afgezocht is
            int zoeknr = 0; int temp;
            bool links = true;
            while (zoeknr - 1 < (imageLinks.Width - kolom))
            {
                if (links) //linkerstukje verwijderen
                {
                    //een kolom pixels van de linkerbitmap pakken
                    x = zoeknr + kolom; y = 0;

                    //data van de linker frame laden
                    //de linkerkant spiegelen ivm de verschillende hoeken tov de plaatsting van de webcams
                    for (i = boven; i < max; i++, y++)
                    {
                        if (y == onder)
                        {
                            x--;
                            y = 0;
                        }
                        kolomLinks[i] = (imageLinks.GetPixel(x, y).R + imageLinks.GetPixel(x, y).G + imageLinks.GetPixel(x, y).B) / 3;
                    }

                }
                else //rechterstukje verwijderen
                {
                    x = zoeknr; y = 0;

                    //data van de rechter frame invullen
                    for (i = boven; i < max; i++, y++)
                    {
                        if (y == onder)
                        {
                            x++;
                            y = 0;
                        }
                        kolomMidden[i] = (imageMidden.GetPixel(x, y).R + imageMidden.GetPixel(x, y).G + imageMidden.GetPixel(x, y).B) / 3;
                    }
                }

                //'beurt' omwisselen (zodat er van beide frames een stukje weggehaald word)
                links = !links;

                //kolommen vergelijken
                int totaal = 0;
                for (i = 0; i < max; i++)
                {
                    temp = kolomLinks[i] - kolomMidden[i];
                    if (temp < 0) temp = -temp;
                    if (temp < ver)
                        totaal++;
                }

                //kijken hoeveel pixels overeenkomen, als het genoeg is, dan kunnen de frams samengevoegd worden
                float verschil = (float)totaal / (float)max;
                tempverschillenLM.Add(verschil);
                if (verschil >= percentage)
                {
                    eindLinks = imageLinks.Width - (zoeknr / 2);
                    begMidden = zoeknr / 2;
                    return true;
                }
                //zoeknr nu alleen verhogen als links ne rechts geprobeert is
                if (!links)
                    zoeknr += 4;
            }
            //niets gevonden dus linkerimage gebruiken
            image = imageLinks;
            width = imageLinks.Width;
            return false;
        }

        //todo: nu moet de hele kolom hetzelfde zijn: dus als 1 webcam iets hoger staat, vind die al niets
        //links en rechts war eraf snijden, om de beeldkwaliteit hoog te houden
        public bool ImagesVergelijkenRechtsMidden()
        {
            tempverschillenRM = new List<float>();

            //TODO: uiteindelijk mag dit weg (dan hoven de kolommen niet meer evenlang te zijn)
            //alleen frames samenvoegen als ze even hoog zijn
            if (imageRechts.Height != imageMidden.Height)
                return false;

            //variabelen voor de rechter frame declareren
            int i, x = 0, y = 0;
            int max = (int)((double)height * (double)kolom * ((double)hoogteKolom / (double)100));
            int[] kolomRechts;
            kolomRechts = new int[max];

            //variabelen van de midden frame declareren
            int[] kolomMidden;
            kolomMidden = new int[max];

            int boven = (int)(height * (double)((100 - hoogteKolom) / 2 ) / (double)100);
            int onder = height - boven;
            //data van de rechter frame invullen
            for (i = boven; i < max; i++, y++)
            {
                if (y == onder)
                {
                    x++;
                    y = 0;
                }
                kolomRechts[i] = (imageRechts.GetPixel(x, y).R + imageRechts.GetPixel(x, y).G + imageRechts.GetPixel(x, y).B) / 3;
            }

            //door blijven zoeken naar het goeie strookje
            //of totdat de hele bitmap afgezocht is
            int zoeknr = 0; int temp;
            bool midden = true;
            while (zoeknr - 1 < (imageMidden.Width - kolom)) //TODO: AANPASSEN?
            {
                 if (midden) //middenstukje verwijderen
                 {
                        //een kolom pixels van de linkerbitmap pakken
                       // x = imageLinks.Width - (kolom + zoeknr); y = 0;
                        x = zoeknr+kolom; y = 0;


                        //data van de linker frame laden
                        //de linkerkant spiegelen ivm de verschillende hoeken tov de plaatsting van de webcams
                        for (i = boven; i < max; i++, y++)
                        {
                            if (y == onder)
                            {
                                x--;
                                y = 0;
                            }
                            kolomMidden[i] = (imageMidden.GetPixel(x, y).R + imageMidden.GetPixel(x, y).G + imageMidden.GetPixel(x, y).B) / 3;
                        }

                    }
                    else //rechterstukje verwijderen
                    {
                        x = zoeknr; y = 0;

                        //data van de rechter frame invullen
                        for (i = boven; i < max; i++, y++)
                        {
                            if (y == onder)
                            {
                                x++;
                                y = 0;
                            }
                            kolomRechts[i] = (imageRechts.GetPixel(x, y).R + imageRechts.GetPixel(x, y).G + imageRechts.GetPixel(x, y).B) / 3;
                        }
                    }

                    //'beurt' omwisselen (zodat er van beide frames een stukje weggehaald word)
                    midden = !midden;

                    //kolommen vergelijken
                    int totaal = 0;
                    for (i = 0; i < max; i++)
                    {
                        temp = kolomMidden[i] - kolomRechts[i];
                        if (temp < 0) temp = -temp;
                        if (temp < ver)
                            totaal++;
                    }

                    //kijken hoeveel pixels overeenkomen, als het genoeg is, dan kunnen de frams samengevoegd worden
                    float verschil = (float)totaal / (float)max;
                    tempverschillenRM.Add(verschil);
                    if (verschil >= percentage)
                    {
                        //NIEUWE CODE VOOR MIDDEN-RECHTS
                        eindMidden = imageMidden.Width - (zoeknr / 2);
                        begRechts = zoeknr / 2;
                        
                        //startRechts = zoeknr / 2; //vanuit welke positie de rechterimage 'gemerged' moet worden
                        //this.verschil = imageMidden.Width - (zoeknr / 2); // /2 wat andere kant van rechterframe //EERST (KOLOM + ZOEKNR / 2)
                        //width = imageMidden.Width + imageRechts.Width - zoeknr; //EERST imageRechts.Width + imageLinks.Width - (kolom + zoeknr);
                        return true;
                    }
       //         }
                //zoeknr nu alleen verhogen als midden en rechts geprobeert is
                if (!midden) 
                        zoeknr += 4;
            }
            //niets gevonden dus linkerimage gebruiken
    /*        image = imageLinks;
            this.verschil = 0;
            width = imageLinks.Width;*/
            return false;
        }

        public bool NieuweImageInitialiseren()
        {
        //    try
            {
                //totale lengte van samengevoegde image (hoogte is al gedaan)
                width = eindLinks + eindMidden - begMidden + imageRechts.Width - begRechts;

                image = new Bitmap(width, height, PixelFormat.Format24bppRgb);
                int xR=begRechts, xM=begMidden, xL=0;
                int xRM=eindLinks+eindMidden-begMidden;
                //voor elke x coordinaat
                for (int y=0; y<height; y++)
                {
                    //voor elke y coordinaat
                    for (int x=0; x<width; x++)
                    {
                        //linkerimage
                        if (x < eindLinks)
                        {
                            image.SetPixel(x,y, imageLinks.GetPixel(xL++,y));
                        }
                        //middenimage
                        else if (x < xRM)
                        {
                            image.SetPixel(x,y, imageMidden.GetPixel(xM++,y));
                        }
                        //rechterimage
                        else
                        {
                            image.SetPixel(x,y, imageRechts.GetPixel(xR++,y));
                        }
                    }
                    xL = 0;
                    xR = begRechts;
                    xM = begMidden;
                }

                //de afbeelding in het geheugen zetten zodat het aangepast kan worden
                MemoryStream mstemp = new MemoryStream();
                image.Save(mstemp, ImageFormat.Bmp);
                mstemp.Flush();
                dataImage = mstemp.ToArray();
                mstemp.Close();

                //variabelen invullen voor het samenvoegen van de streams
                eindLinks *= 3;
                begMidden *= 3;
                eindMidden *= 3;
                begRechts *= 3;

                width *= 3;
          //      width = (width + 2) * 3; //aangepast
          //      height *= 3;
                totaal = width * height + 54;
                imgLen = imageMidden.Width * 3;

                //alles terugzetten naar default en true retourneren
                Bitmap.FromStream(new MemoryStream(dataImage)).Save("c://achtergrond.bmp");
                imageMidden = imageLinks = imageRechts = null;

                return true;
                //wat variabelen declareren voor de loops en if
                // /ratio als de gebruiker de uiteindelijk image kleiner wil hebben als het orgineel
    /*            int tempx = 0, tempy = 0;
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
                            //TODO: kijken of startRechts klopt !!! 
                            image.SetPixel(x, y, imageRechts.GetPixel(tempx * ratio + startRechts, tempy * ratio));
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
                  Bitmap.FromStream(new MemoryStream(dataImage)).Save("c://achtergrond.bmp");
                imageLinks = imageRechts = null;
                return true;
            }
   //         catch
            {
                return false;*/
            }
        }

        //de twee achtergrond samen voegen en 1 afbeelding ervan maken
        private byte[] ImagesSamenvoegen()
        {
       //     try
            {
                MemoryStream msL = new MemoryStream();
                imageLinks.Save(msL, ImageFormat.Bmp);
                msL.Flush();
                dataLinks = msL.ToArray();
                msL.Close();

                MemoryStream msR = new MemoryStream();
                imageRechts.Save(msR, ImageFormat.Bmp);
                msR.Flush();
                dataRechts = msR.ToArray();
                msR.Close();

                MemoryStream msM = new MemoryStream();
                imageMidden.Save(msM, ImageFormat.Bmp);
                msM.Flush();
                dataMidden = msM.ToArray();
                msM.Close();

                int x=0, xL=54, xR=54 + begRechts, xM=54 + begMidden, y=0;
                int xRM=eindLinks+eindMidden-begMidden;

                for (int i = 53; i < totaal; i++, x++)
                {
                    //nieuwe regel
                    if (x >= width-2) //eerst alleen >
                    {
                        x = 0;
                        xL = xR = xM = 54;
                        y++;

                        dataImage[i++] = 0;
                        dataImage[i++] = 0;
                    }
                    else
                    {
                        //linkerImage
                        if (x <= eindLinks)
                        {
                            dataImage[i] = dataLinks[xL++ + (y * imgLen)];
                        }
                        //middenImage
                        else if (x <= xRM)
                        {
                            dataImage[i] = dataMidden[xM++ + (y * imgLen)]; //HET KAN ZO ZIJN DAT ER BIJ ELEKE LENGTE NOG +2 GEDAAN MOET WORDEN !!!
                        }
                        //rechterImage
                        else
                        {
                            dataImage[i] = dataRechts[xR++ + (y * imgLen)];
                        }
                    }
                }

                //klaar dus alles weer terugzetten naar default en de data voor videostream retourneren
                Bitmap.FromStream(new MemoryStream(dataImage)).Save("c://achtergrondStream.bmp");
                imageLinks = imageRechts = imageMidden = null;
                return dataImage;
            }
  //          catch
            {
                return null;
            }
        }

        public bool StreamsSamenvoegen()
        {
            try
            {
                //TIJDELIJK
     //           sockFace = new TCPOut("127.0.0.1", 1234);
      //          sockFace.Start();

                //verbinding maken, indien mislukt false retourneren
                if (!socket.Start())
                    return false;

                //motion detection
    //            MotionDetectionInitialiseren();
      //          timer.Enabled = true;

                //webcam voor kleine zijde streamen
      //          kleineZijde.Start();

                //2 bitmaps samenvoegen (voor altijd blijven doen)
                videoL.frame += new VideoInput.EventHandler(videoL_frame);
                videoR.frame += new VideoInput.EventHandler(videoR_frame);
                videoM.frame += new VideoInput.EventHandler(videoM_frame);
                
                //vliegt eruit als er iets mis gaat met verzenden of imagesSamenvoegen
                while (doorgaan)
                    if (imageRechts != null && imageLinks != null && imageMidden != null)
                        if (!socket.Verzenden(ImagesSamenvoegen()))
                            throw new Exception("Error met stream samenvoegen of verzenden");
                if (videoL != null) videoL.Close();
                if (videoR != null) videoR.Close();
            }
            catch
            {
                //error: videstreams afsluiten en false retourneren
                if (videoL != null) videoL.Close();
                if (videoR != null) videoR.Close();
                return false;
            }
            //gebruiker heeft afgesloten en alles is goed gegaan, true retourneren
            return true;
        }     

        //alleen een nieuwe frame accepteren als de oude verwerkt is?
        //alleen doen als het pogramma snel genoeg is...
        void videoL_frame(Bitmap frame)
        {
            if (imageLinks == null)
                imageLinks = frame;
        }
        void videoR_frame(Bitmap frame)
        {
            if (imageRechts == null)
                imageRechts = frame;
        }
        void videoM_frame(Bitmap frame)
        {
            if (imageMidden == null)
                imageMidden = frame;
        }

        private void MotionDetectionInitialiseren()
        {
            detector = new MotionDetection(detVerschil, (image.Width * 3 + 2), (byte[])dataImage.Clone(), socketMotion);
        }

        //proporties
        public Bitmap Frame { get { return image; }  set { image = value;} }
        public bool BlijfSamenvoegen { set { doorgaan = value; } }
        public TCPOut SocketStreamKortZijde { get { return kleineZijde.Socket; } }
        public TCPOut SocketStreamLangeZijde { get { return socket; } }
        public TCPOut SocketMotion { get { return detector.Socket; } }
        public TCPOut SOcketGezichtHerkenning { get { return sockFace; } }
        public bool StreamsAfsluiten
        {
            get
            {
                try
                {
                    videoM.Close();
                    videoL.Close();
                    videoR.Close();
                    return kleineZijde.Stop;
                }
                catch
                {
                    return false;
                }
            }
        }
    }
}
