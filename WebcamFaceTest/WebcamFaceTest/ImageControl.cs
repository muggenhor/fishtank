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
        private byte[] dataImage = null, dataLinks=null, dataRechts=null, dataMidden=null, dataImagebackup=null;//de 4 dataArrys (streams) 3 vd webcams, 1 uiteindelijke
        private bool doorgaan=true, motionZoeken=false; //doorgaan: doorgaan met streamen, motionZoeken: beweging zoeken
        private TCPOut socket, socketMotion, sockFace; //de sockets voor het verzenden van de stream en coordinaten van de motion
        private MotionDetection detector; //de bewegingsdetector
        private System.Timers.Timer timer, timerGC; //de timer voor de bewegingdetector en garbagecollector
        private ShortSide kleineZijde; //de webcambeelden voor dekleine kant
        private face gezichtHerk=null;
        private int IVboven = 0, IVonder = 0;
        private int Alinks, Arechts, Aboven, Aonder;
        private int Mlinks, Mrechts, Mboven, Monder;
        private int eindLinks, begMidden, eindMidden, begRechts; //de start/eind posities in de streams voor het samenveogen van de streams tot 1
        private int width = 0, height = 0, totaal, imgLen; // de grootte van het uiteindelijk afbeeldingen in aantal bytes (imgLen is de lengte van 1 rij van een stream van een webcam)

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
            int detectieVerschil, int intervalMotionCheck, TCPOut socketGezichtsHerkenning, int intervalFaceCheck,
            int links, int rechts, int boven, int onder, int Mlinks, int Mrechts, int Mboven, int Monder)
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
            this.Aboven = boven;
            this.Alinks = links;
            this.Aonder = onder;
            this.Arechts = rechts;
            this.Mboven = Mboven;
            this.Mlinks = Mlinks;
            this.Monder = Monder;
            this.Mrechts = Mrechts;
            this.timer = new System.Timers.Timer(intervalMotionCheck);
            timer.Enabled = false;
            timer.Elapsed += new System.Timers.ElapsedEventHandler(timer_Elapsed);
            timerGC = new System.Timers.Timer(1000);
            timerGC.Enabled = true;
            timerGC.Elapsed += new System.Timers.ElapsedEventHandler(timerGC_Elapsed);           
        }

        //een standaard imageControl, oorspronkelijk voor avi bestanden
        public ImageControl()
            : this(80, 5, 20, 5, (float)0.95, 1, 15, 20, new TCPOut("127.0.0.1", 7779), new TCPOut("127.0.0.1", 7780),
                20, 500, new TCPOut("127.0.0.1", 7781), 100, 100, 100, 100, 100, 100, 100, 100, 100) { }

/*EVENTS*/
        //alleen een nieuwe frame accepteren als de oude verwerkt is?
        //alleen doen als het pogramma snel genoeg is...
        void videoL_frame(Bitmap frame)
        {
            //indien stream is samengevoegd, nieuwe frame accepteren
            if (imageLinks == null)
                imageLinks = frame;
            //alleen naar gezichten zoeken in het webcambeelden indien het geeist is
            if (gezichtHerk.Webcam == 0)
                gezichtHerk.newFrame(frame);
        }
        void videoM_frame(Bitmap frame)
        {
            if (imageRechts == null)
                imageRechts = frame;
            if (gezichtHerk.Webcam == 1)
                gezichtHerk.newFrame(frame);
        }
        void videoR_frame(Bitmap frame)
        {
            if (imageMidden == null)
                imageMidden = frame;
            if (gezichtHerk.Webcam == 2)
                gezichtHerk.newFrame(frame);
        }

        //garbage collecteren (omdat er soms geheugen niet vrijgemaakt word)
        void timerGC_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            System.GC.Collect();
        }

        //timer voor beweging zoeken en tevens ook naar beweging zoeken
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
                        Console.WriteLine("Beweging op: " + (int)data[0] + ":" + (int)data[1]);
                    }
                }
                motionZoeken = false;
            }
        }

