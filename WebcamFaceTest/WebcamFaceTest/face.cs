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
        private int[] currentXY = new int[2];
        private int noFaceCount = 0;
        private int[] oudeXY = new int[2];
        private int webcam=0;

        public face(TCPOut socket)
        {
            objFaceDetector = clsFaceDetectionWrapper.GetSingletonInstance();
            data = new int[2];
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
                    Console.WriteLine(faces + " gezichten gevonden");
                    switch(faces)
                    {
                        //geen gezicht, dus nieuwe webcam pakken
                        case 0:
                            webcam++;
                            if (webcam > 2)
                            {
                                //nieuwe webcam pakken
                                webcam = 0;

                                data[0] = 50;
                                data[1] = 50;
                                //optellen die chap
                                noFaceCount++;
                            }
                            break;
                        //meerdere gezichten, de meeste links word gepakt
                        default:
                            objFaceDetector.WrapGetFaceCordinates(0, &lx, &ly, &rx, &ry);
                            //opschalen
                            data[0] = (int)(-(webcam * 30 + (double)(((double)lx + (double)rx / 2.0) / 3.2)/3.0) + 100.0); //3.2: breedte van de webcam stream (altijd 320)
                            data[1] = (int)(-(((double)ly + (double)ry / 2.0) / 2.4) + 100.0); //2.4: hoogte van de webcam stream (altijd 240)
                            Console.WriteLine("huidige positie: " + data[0] + ", " + data[1]);
                            //resetten die chap
                            noFaceCount = 0;
                            break;
                    }
                }
                //data versturen, maar alleen als de chap 0 is of groter dan andere chap
                if (noFaceCount == 0 || noFaceCount > 4)
                {
                    byte[] buffer = new byte[2];
                    int verschuif;
                    verschuif = (int)(((double)data[0] - (double)currentXY[0]) / 2.0);
                    if (verschuif <= 1)
                        currentXY[0] = data[0];
                    else
                        currentXY[0] += verschuif;
                    verschuif = (int)(((double)data[1] - (double)currentXY[1]) / 2.0);
                    if (verschuif <= 1)
                        currentXY[1] = data[1];
                    else
                        currentXY[1] += verschuif;
                    //data versturen
                    buffer[0] = (byte)((oudeXY[0] + oudeXY[0] + data[0]) / 3); //(byte)data[0]; // ((webcam * 33) + (data[0] / 3));
                    buffer[1] = (byte)((oudeXY[1] + oudeXY[1] + data[1]) / 3);//(byte)data[1];
                    socket.Verzenden(buffer);
                }
            }
            lastPic.Dispose();
            objFaceDetector.CloseImage();
            return faces;
        }

        public int Webcam { get { return webcam; } }
        public TCPOut Socket { get { return socket; } }
    }
}
