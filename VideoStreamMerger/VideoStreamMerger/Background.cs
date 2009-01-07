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
        private int[] bF;
        private int[,] cF;
        private int gevonden=-1; //het aantal pixels dat veranderd moet worden omdat ze niet overeenkomen met de achtergrond
        private int x, y, height, width, tot, nr=0; //nr = het hoeveelste frame om background te bekijken

        private int opti;

        public Background()
        {
            opti = 5;
        }

        /// <summary> een image word bekeken om zo de achterground 'bij te stellen' <summary>
        public void EditBackground(Bitmap image)
        {
            y = x = 0;

            //eerste keer word bf gewoon cf
            //ook worden dan de variabelen goedgezet
            if (bF == null)
            {
                //variabelen
                height = image.Height;
                width = image.Width;
                tot = width * height;

                cF = new int[opti, tot];
                bF = new int[1]; //wordt niet meer gebruikt !!!
 /*               bF = new int[tot];
                

                for (int i = 0; i < tot; i++, y++)
                {
                    if (y == height)
                    {
                        x++;
                        y = 0;
                    }
                    bF[i] = image.GetPixel(x, y).ToArgb();
                }
                return;*/
            }

            //cf invullen
            for (int i = 0; i < tot; i++, y++)
            {
                if (y == height)
                {
                    x++;
                    y = 0;
                }
                cF[nr, i] = image.GetPixel(x, y).ToArgb();
            }

            //nr verhogen
            nr++;
            if (nr == opti)
                nr = 0;


            //vergelijken en meteen invullen (word nog anders)
            //werkt niet, de laatste is nu de background indien die niet veranderd... (klopt eigenlijk ook wel)
            for (int i = 0; i < tot; i++)
                //alle 5 de frames moeten nu precies hetzelfde zijn
                for (int a = 1; a < opti; a++)
                {
                    int temp1 = cF[0, i];
                    int temp2 = cF[a, i];
                    if (cF[0, i] != cF[a, i])
                        return; // niet gevonden dus meteen afbreken
                }
            //alles klopt :)
            gevonden = 0;
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
