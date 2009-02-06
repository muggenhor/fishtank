using System;
using System.Collections.Generic;
using System.Text;

namespace VideoStreamMerger
{
    class face
    {
       
        private clsFaceDetectionWrapper objFaceDetector;
        private int[] data;
        private int faces=0;
        int xx;

        public face()
        {
            objFaceDetector = clsFaceDetectionWrapper.GetSingletonInstance();
            data = new int[2];
        }

        public int start(System.Drawing.Bitmap lastPic)
        {
            faces = objFaceDetector.WrapDetectFaces(lastPic);
            int lx = 0, ly = 0, rx = 0, ry = 0;

            if (lastPic != null && faces == 1)
            {
                unsafe
                {
                    for (int f = 0; f < faces; f++)
                    {
                        objFaceDetector.WrapGetFaceCordinates(f, &lx, &ly, &rx, &ry);
                        
                    }
                }
            }

            xx = lx;
            data[0] = (lx + rx / 2);
            data[1] = (ly + ry / 2);
            lastPic.Dispose();

            objFaceDetector.CloseImage();

            return faces;
        }
        public int facex { get { return faces; } }
        public int x { get { return xx; } }
        public int[] Coordinaten { get { return data; } }
        public int aantalGezichten { get { return faces; } }
    }
}
