using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Drawing;
using System.Drawing.Imaging;

using AForge.Imaging;
using AForge.Imaging.Filters;

namespace VideoStreamMerger
{
    public class Background
    {
        private int[,] cF;
        private int gevonden=-1; //het aantal pixels dat veranderd moet worden omdat ze niet overeenkomen met de achtergrond
        private int x, y, height, width, tot, nr=0; //nr = het hoeveelste frame om background te bekijken
        private bool variabelen = false;

        private int opti; //optimilisatie: hoeveel frames er achter elkaar hetzelfde moeten zijn voor de background
        private int prec; //precisie: hoe precies de frames hetzelfde moeten zijn (10: om de 10 pixels word er bekeken)
        private int ver;
        public int totaal;

        public Background(int frames, int pixels, int verschil)
        {
            opti = frames;
            prec = pixels;
            ver = verschil;
        }

        /// <summary> een image word bekeken om zo de achterground 'bij te stellen' <summary>
        public void EditBackground(Bitmap image, bool vergelijken)
        {
            //eerste keer worden de variabelen goedgezet
            if (!variabelen)
            {
                //variabelen
                height = image.Height;
                width = image.Width;
                tot = (width * height) / (prec * prec);
                cF = new int[opti, tot];
                variabelen = true;
            }

            //cf invullen
            frameInvullen(image);

            //nr verhogen
            nr++;
            if (nr == opti)
                nr = 0;

            //vergelijken (alleen als het nodig is)
            if (!vergelijken)
                return;
            for (int i = 0; i < tot; i++)
                for (int a = 1; a < opti; a++)
                {
                    int ver = cF[0, i] - cF[a, i];
                    if (ver < 0) ver *= -1; //indien negatief, het getal positief maken
                    if (ver > this.ver) //kijken of het verschil accpetabel is, zo niet, methode beeindigen
                        return;
                }
            gevonden = 0;
        }

        private void frameInvullen(Bitmap image)
        {
            y = x = 0;
            //cf invullen
            for (int i = 0; i < tot; i++, y += prec)
            {
                if (y >= height)
                {
                    x += prec;
                    if (x > width) //word een pixel gepakt dat niet bestaat dus klaar met array vullen
                        return;
                    y = 0;
                }
                //de r, g en b optellen en delen door 3 voor een gem waarde
                cF[nr, i] = ((int)image.GetPixel(x, y).R + (int)image.GetPixel(x, y).G + (int)image.GetPixel(x, y).B) / 3;
            }
        }

        /// <summary> als er niks aan de background is veranderd, dan is de background klaar <summary>
        public bool BackgroundCheck()
        {
            if (gevonden == 0)
                return true;
            return false;
        }
    }
}
