using System;
using System.Collections.Generic;
using System.Text;

namespace Webcam_Project
{
    public class Webcams
    {
        public Webcams()
        {
        }

        /// <summary>
        /// DE methode DER merhodes, deze doet echt alles, nadat de instellingen gelezen zijn uit
        /// settings.txt zal deze pas stoppen als de gebruiker het programma afsluit of als er een error
        /// optreedt. In de tussentijd start die de webcams op, zoekt naar achtergronden, probeert beelden samen te voegen,
        /// zoekt naar beweging en gezichten in de lange zijde en verstuurt alle nodige informatie door naar het vissen project
        /// </summary>
        static public void AllesStarten()
        {
            try
            {
                Console.WriteLine("Webcam Programma voor het Vissen Project\n\r----------------------------------------");
                Console.WriteLine("Eigendom van: Fontys Hogescholen\n\rGemaakt door: Gijs Trepels & Rik Timmers\n\r----------------------------------------\n\r");
                //alles initialiseren
                Console.WriteLine("PROGRAMMA INITIALISEREN");
                VideoStreamMerger.ImageControl imageC;
                VideoSource.CaptureDevice[] webcams = new VideoSource.CaptureDevice[4];
                string[] data = new string[20];
                Console.WriteLine(" Bezig met instellingen lezen (settings.txt)...");
                System.IO.StreamReader sr = System.IO.File.OpenText("settings.txt");
                for (int i = 0; i < 18; i++)
                    data[i] = sr.ReadLine();
                //alle webcams zoeken op de pc
                Console.WriteLine(" Bezig met webcams openen...");
                dshow.FilterCollection filters = new dshow.FilterCollection(dshow.Core.FilterCategory.VideoInputDevice);
                for (int i = 0; i < filters.Count; i++)
                    Console.WriteLine("  Webcam '" + filters[i].Name + "' gevonden");
                //de 4 webcams
                webcams[0].VideoSource = filters[Convert.ToInt32(data[0])].MonikerString;
                webcams[1].VideoSource = filters[Convert.ToInt32(data[1])].MonikerString;
                webcams[2].VideoSource = filters[Convert.ToInt32(data[2])].MonikerString;
                webcams[3].VideoSource = filters[Convert.ToInt32(data[3])].MonikerString;
                //imagecontrol aanmaken
                //en de imagecontrol alles laten doen
                Console.WriteLine("PROGRAMMA STARTEN");
                Console.WriteLine(" Bezig met aanmaken van de ImageControl...");
                imageC = new VideoStreamMerger.ImageControl(
                    Convert.ToInt32(data[15]),
                    Convert.ToInt32(data[14]),
                    Convert.ToInt32(data[10]),
                    Convert.ToInt32(data[9]),
                    ((float)Convert.ToInt32(data[16])) / (float)100,
                    1,
                    Convert.ToInt32(data[17]),
                    Convert.ToInt32(data[11]),
                    new VideoStreamMerger.TCPOut(data[4], Convert.ToInt32(data[7])),
                    new VideoStreamMerger.TCPOut(data[4], Convert.ToInt32(data[8])),
                    Convert.ToInt32(data[13]),
                    Convert.ToInt32(data[12]),
                    new VideoStreamMerger.TCPOut(data[4], Convert.ToInt32(data[5])),
                    Convert.ToInt32(data[19]));
                imageC.WebcamVoorKleineZijdeLaden(
                    webcams[0],
                    new VideoStreamMerger.TCPOut(data[4], Convert.ToInt32(data[6])));
                Console.WriteLine(" Zoeken naar achtergronden in de webcambeelden...");
                imageC.WebcamLadenEnAchtergrondBepalen(
                    webcams[1],
                    webcams[2],
                    webcams[3]);
                Console.WriteLine(" Zoeken naar vergelijkingen in de webcambeelden...");
                if (data[18] == "3") //indien 3 webcams
                {
                    if (!imageC.ImagesVergelijkenRechtsMidden())
                        Console.WriteLine("  Geen vergelijking tussen Rechts en Midden gevonden");
                    Console.WriteLine("  Vergelijking tussen Rechts en Midden gevonden");
                    if (!imageC.ImagesVergelijkenLinksMidden())
                        Console.WriteLine("  Geen vergelijking tussen Links en Midden gevonden");
                    Console.WriteLine("  Vergelijking tussen Rechts en Midden gevonden");
                    Console.WriteLine(" Nieuwe videostream aanmaken voor de lange zijde...");
                    imageC.NieuweImageInitialiseren();
                    Console.WriteLine(" Bezig met streamen naar " + data[4] + " op poorten " + data[5] + ", " + data[6] + ", " + data[7] + ", " + data[8] + "...");
                    imageC.StreamsSamenvoegen(); //blijft voor altijd doorgaan (doet alles, het samenvoegen van de streams, motion detectie, venzenden van alles)                                                  
                }
                else //2 webcams
                {
                    if (!imageC.ImagesVergelijkenRechtsLinks())
                        Console.WriteLine("  Geen vergelijking tussen Links en Rechts gevonden");
                    Console.WriteLine("  Vergelijking tussen Links en Rechts gevonden");
                    Console.WriteLine(" Nieuwe videostream aanmaken voor de lange zijde...");
                    imageC.NieuweImageInitialiseren2();
                    Console.WriteLine(" Bezig met streamen naar " + data[4] + " op poorten " + data[5] + ", " + data[6] + ", " + data[7] + ", " + data[8] + "...");
                    imageC.StreamsSamenvoegen2();
                }
                Console.WriteLine(" Streamen is gestopt");
                if (imageC.StreamsAfsluiten)
                    Console.WriteLine("Webcams zijn 'losgekoppeld' van het programma");
            }
            catch
            {
                Console.Write("\n\rERROR - Het probleem kan verholpen worden door het programma \n\ropnieuw op te starten,");
                Console.Write(" druk op Enter om programma af te sluiten"); Console.ReadLine();
            }
        }
    }
}
