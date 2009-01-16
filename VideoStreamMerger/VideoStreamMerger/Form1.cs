using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;


using System.IO;
using System.Drawing.Imaging;
using VideoSource;

namespace VideoStreamMerger
{
    public partial class Form1 : Form
    {
        private ImageControl image=null;
        private bool imageLinks=false, imageRechts=false;
        private byte[] dataLinks=null, dataRechts=null;
        private VideoFileSource vid1=new VideoFileSource(), vid2=new VideoFileSource();

        /*
        private Bitmap imageLinks, imageRechts, image;
        private VideoInput video1, video2;
        private int height, width, verschil;
        private byte[] dataImage=null, dataRechts=null, dataLinks=null;
        private int imLengte = 0;
        private MemoryStream msL = new MemoryStream(), msR = new MemoryStream();
        */
         
        public Form1()
        {
            InitializeComponent();

            //imageControl opstarten
            image = new ImageControl(1, 1, 10, (float)0, 1, 255, 255, new TCPOut());

            //videostreams laden
            vid1.VideoSource = "c://Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
            vid2.VideoSource = "c://Simpsons 08x14 - The Itchie Scratchy and Poochie Show [kl0wnz].avi";
           // image.WebcamLadenEnAchtergrondBepalen();
            image.VideoLadenEnAchtergrondBepalen(vid1, vid2);

            //alles initialiseren
            image.ImagesVergelijken();
            image.NieuweImageInitialiseren();

            //blijft voor altijd doorgaan (behalve als image.BlijfSamenvoegen false word)
            image.StreamsSamenvoegen(); 
            
        }

            //webcam laden
  /*          CaptureDeviceForm form = new CaptureDeviceForm();
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                // create video source
                CaptureDevice localSource = new CaptureDevice();
                localSource.VideoSource = form.Device;

                // open it
                video1 = new VideoInput(localSource, frames, pixels);
                while (!video1.BackgroundFound) { }
                label1.Text = "Frames voor optimalisatie: " + video1.Source.FramesReceived;
                imageLinks = video1.backGround;
                imageLinks.Save("c://Achtergrond_Links.bmp");

                //         pictureBox1.Size = imageLinks.Size;
                //          video1.frame += new VideoInput.EventHandler(video1_frame);

                imageRechts = new Bitmap("c://temp000.bmp");
                if (ImagesVergelijken())
                {
                    NieuweImageInitialiseren();
                    pictureBox1.Size = image.Size;
                    pictureBox1.Image = image;
                }
            }
            //avi laden
            else
            {*/
                //videobestanden en/of webcams laden en achtergrond bepalen
     /*           VideoLadenEnAchtergrondBepalen();

                //kijken waar de bitmaps over elkaar gaan
                //als niets overeenkomt, achtergronden opslaan en niets doen
                if (!ImagesVergelijken())
                {
                    MessageBox.Show("Geen vergelijking gevonden, afbeeldingen opgeslagen (c:/Achtergrond_Rechts en c:/Achtergrond_Links)");
                    imageLinks.Save("c://Achtergrond_Links.bmp");
                    imageRechts.Save("c://Achtergrong_Rechts.bmp");
                }
                else
                {*/
                    //             try
          //          {


                        //eerste keer moeten de afmetingen van de image bepaald worden
    //                    NieuweImageInitialiseren();
                        
                        

                        //voor altijd de streams samenvoegen en doorsturen
    /*                    while (true)
                        {
                            //wachten totdat er nieuwe frames zijn
                            if (imageLinks != null && imageRechts != null)
                            {
                                //images samenvoegen
                                ImagesSamenvoegen();
                                //image verzend
                                ImageVerzenden();
                                //wachten op nieuwe images
                                imageLinks = imageRechts = null;
                                //motion detection
                            }
                        }
                    }
                    //              catch
                    {
                        MessageBox.Show("Geen verbinding met de Server.");
                    }
                }
     //       }

        }*/ 
    }
}