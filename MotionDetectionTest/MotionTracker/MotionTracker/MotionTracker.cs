using System;
using System.Drawing;
using System.Drawing.Imaging;

using AForge.Imaging;
using AForge.Imaging.Filters;

using System.IO;
using System.Data;

namespace motion
{
    public class MotionTracker
    {
        //variabelen
        private int xas, yas, blokjes;
        private byte[] backgroundFrame = null, currentFrame=null, currentFrameDilatated=null;
        private int counter = 0, width, height, pixelsChanged;
        private bool calculateMotionLevel = false;

        private byte[,] background=null, current=null;

        //testje
        private double x=0, y = 0, ox=0, oy=0;

        //methoden
        // Constructor
		public MotionTracker( )
		{
            
		}

		// Reset detector to initial state
		public void Reset( )
		{
			backgroundFrame = null;
			currentFrame = null;
			currentFrameDilatated = null;
			counter = 0;
		}

		// Process new frame
		public void ProcessFrame( ref Bitmap image )
		{
			// get image dimension
			width	= image.Width;
			height	= image.Height;

			int fW = ( ( ( width - 1 ) / 8 ) + 1 );
			int fH = ( ( ( height - 1 ) / 8 ) + 1 );
			int len = fW * fH;

   /*         if (background == null)
            {
                background = new byte[width, height];
                current = new byte[width, height];

                // lock image
                BitmapData imgData = image.LockBits(
                    new Rectangle(0, 0, width, height),
                    ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

                // create initial backgroung image
                PreprocessInputImage(imgData, width, height, backgroundFrame);
 //               PreprocessInputImage(imgData, width, height, background);

                //teste
                for (int i=0; i<fH; i++)
                    background

                // unlock the image
                image.UnlockBits(imgData);

                // just return for the first time
                return;
            }
            // lock image
            BitmapData data = image.LockBits(
                new Rectangle(0, 0, width, height),
                ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

            // preprocess input image
            PreprocessInputImage(data, width, height, currentFrame);


            //variablen
            int temp;
            current = new byte[width,height];
            //weer wat testen
            for (int x=0; x<width; x++)
                for (int y = 0; y < height; y++)
                {
                    //kleurverschil bepalen
                    temp = current[x, y] - background[x, y];
                    //indien negatief, even postitief maken
                    if (temp < 0)
                        temp = -temp;
                    //indoen verschil groot genoeg, gegevens toevoegen
                    if (temp >= 40)
                    {
                        xas += x;
                        yas += y;
                        blokjes++;
                    }
                }

            SaveCoordinates(xas / blokjes, yas / blokjes);
            // unlock the image
            image.UnlockBits(data);
   */         
			if ( backgroundFrame == null )
			{
				// alloc memory for a backgound image and for current image
				backgroundFrame = new byte[len];
				currentFrame = new byte[len];
				currentFrameDilatated = new byte[len];

				// lock image
				BitmapData imgData = image.LockBits(
					new Rectangle( 0, 0, width, height ),
					ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb );

				// create initial backgroung image
				PreprocessInputImage( imgData, width, height, backgroundFrame );

				// unlock the image
				image.UnlockBits( imgData );

				// just return for the first time
				return;
			}

			// lock image
			BitmapData data = image.LockBits(
				new Rectangle( 0, 0, width, height ),
				ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb );

			// preprocess input image
			PreprocessInputImage( data, width, height, currentFrame );

			if ( ++counter == 2 )
			{
				counter = 0;

				// move background towards current frame
                
				for ( int i = 0; i < len; i++)
				{


					int t = currentFrame[i] - backgroundFrame[i];
					if ( t > 0 )
						backgroundFrame[i]++;
					else if ( t < 0 )
						backgroundFrame[i]--;
				}
			}

            // difference and thresholding
			pixelsChanged = 0;

            /* test */
            int x = 0, y = 0;
            blokjes = xas = yas = 0;
			for ( int i = 0; i < len; i++, x++ )
			{
                //test
                if (x == fW) //gelijk aan de lengte
                {
                    x = 0; y++;
                }
                //einde test


				int t = currentFrame[i] - backgroundFrame[i];
				if ( t < 0 )
					t = -t;

				if ( t >= 40 ) //15 is standaard
				{
                    //test
                    xas += x*8;
                    yas += y*8;
                    blokjes++;

					pixelsChanged++;
					currentFrame[i] = (byte) 255;
				}
				else
				{
					currentFrame[i] = (byte) 0;
				}
			}
            SaveCoordinates2(xas / blokjes, yas / blokjes);
			if ( calculateMotionLevel )
				pixelsChanged *= 64;
			else
				pixelsChanged = 0;

			// dilatation analogue for borders extending
			// it can be skipped
			for ( int i = 0; i < fH; i++ )
			{
				for ( int j = 0; j < fW; j++ )
				{
					int k = i * fW + j;
					int v = currentFrame[k];

					// left pixels
					if ( j > 0 )
					{
						v += currentFrame[k - 1];

						if ( i > 0 )
						{
							v += currentFrame[k - fW - 1];
						}
						if ( i < fH - 1 )
						{
							v += currentFrame[k + fW - 1];
						}
					}
					// right pixels
					if ( j < fW - 1 )
					{
						v += currentFrame[k + 1];

						if ( i > 0 )
						{
							v += currentFrame[k - fW + 1];
						}
						if ( i < fH - 1 )
						{
							v += currentFrame[k + fW + 1];
						}
					}
					// top pixel
					if ( i > 0 )
					{
						v += currentFrame[k - fW];
					}
					// right pixel
					if ( i < fH - 1 )
					{
						v += currentFrame[k + fW];
					}

					currentFrameDilatated[k] = (v != 0) ? (byte) 255 : (byte) 0;
				}
			}

			// postprocess the input image
			PostprocessInputImage( data, width, height, currentFrameDilatated );

			// unlock the image
			image.UnlockBits( data );
    
		}