/*ALGEMENE METHODEN*/
        private int KolommenVergelijken(int max, int[] links, int[] rechts)
        {
            //kolommen vergelijken
            int totaal = 0, temp =0;
            for (int i = 0; i < max; i++)
            {
                temp = links[i] - rechts[i];
                if (temp < 0) temp = -temp;
                if (temp < ver)
                    totaal++;
            }
            return totaal;
        }

        private void StramsAfsluiten()
        {
            if (videoL != null) videoL.Close();
            if (videoR != null) videoR.Close();
            if (videoM != null) videoM.Close();
        }

        private bool StreamInit()
        {
            try
            {
                //verbinding maken, indien mislukt false retourneren
                if (!socket.Start())
                    throw new Exception("GEEN VERBINDING MET HET AQUARIUM");

                //motion detection
                MotionDetectionInitialiseren();
                timer.Enabled = true;

                //gezichts herkenning opstarten, zowel timer als socket
                gezichtHerk = new face(sockFace);

                //webcam voor kleine zijde streamen
                kleineZijde.Start(Alinks, Arechts, Aonder, Aboven);

                //3 bitmaps samenvoegen (voor altijd blijven doen)
                videoL.frame += new VideoInput.EventHandler(videoL_frame);
                videoR.frame += new VideoInput.EventHandler(videoR_frame);
                videoM.frame += new VideoInput.EventHandler(videoM_frame);

                //alles gelukt, true retourneren
                return true;
            }
            catch { return false; }
        }

        private void MotionDetectionInitialiseren()
        {
            detector = new MotionDetection(detVerschil, (image.Width * 3 + 2), (byte[])dataImage.Clone(), socketMotion);
        }

        //als er niet automatisch een vergelijking tussen de videstreams gevonden 
        //kan worden kan het alsnog met de hand gedaan worden
        public void ScheidingHardwarematig(int zoeknr)
        {
            eindLinks = imageLinks.Width - (zoeknr / 2);
            begRechts = zoeknr / 2;
        }
    
        //webcam voor de kleine zijde laden
        public bool WebcamVoorKleineZijdeLaden(CaptureDevice webcam, TCPOut socket)
        {
            try { kleineZijde = new ShortSide(webcam, socket.IpAddres, socket.Poort);
                return true; }
            catch { return false; }
        }

        //video laden en achtergrond bepalen
        //(is voor testen bedoeld als er momenteel 
        public bool VideoLadenEnAchtergrondBepalen(VideoFileSource videoLinks, VideoFileSource videoRechts, VideoFileSource videoMidden)
        {
            try
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

                //hoogte van image bepalen
                height = imageMidden.Height;
                return true;
            }
            catch { return false; }
        }

        //webcam laden en achergrond laden
        public bool WebcamLadenEnAchtergrondBepalen(CaptureDevice webcamLinks, CaptureDevice webcamRechts, CaptureDevice webcamMidden)
        {
            try
            {
                //linkerwebcam
                videoL = new VideoInput(webcamLinks, frames, pixels, verBack);
                while (!videoL.BackgroundFound) { }
                imageLinks = videoL.backGround;
                imageLinks.Save("c://achtergrond_links.bmp");
                Console.WriteLine("  Achtergrond gevonden van linkerwebcam,");
                Console.WriteLine("   afbeelding opgeslagen in c://achtergrond_links.bmp");
                //middenwebcam
                videoM = new VideoInput(webcamMidden, frames, pixels, verBack);
                while (!videoM.BackgroundFound) { }
                imageMidden = videoM.backGround;
                imageMidden.Save("c://achtergrond_midden.bmp");
                Console.WriteLine("  Achtergrond gevonden van middenwebcam,");
                Console.WriteLine("   afbeelding opgeslagen in c://achtergrond_midden.bmp");
                //rechterwebcam
                videoR = new VideoInput(webcamRechts, frames, pixels, verBack);
                while (!videoR.BackgroundFound) { }
                imageRechts = videoR.backGround;
                imageRechts.Save("c://achtergrond_rechts.bmp");
                Console.WriteLine("  Achtergrond gevonden van rechterwebcam,");
                Console.WriteLine("   afbeelding opgeslagen in c://achtergrond_rechts.bmp");

                //hoogte van image bepalen
                height = imageMidden.Height;
                return true;
            }
            catch { return false; }
        }

        /// <summary>
        /// code : LM: links-midden, RM: rechts-midden, LR: Links-rechts
        /// </summary>
        private bool ImagesVergelijken(string code, Bitmap bitLinks, Bitmap bitRechts)
        {
            //variabelen
            //totale grootte van een bitmap
            int max = (int)((double)height * (double)kolom * ((double)hoogteKolom / (double)100));
            //de rgb waardes van de twee images
            int[] kolomRechts = null;
            int[] kolomLinks = null;
            //de afmetingen van de bitmaps
            IVboven = (int)(height * (double)((100 - hoogteKolom) / 2) / (double)100);
            IVonder = height - IVboven;
            //'stappen' van het zoeken
            int zoeknr = 0;
            bool wissel = true;

            //data van de rechter frame invullen
            kolomRechts = KolomVullen(true, max, bitRechts, zoeknr);

            //het 'echte zoeken'
            while (zoeknr - 1 < (bitLinks.Width - kolom))
            {
                //stukjes van de afbeeldingen verwijderen
                if (wissel) //data van de linker frame invullen
                    kolomLinks = KolomVullen(false, max, bitLinks, (zoeknr + kolom));
                else //data van de rechter frame invullen 
                    kolomRechts = KolomVullen(true, max, bitRechts, zoeknr);

                //beurt omdraaien
                wissel = !wissel;

                //kijken of er genoeg overeenkomt, zo ja: variabelen goed zetten en returnen
                if (((float)KolommenVergelijken(max, kolomLinks, kolomRechts) / (float)max) >= percentage)
                {
                    switch (code)
                    {
                        case "LM":
                            eindLinks = bitLinks.Width - zoeknr;
                            begMidden = zoeknr / 2;
                            break;
                        case "LR":
                            eindLinks = bitLinks.Width - (zoeknr / 2);
                            begRechts = zoeknr / 2;
                            break;
                        case "RM":
                            eindMidden = bitRechts.Width - (zoeknr / 2);
                            begRechts = zoeknr / 2;
                            break;
                        default:
                            throw new Exception("VERKEERDE CODE INGEVOERD BIJ IMAGESVERGELIJKEN");
                    }
                    return true;
                }

                //zoeknr nu alleen verhogen als midden en rechts geprobeert is
                if (!wissel)
                    zoeknr += 4;
            }

            //geen vergelijkingen gevonden
            return false;
        }

        private int[] KolomVullen(bool rechts, int max, Bitmap afbeelding, int x)
        {
            int y=0;
            int[] kolom = new int[max];
            for (int i = IVboven; i < max; i++, y++)
            {
                if (y == IVonder)
                {
                    if (rechts) x++;
                    else x--;
                    y = 0;
                }
                kolom[i] = (afbeelding.GetPixel(x, y).R + afbeelding.GetPixel(x, y).G + afbeelding.GetPixel(x, y).B) / 3;
            }
            return kolom;
        }

