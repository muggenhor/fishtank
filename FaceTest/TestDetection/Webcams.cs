using System;
using System.Collections.Generic;
using System.Text;

namespace Webcam_Project
{
    class Webcams
    {
        //sockets verzenden:
        //allemaal op ip adres 127.0.0.1 (localhost)
        //7778: streamen van webcambeeld aan de kleine zijde
        //7779: streamen van webcambeeld aan de lange zijde
        //7780: coordinaten van motiondetection aan de lange zijde
        //7781: coordianten van de gezichtsherkenning aan de lange zijde

        public Webcams()
        {
        }

        public void AllesStarten()
        {
     //       try
            {
                Console.WriteLine("Webcam Project");
                Console.WriteLine("--------------");
                Console.WriteLine();
                //variabelen
                VideoStreamMerger.ImageControl imageC;

                Console.Write("toets 'v' voor videotest: ");
                if (Console.ReadLine() != "v")
                {
                    //alle webcams zoeken op de pc
                    dshow.FilterCollection filters = new dshow.FilterCollection(dshow.Core.FilterCategory.VideoInputDevice);
                    //de 4 webcams
                    VideoSource.CaptureDevice webcamLinks = new VideoSource.CaptureDevice();
                    VideoSource.CaptureDevice webcamRechts = new VideoSource.CaptureDevice();
                    VideoSource.CaptureDevice webcamMidden = new VideoSource.CaptureDevice();
                    VideoSource.CaptureDevice webcamAlleen = new VideoSource.CaptureDevice();

                    //menu
                    //--begin webcams selecteren--
                    Console.WriteLine("WEBCAM KIEZEN");
                    Console.WriteLine(" Gevonden webcams: ");
                    for (int i = 0; i < filters.Count; i++)
                        Console.WriteLine("  " + i + ": " + filters[i].Name);
                    //de 'alleenstaande webcam kiezen'
                    Console.Write(" Kies de camera aan de korte zijde: "); int a = Convert.ToInt32(Console.ReadLine());
                    webcamAlleen.VideoSource = filters[a].MonikerString;
                    
                    // de webcams voor de lange zijde kiezen
                    //linkerwebcam
                    Console.Write(" Kies de webcam aan de linkerkant: ");
                    int l = Convert.ToInt32(Console.ReadLine());
                    while (a == l)
                    {
                        Console.Write(" Is al in gebruik, kies een andere: ");
                        l = Convert.ToInt32(Console.ReadLine());
                    }
                    webcamLinks.VideoSource = filters[l].MonikerString;
                    //middenwebcam
                    Console.Write(" Kies de webcam in het midden: ");
                    int m = Convert.ToInt32(Console.ReadLine());
                    while (m == a || m == l)
                    {
                        Console.Write(" Is al in gebruik, kies een andere: ");
                        m = Convert.ToInt32(Console.ReadLine());
                    }
                    webcamMidden.VideoSource = filters[m].MonikerString;
                    //rechterwebcam
                    Console.Write(" Kies de webcam aan de rechterkant: ");
                    int r = Convert.ToInt32(Console.ReadLine());
                    while (r == l || r == a || r == m)
                    {
                        Console.Write(" Is al in gebruik, kies een andere: ");
                        r = Convert.ToInt32(Console.ReadLine());
                    }
                    webcamRechts.VideoSource = filters[r].MonikerString;
                    //--einde webcams selecteren--
                    //instellingen setten
                    Console.WriteLine("INSTELLINGEN GOED ZETTEN");
    /*                Console.Write(" 'j' om de default waarden te laden van de Image Control: ");
                    if (Console.ReadLine().ToLower() != "j")
                    {
                        int hoo, kol, fra, pix, rat, vba, vim, por, tim, vimm, porm; float per; string ip, ipm;
                        Console.Write(" Aantal frames dat gebruikt moet worden voor de achtergrond (1-50): "); fra = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Aantal pixels dat word overgeslagen bij achtergrond bepalen (1-50): "); pix = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Maximale kleurverschil tussen pixels bij het bepalen van de achtergrond (0-255): "); vba = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Hoogte van de kolom dat gebruikt word bij het vergelijken (1-100): "); hoo = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Aantal kolommen dat vergeleken moet worden (1-50): "); kol = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Percentage dat de webcambeelden hetzelfde moeten zijn (0-1): "); per = (float)Convert.ToDouble(Console.ReadLine());
                        Console.Write(" Maximale kleurverschil tussen pixels bij het vergelijken (0-255): "); vim = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Aantal keren dat het samengevoegd beeld kleiner is (1-5): "); rat = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Het ip addres van de server voor de beelden (bijv 127.0.0.1): "); ip = Console.ReadLine();
                        Console.Write(" De poort voor het verzenden van het beeld (bijv 1234): "); por = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Maximale kleurverschil tussen pixels bij het zoeken van beweging (0-255): "); vimm = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Aantal miliseconden dat er op beweging gezocht moet worden (1000-10000): "); tim = Convert.ToInt32(Console.ReadLine());
                        Console.Write(" Het ip addres van de server voor de beweging (bijv 127.0.0.1): "); ipm = Console.ReadLine();
                        Console.Write(" De poort voor het verzenden van de coordinaat van de beweging (bijv 1235): "); porm = Convert.ToInt32(Console.ReadLine());
                        imageC = new VideoStreamMerger.ImageControl(hoo, kol, fra, pix, per, rat, vim, vba, new VideoStreamMerger.TCPOut(ip, por), new VideoStreamMerger.TCPOut(ipm, porm), vimm, tim);
                    }
                    else*/
                        imageC = new VideoStreamMerger.ImageControl(80, 5, 10, 10, (float)0.8, 1, 30, 50, new VideoStreamMerger.TCPOut("127.0.0.1", 7779), new VideoStreamMerger.TCPOut("127.0.0.1", 7780), 20, 500, new VideoStreamMerger.TCPOut("127.0.0.1", 7781));
                    //en de imagecontrol alles laten doen
                    Console.WriteLine("PROGRAMMA STARTEN");
                    Console.WriteLine(" Achtergrond gevonden in de webcambeelden: " + imageC.WebcamLadenEnAchtergrondBepalen(webcamLinks, webcamRechts, webcamMidden));
                    Console.WriteLine(" Zoeken naar vergelijkingen in de webcambeelden");
                    if (!imageC.ImagesVergelijkenRechtsMidden())
                        Console.WriteLine("  Geen vergelijking tussen Rechts en Midden gevonden");
                    Console.WriteLine("  Vergelijking tussen Rechts en Midden gevonden");
                    if (!imageC.ImagesVergelijkenLinksMidden())
                        Console.WriteLine("  Geen vergelijking tussen Links en Midden gevonden");
                    Console.WriteLine("  Vergelijking tussen Rechts en Midden gevonden");                  
                    Console.WriteLine(" Nieuwe videostream gemaakt van de webcambeelden: " + imageC.NieuweImageInitialiseren());
                    Console.WriteLine(" Wencambeelden voor de kleine zijde worden verstuurt naar: " + imageC.SocketStreamKortZijde.IpAddres + ":" + imageC.SocketStreamKortZijde.Poort);                   
                    Console.WriteLine(" Webcambeelden voor de lange zijde worden verstuurt naar: " + imageC.SocketStreamLangeZijde.IpAddres + ":" + imageC.SocketStreamLangeZijde.Poort);
                    Console.WriteLine(" Coordinaten van de Motion Detection worden verstuurt naar: " + imageC.SocketMotion.IpAddres + ":" + imageC.SocketMotion.Poort);
                    Console.WriteLine(" Coordinaten van gezichtHerkenning worden verstuurt naar: " + imageC.SocketStreamKortZijde.IpAddres + ":" + imageC.SocketStreamKortZijde.Poort);
                    Console.WriteLine(" Bezig met streamen...");
                    imageC.StreamsSamenvoegen(); //blijft voor altijd doorgaan (doet alles, het samenvoegen van de streams, motion detectie, venzenden van alles)                               
                    Console.WriteLine(" Streamen is gestopt");
                    if (imageC.StreamsAfsluiten)
                        Console.WriteLine("Webcams zijn 'losgekoppeld' van het programma");
                }
                else
                {
                    //TEST
                    Console.WriteLine("---TEST---");
                    VideoSource.VideoFileSource vid1 = new VideoSource.VideoFileSource();
                    VideoSource.VideoFileSource vid2 = new VideoSource.VideoFileSource();
                    VideoSource.VideoFileSource vid3 = new VideoSource.VideoFileSource();
                    Console.Write("pad van video1 (bijv. c://Sin City.avi): "); vid1.VideoSource = "C://Wall-E.avi"; //Console.ReadLine();
                    Console.Write("pad van video2 (bijv. c://Sin City.avi): "); vid2.VideoSource = "C://Wall-E.avi"; // Console.ReadLine();
                    vid3.VideoSource = "C://Wall-E.avi";
                    imageC = new VideoStreamMerger.ImageControl(80, 5, 1, 10, (float)0.95, 1, 100, 100, new VideoStreamMerger.TCPOut("127.0.0.1", 7779),
                        new VideoStreamMerger.TCPOut("127.0.0.1", 7780), 50, 500, new VideoStreamMerger.TCPOut("127.0.0.1", 7781));
                    Console.WriteLine("ImageControl aangemaakt");
                    Console.WriteLine("Achtergrond Laden: " + imageC.VideoLadenEnAchtergrondBepalen(vid1, vid2, vid3));
                    Console.WriteLine("Vergelinking Links-Midden: " + imageC.ImagesVergelijkenLinksMidden());
                    Console.WriteLine("Vergelijking Rechts-Midden: "+ imageC.ImagesVergelijkenRechtsMidden());
                    Console.WriteLine("Initialiseren: " + imageC.NieuweImageInitialiseren());
                    Console.WriteLine("Bezig met streamen naar " + imageC.SocketStreamLangeZijde.IpAddres + ":" + imageC.SocketStreamLangeZijde.Poort);
                    imageC.StreamsSamenvoegen();
                    Console.WriteLine("ImageControl is gestopt..."); Console.ReadLine(); imageC.BlijfSamenvoegen = false;
                    Console.WriteLine("Verbinden verbreken met de webcams is:" + imageC.StreamsAfsluiten);
               }
            }
 /*           catch 
            { 
                Console.Write("ERROR - Het probleem kan verholpen worden door het programma opnieuw op te starten");
                Console.Write(" druk op Enter om programma af te sluiten"); Console.ReadLine();                
            }
 */       }
    }
}
