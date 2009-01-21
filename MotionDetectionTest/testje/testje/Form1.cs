using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.IO;
using System.Net.Sockets;
using System.Net;

namespace testje
{
    public partial class Form1 : Form
    {
        private bool eerste = true, eerste1 = true;
        private Socket s2, s3;
        private int totaal = 0, totaal2=0, fail=0, fail1=0;
        private Bitmap image;

        public Form1()
        {
            InitializeComponent();
            panel1.Size = new Size(5, 5);
            panel2.Size = new Size(5, 5);
            panel1.BackColor = Color.Blue;
            panel2.BackColor = Color.Red;

            //plaatjes
            
            //coordinaten


            timer1.Enabled = true;
            timer2.Enabled = true;
            
       }

        private void timer1_Tick(object sender, EventArgs e)
        {

      /*      try
            {
                FileStream fs = new FileStream("c://temp2.txt", FileMode.Open, FileAccess.Read);
                StreamReader sr = new StreamReader(fs);
                int x = (int)(Convert.ToDouble(sr.ReadLine()) * Convert.ToDouble(this.Size.Width));
                int y = (int)(Convert.ToDouble(sr.ReadLine()) * Convert.ToDouble(this.Size.Height));
                sr.Close();
                panel2.Location = new Point(x, y);
            }
            catch { }
            try
            {
                FileStream fs = new FileStream("c://temp.txt", FileMode.Open, FileAccess.Read);
                StreamReader sr = new StreamReader(fs);              
                int x = (int)(Convert.ToDouble(sr.ReadLine()) * Convert.ToDouble(this.Size.Width));
                int y = (int)(Convert.ToDouble(sr.ReadLine()) * Convert.ToDouble(this.Size.Height));
                sr.Close();
                panel1.Location = new Point(x, y);               
            }
            catch { }*/

            try
            {
                if (eerste)
                {
                    Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                    s.Bind(new IPEndPoint(IPAddress.Any, 1234));
                    s.Listen(10);
                    s2 = s.Accept();
                    eerste = false;
                }

      //          System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
        //        System.IO.MemoryStream ms;
          //      byte[] temp = new byte[284364]; //<- uiteindelijk krijgt die het binnen van de client
                
        //        int aantal = 0;
          //      aantal = s2.Receive(temp);
            //    ms = new MemoryStream(temp);
              //  image = (Bitmap)bf.Deserialize(ms);
                byte[] temp = new byte[2000000];
                s2.Receive(temp);
                MemoryStream ms = new MemoryStream(temp);
                image = (Bitmap)Bitmap.FromStream(ms);
                ms.Flush();

      /*          Bitmap tempBit = new Bitmap(image.Width * 2, image.Height * 2);
                for (int x = 0; x < tempBit.Width; x++)
                    for (int y = 0; y < tempBit.Height; y++)
                        tempBit.SetPixel(x, y, image.GetPixel(x/2, y/2));*/


                pictureBox1.Size = image.Size;
                pictureBox1.Image = image;
                label1.Text = "ontvangen: " + totaal++;
            }
            catch { label4.Text = "misluk: " + ++fail;}

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            try
            {
                if (eerste1)
                {
                    Socket s4 = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                    s4.Bind(new IPEndPoint(IPAddress.Any, 1235));
                    s4.Listen(10);
                    s3 = s4.Accept();
                    eerste1 = false;
                }

                byte[] temp = new byte[2];
                s3.Receive(temp);


                pictureBox1.Size = image.Size;
                pictureBox1.Image = image;
                label2.Text = "coordinaten: " + totaal2++;
            }
            catch
            {
                label3.Text = "mislukt: " + ++fail1;
            }
        }
    }
}