/*METHODEN VOOR 2 WEBCAMS*/
        //images vergelijken voor het bepalen van de stream met de webcams Links en Rechts (2 webcams gebruiken)
        public bool ImagesVergelijkenRechtsLinks()
        {
            //alleen frames samenvoegen als ze even hoog zijn
            if (imageRechts.Height != imageLinks.Height)
                return false;

            return ImagesVergelijken("LR", imageLinks, imageRechts);
        }

        public bool NieuweImageInitialiseren()
        {
            try
            {
                //totale lengte van samengevoegde image (hoogte is al gedaan)
                width = eindLinks + eindMidden - begMidden + imageRechts.Width - begRechts;

                image = new Bitmap(width, height, PixelFormat.Format24bppRgb);
                int xR = begRechts, xM = begMidden, xL = 0;
                int xRM = eindLinks + eindMidden - begMidden;
                //voor elke x coordinaat
                for (int y = 0; y < height; y++)
                {
                    //voor elke y coordinaat
                    for (int x = 0; x < width; x++)
                    {
                        //linkerimage
                        if (x < eindLinks)
                        {
                            image.SetPixel(x, y, imageLinks.GetPixel(xL++, y));
                        }
                        //middenimage
                        else if (x < xRM)
                        {
                            image.SetPixel(x, y, imageMidden.GetPixel(xM++, y));
                        }
                        //rechterimage
                        else
                        {
                            image.SetPixel(x, y, imageRechts.GetPixel(xR++, y));
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
                totaal = width * height + 54;
                imgLen = imageMidden.Width * 3;

                //alles terugzetten naar default en true retourneren
                Bitmap.FromStream(new MemoryStream(dataImage)).Save("c://achtergrond.bmp");
                imageMidden = imageLinks = imageRechts = null;

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

                int x = 0, xL = 54, xR = 54 + begRechts, xM = 54 + begMidden, y = 0;
                int xRM = eindLinks + eindMidden - begMidden;

                for (int i = 53; i < totaal; i++, x++)
                {
                    //nieuwe regel
                    if (x >= width - 2)
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
                            dataImage[i] = dataLinks[xL++ + (y * imgLen)];
                        //middenImage
                        else if (x <= xRM)
                            dataImage[i] = dataMidden[xM++ + (y * imgLen)];
                        //rechterImage
                        else
                            dataImage[i] = dataRechts[xR++ + (y * imgLen)];
                    }
                }

                //klaar dus alles weer terugzetten naar default en de data voor videostream retourneren
                imageLinks = imageRechts = imageMidden = null;
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
                //alles initialiseren
                if (!StreamInit())
                    throw new Exception("FOUT BIJ HET INITIALISEREN VAN DE STREAMS");

                //vliegt eruit als er iets mis gaat met verzenden of imagesSamenvoegen
                while (doorgaan)
                    if (imageRechts != null && imageLinks != null && imageMidden != null)
                        if (!socket.Verzenden(ImagesSamenvoegen()))
                            throw new Exception("Error met stream samenvoegen of verzenden");
                
                //streams afluiten (webcams 'ontkoppelen')
                StramsAfsluiten();
                //gebruiker heeft afgesloten en alles is goed gegaan, true retourneren
                return true;
            }
            catch
            {
                //streams afluiten (webcams 'ontkoppelen')
                StramsAfsluiten();
                return false;
            }
        }     

/*METHODEN VOOR 3 WEBCAMS*/
        //images vergelijken voor het bepalen van de stream met de webcams Links en Midden
        public bool ImagesVergelijkenLinksMidden()
        {
            //alleen frames samenvoegen als ze even hoog zijn (bij camera's is dit altijd het geval)
            if (imageMidden.Height != imageLinks.Height)
                return false;

            return ImagesVergelijken("LM", imageLinks, imageMidden);
        }

        //images vergelijken voor het bepalen van de stream met de webcams Links en Midden
        public bool ImagesVergelijkenRechtsMidden()
        {
            //alleen frames samenvoegen als ze even hoog zijn
            if (imageRechts.Height != imageMidden.Height)
                return false;

            return ImagesVergelijken("RM", imageMidden, imageRechts);
        }

        public bool NieuweImageInitialiseren2()
        {
            height = imageRechts.Height - Mboven - Monder;
            width = eindLinks + imageRechts.Width - begRechts - Mlinks - Mrechts;

            image = new Bitmap(width, height, PixelFormat.Format24bppRgb);
            int xR = begRechts, xL = Mlinks;
            //voor elke x coordinaat
            for (int y = 0; y < height; y++)
            {
                //voor elke y coordinaat
                for (int x = 0; x < width; x++)
                {
                    //linkerimage
                    if (x < (eindLinks - Mlinks))
                        image.SetPixel(x, y, imageLinks.GetPixel(xL++, y));
                    //rechterimage
                    else
                        image.SetPixel(x, y, imageRechts.GetPixel(xR++, y));
                }
                xL = Mlinks;
                xR = begRechts;
            }

            //de afbeelding in het geheugen zetten zodat het aangepast kan worden
            MemoryStream mstemp = new MemoryStream();
            image.Save(mstemp, ImageFormat.Bmp);
            mstemp.Flush();
            dataImage = mstemp.ToArray();
            mstemp.Close();

            //variabelen invullen voor het samenvoegen van de streams
            eindLinks *= 3;
            begRechts *= 3;
            Mlinks *= 3;
            width *= 3;
            width -= 2;
            totaal = width * height + 54;
            imgLen = imageLinks.Width * 3;

            //alles terugzetten naar default en true retourneren
            Bitmap.FromStream(new MemoryStream(dataImage)).Save("c://achtergrond.bmp");
            imageLinks = imageRechts = null;
            return true;
        }

        //de twee achtergrond samen voegen en 1 afbeelding ervan maken
        private byte[] ImagesSamenvoegen2()
        {
            try
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

                int x = 0, xL = 54 + Mlinks, xR = 54 + begRechts, y = Monder, totlinks = eindLinks - Mlinks;

                for (int i = 53; i < totaal; i++, x++)
                {
                    //nieuwe regel
                    if (x >= width)
                    {
                        x = 0; y++;
                        xL = 54 + Mlinks;
                        xR = 54 + begRechts;

                        dataImage[i++] = 0;
                        dataImage[i++] = 0;
                    }
                    else
                    {
                        //linkerImage
                        if (x <= totlinks)
                            dataImage[i] = dataLinks[xL++ + (y * imgLen)];
                        //rechterImage
                        else
                            dataImage[i] = dataRechts[xR++ + (y * imgLen)];
                    }
                }

                //klaar dus alles weer terugzetten naar default en de data voor videostream retourneren
                imageLinks = imageRechts = null;
                dataImagebackup = dataImage; //soms blijft die ergens hangen, hierdoor word de oude videostream doorgestuurd
                return dataImage;
            }
            catch { return dataImagebackup; }
        }

        public bool StreamsSamenvoegen2()
        {
            try
            {
                //alles initialiseren
                if (!StreamInit())
                    throw new Exception("FOUT BIJ HET INITIALISEREN VAN DE STREAMS");

                //vliegt eruit als er iets mis gaat met verzenden of imagesSamenvoegen
                while (doorgaan)
                    if (imageRechts != null && imageLinks != null)
                        if (!socket.Verzenden(ImagesSamenvoegen2()))
                            throw new Exception("Error met stream samenvoegen of verzenden");

                //streams afluiten (webcams 'ontkoppelen')
                StramsAfsluiten();

                //gebruiker heeft afgesloten en alles is goed gegaan, true retourneren
                return true;
            }
            catch
            {
                //streams afluiten (webcams 'ontkoppelen')
                StramsAfsluiten();
                return false;
            }
        }

/*PROPORTIES*/
        public Bitmap Frame { get { return image; } set { image = value; } }
        public bool BlijfSamenvoegen { set { doorgaan = value; } }
        public TCPOut SocketStreamKortZijde { get { return kleineZijde.Socket; } }
        public TCPOut SocketStreamLangeZijde { get { return socket; } }
        public TCPOut SocketMotion { get { return detector.Socket; } }
        public TCPOut SOcketGezichtHerkenning { get { return sockFace; } }
        public bool StreamsAfsluiten { get { 
            try { videoM.Close(); videoL.Close(); videoR.Close(); return kleineZijde.Stop; }
            catch { return false; } } }
    }
}
