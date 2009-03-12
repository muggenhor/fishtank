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
        private TCPOut socket;
        private int[] oudeXY = new int[2];

        public face(TCPOut socket)
        {
            objFaceDetector = clsFaceDetectionWrapper.GetSingletonInstance();
            data = new int[2];
            oudeXY[0] = oudeXY[1] = 50;
            this.socket = socket;
            //starten
            socket.Start();
        }

        public bool SocketStop()
        {
            return socket.Stop();
        }

        public int newFrame(System.Drawing.Bitmap lastPic)
        {
            //krijgt maar van 1 webcam binnen
            faces = objFaceDetector.WrapDetectFaces(lastPic);
            int lx = 0, ly = 0, rx = 0, ry = 0;

            if (lastPic != null)
            {
                unsafe
                {
                    switch(faces)
                    {
                        //geen gezicht, dus nieuwe webcam pakken
                        case 0:
                            //coordianten op 0 zetten zodat als er meerdere personen zijn en iemand word gevolgd die net in een nieuwe
                            //camera komt, als 'beste' word gezien
                            data[0] = 50;
                            data[1] = 50;
                            break;
                        //meerdere gezichten, kijken welke het dichtsbijzijnde van de oude positie is
                        default:
                            objFaceDetector.WrapGetFaceCordinates(0, &lx, &ly, &rx, &ry);
                            data[0] = (lx + rx / 2) / 320 * 100; //320: breedte van de webcam stream (altijd 320)
                            data[1] = (ly + ry / 2) / 240 * 100; //240: hoogte van de webcam stream (altijd 240)
                            break;
                    }
                }
                //data versturen
                byte[] buffer = new byte[2];
                buffer[0] = (byte)((oudeXY[0] + oudeXY[0] + data[0]) / 3); //(byte)data[0]; // ((webcam * 33) + (data[0] / 3));
                buffer[1] = (byte)((oudeXY[1] + oudeXY[1] + data[1]) / 3);//(byte)data[1];
                socket.Verzenden(buffer);
            }

            lastPic.Dispose();
            objFaceDetector.CloseImage();
            return faces;
        }

        public TCPOut Socket { get { return socket; } }
    }
}
