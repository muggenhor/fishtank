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
        public int totaal;
        public int goed;

        public Background(int frames, int pixels)
        {
            opti = frames;
            prec = pixels;
        }

        /// <summary> een image word bekeken om zo de achterground 'bij te stellen' <summary>
        public void EditBackground(Bitmap image)
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

            //vergelijken en meteen invullen (word nog anders)
            //werkt niet, de laatste is nu de background indien die niet veranderd... (klopt eigenlijk ook wel)
            for (int i = 0; i < tot; i++)
                //alle frames moeten nu precies hetzelfde zijn
                for (int a = 1; a < opti; a++)
                {
                    int temp1 = cF[0, i];
                    int temp2 = cF[a, i];
                    if (cF[0, i] != cF[a, i]) 
                        return;
                    else goed++;
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
                    if (x > width) //word een pixel gepakt dat niet bestaat...
                        return;
                    y = 0;
                }
                cF[nr, i] = image.GetPixel(x, y).ToArgb();
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
