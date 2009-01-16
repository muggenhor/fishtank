﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Webcam_Project
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Webcam Project - gemaakt door Gijs Trepels");
            Console.WriteLine("------------------------------------------");
            Console.WriteLine();
            //variabelen
            VideoStreamMerger.ImageControl imageC;
            dshow.FilterCollection filters = new dshow.FilterCollection(dshow.Core.FilterCategory.VideoInputDevice);
            VideoSource.CaptureDevice webcamLinks = new VideoSource.CaptureDevice();
            VideoSource.CaptureDevice webcamRechts = new VideoSource.CaptureDevice();            
            //menu
            //webcam selecteren
    /*        Console.WriteLine("WEBCAM KIEZEN");
            Console.WriteLine(" Gevonden webcams: ");
            for (int i = 0; i < filters.Count; i++)
                Console.WriteLine(" "+ i + ": " + filters[i].Name);
            Console.Write(" Kies de linkerwebcam: ");
            int l = Convert.ToInt32(Console.ReadLine());
            webcamLinks.VideoSource = filters[l].MonikerString;
            Console.Write(" Kies de rechterwebcam: ");
            int r = Convert.ToInt32(Console.ReadLine());
            while (r == l)
            {
                Console.Write(" Is al in gebruik, kies een andere: ");
                r = Convert.ToInt32(Console.ReadLine());
            }
            webcamRechts.VideoSource = filters[r].MonikerString;
            //instellingen setten
            Console.WriteLine("INSTELLINGEN GOED ZETTEN");
            Console.Write(" 'j' om de default waarden te laden van de Image Control: ");
            if (Console.ReadLine().ToLower() != "j")
            {
                int kol, fra, pix, rat, vba, vim, por, tim, vimm, porm; float per; string ip, ipm;
                Console.Write(" Aantal frames dat gebruikt moet worden voor de achtergrond (1-50): "); fra = Convert.ToInt32(Console.ReadLine());
                Console.Write(" Aantal pixels dat word overgeslagen bij achtergrond bepalen (1-50): "); pix = Convert.ToInt32(Console.ReadLine());
                Console.Write(" Maximale kleurverschil tussen pixels bij het bepalen van de achtergrond (0-255): "); vba = Convert.ToInt32(Console.ReadLine());
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
                imageC = new VideoStreamMerger.ImageControl(kol, fra, pix, per, rat, vim, vba, new VideoStreamMerger.TCPOut(ip, por), vimm, new VideoStreamMerger.TCPOut(ipm, porm), tim);
            }
            else
                imageC = new VideoStreamMerger.ImageControl(1, 1, 10, (float)0.05, 1, 20, 20, new VideoStreamMerger.TCPOut(), 50, new VideoStreamMerger.TCPOut("localhost", 1235), 2000);
            //en de imagecontrol alles laten doen
            Console.WriteLine("PROGRAMMA STARTEN");
            Console.WriteLine(" Achtergrond gevonden in de webcambeelden: " + imageC.WebcamLadenEnAchtergrondBepalen(webcamLinks, webcamRechts));
            Console.WriteLine(" Vergelijking gevonden in de webcambeelden: " + imageC.ImagesVergelijken());
            Console.WriteLine(" Nieuwe videostream gemaakt van de webcambeelden: " + imageC.NieuweImageInitialiseren());
            Console.WriteLine(" Webcambeelden worden samengevoegd en verstuurt naar: " + imageC.SocketStream.IpAddres + ":" + imageC.SocketStream.Poort);
            Console.WriteLine(" Coordinaten van de Motion Detection worden verstuurt naar: " + imageC.SocketMotion.IpAddres + ":" + imageC.SocketMotion.Poort);
            Console.WriteLine(" Typ 'quit' om af te sluiten.");
            imageC.StreamsSamenvoegen();
            //afsluiten
            while (Console.ReadLine().ToLower() != "quit") { }
            imageC.BlijfSamenvoegen = false;*/

            //TEST
            Console.WriteLine("---TEST---");
            VideoSource.VideoFileSource vid1 = new VideoSource.VideoFileSource();
            VideoSource.VideoFileSource vid2 = new VideoSource.VideoFileSource();
            vid1.VideoSource = "e://Movies//Pulp Fiction.avi";
            vid2.VideoSource = "e://Movies//Pulp Fiction.avi";
            imageC = new VideoStreamMerger.ImageControl(1, 1, 10, (float)0.05, 1, 100, 100, new VideoStreamMerger.TCPOut(), 
                50, new VideoStreamMerger.TCPOut("localhost", 1235), 2000);
            Console.WriteLine("Achtergrond Laden: " + imageC.VideoLadenEnAchtergrondBepalen(vid1, vid2));
            Console.WriteLine("Vergelijken: " + imageC.ImagesVergelijken());
            Console.WriteLine("Initialiseren: " + imageC.NieuweImageInitialiseren());
            imageC.StreamsSamenvoegen();
            Console.WriteLine("Afsluiten?"); Console.ReadLine();
        }
    }
}