		// Preprocess input image
		private void PreprocessInputImage( BitmapData data, int width, int height, byte[] buf )
//        private void PreprocessInputImage( BitmapData data, int width, int height, byte[,] buf2 )
		{
			int stride = data.Stride;
			int offset = stride - width * 3;
			int len = (int)( ( width - 1 ) / 8 ) + 1;
			int rem = ( ( width - 1 ) % 8 ) + 1;
			int[] tmp = new int[len];
			int i, j, t1, t2, k = 0;

			unsafe
			{
				byte * src = (byte *) data.Scan0.ToPointer( );

				for ( int y = 0; y < height; )
				{
					// collect pixels
					Array.Clear( tmp, 0, len );

					// calculate
					for ( i = 0; ( i < 8 ) && ( y < height ); i++, y++ )
					{
						// for each pixel
						for ( int x = 0; x < width; x++, src += 3 )
						{
							// grayscale value using BT709
							tmp[(int) ( x / 8 )] += (int)( 0.2125f * src[RGB.R] + 0.7154f * src[RGB.G] + 0.0721f * src[RGB.B] );
						}
						src += offset;
					}

					// get average values
					t1 = i * 8;
					t2 = i * rem;

					for ( j = 0; j < len - 1; j++, k++ )
						buf[k] = (byte)( tmp[j] / t1 );
					buf[k++] = (byte)( tmp[j] / t2 );
        //            for (int x = 0; x < width; x++)
          //              //      for (int y=0; y<height; y++)
            //            buf2[x, y] = (byte)(tmp[x*y] / t1);
				}
			}
		}

		// Postprocess input image
		private void PostprocessInputImage( BitmapData data, int width, int height, byte[] buf )
		{
			int stride = data.Stride;
			int offset = stride - width * 3;
			int len = (int)( ( width - 1 ) / 8 ) + 1;
			int lenWM1 = len - 1;
			int lenHM1 = (int)( ( height - 1 ) / 8);
			int rem = ( ( width - 1 ) % 8 ) + 1;

			int i, j, k;

            //testen
            xas = yas = blokjes = 0;
	
			unsafe
			{
				byte * src = (byte *) data.Scan0.ToPointer( );

				// for each line
				for ( int y = 0; y < height; y++ )
				{
					i = (y / 8);

					// for each pixel
					for ( int x = 0; x < width; x++, src += 3 )
					{
						j = x / 8;	
						k = i * len + j;

						// check if we need to highlight moving object
						if (buf[k] == 255)
						{
							// check for border
				/*			if (
								( ( x % 8 == 0 ) && ( ( j == 0 ) || ( buf[k - 1] == 0 ) ) ) ||
								( ( x % 8 == 7 ) && ( ( j == lenWM1 ) || ( buf[k + 1] == 0 ) ) ) ||
								( ( y % 8 == 0 ) && ( ( i == 0 ) || ( buf[k - len] == 0 ) ) ) ||
								( ( y % 8 == 7 ) && ( ( i == lenHM1 ) || ( buf[k + len] == 0 ) ) )
								)
							{
								src[RGB.R] = 255;
                          */      //ff iets testen
                                xas += x;
                                yas += y;
                                blokjes++;
					//		}
						}
					}
					src += offset;
				}
			}

            SaveCoordinates(xas / blokjes, yas / blokjes);
		}

        //save the best coordiante in (for now) a txt file
        private void SaveCoordinates2(float x, float y)
        {

            this.x = (float)(x / (float)width);
            this.y = (float)(y / (float)height);
            
            FileStream fs = new FileStream("c://temp2.txt", FileMode.OpenOrCreate, FileAccess.Write);
            StreamWriter sw = new StreamWriter(fs);
            sw.WriteLine(this.x);
            sw.WriteLine(this.y);
            sw.Close();

 
        }

        //save the best coordiante in (for now) a txt file
        private void SaveCoordinates(float x, float y)
        {
            this.ox = (float)(x / (float)width);
            this.oy = (float)(y / (float)height);

            FileStream fs = new FileStream("c://temp.txt", FileMode.OpenOrCreate, FileAccess.Write);
            StreamWriter sw = new StreamWriter(fs);
            sw.WriteLine(this.ox);
            sw.WriteLine(this.oy);
            sw.Close();


        }

        //proporties
        // Motion level calculation - calculate or not motion level
        public bool MotionLevelCalculation
        {
            get { return calculateMotionLevel; }
            set { calculateMotionLevel = value; }
        }

        // Motion level - amount of changes in percents
        public double MotionLevel
        {
            get { return (double)pixelsChanged / (width * height); }
        }

        //testje
        public double MethodCheckX { get { return x; } }
        public double MethodCheckY { get { return y; } }
        public double MethodCheckoX { get { return ox; } }
        public double MethodCheckoY { get { return oy; } }

    }
}
