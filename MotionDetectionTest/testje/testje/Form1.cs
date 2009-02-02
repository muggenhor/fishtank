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
        private int totaal = 0, totaal2=0, fail=0, fail1=0, fail2=0, fail3=0;
        private int totaal1 = 0, totaal3=0, fai4=0, fail5=0, fail6=0, fail7=0;
        private Bitmap image, image2;

        public Form1()
        {
            InitializeComponent();
            panel1.Size = new Size(5, 5);
            panel2.Size = new Size(5, 5);
            panel1.BackColor = Color.Blue;
            panel2.BackColor = Color.Red;
            pictureBox1.Location = new Point(0, 0);
            pictureBox1.Location = new Point(0, 350);
            timer1.Enabled = true;
     //       timer2.Enabled = true;          
       }

        private void timer1_Tick(object sender, EventArgs e)
        {
            byte[] temp;
            try
            {
                if (eerste)
                {
                    Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                    s.Bind(new IPEndPoint(IPAddress.Any, 1234));
                    s.Listen(1000);
                    s2 = s.Accept();
                    eerste = false;
                    label5.Text += " timer 1";
                }

                temp = new byte[1500000];
                s2.Receive(temp);
                if (temp[0] == 66 && temp[1] == 77) //een bitmap
                {
                    try
                    {
                        MemoryStream ms = new MemoryStream(temp);
                        ms.Flush();
                        image = (Bitmap)Bitmap.FromStream(ms);
                        ms.Close();

                        pictureBox1.Size = image.Size;
                        pictureBox1.Image = image;
                        label1.Text = "ontvangen: " + totaal++;
                    }
                    catch
                    {
                        label4.Text = "misluk: " + ++fail;
                    }
                }
                else if (temp[2] == 0 && temp[3] == 0 && temp[4] == 0)
                {
                    try
                    {
                        int x = (int)temp[0]*pictureBox1.Width/100;
                        int y = (int)temp[1]*pictureBox1.Height/100;
                        panel1.Location = new Point(x, y);
                        label2.Text = "coordinaten: " + totaal2++;
                    }
                    catch
                    {
                        label3.Text = "mislukt: " + ++fail1;
                    }

                }
                else
                {
                    label5.Text = "troep: " + ++fail2;
                }
            }
            catch { label5.Text = "TOTALE ERRORS: " + fail3++; }

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            byte[] temp;
            try
            {
                if (eerste1)
                {
                    Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                    s.Bind(new IPEndPoint(IPAddress.Any, 1235));
                    s.Listen(1000);
                    s3 = s.Accept();
                    eerste = false;
                    label10.Text += " timer 1";
                }

                temp = new byte[1500000];
                s3.Receive(temp);
                if (temp[0] == 66 && temp[1] == 77) //een bitmap
                {
                    try
                    {
                        MemoryStream ms = new MemoryStream(temp);
                        ms.Flush();
                        image2 = (Bitmap)Bitmap.FromStream(ms);
                        ms.Close();


                        pictureBox2.Size = image.Size;
                        pictureBox2.Image = image;
                        label6.Text = "ontvangen: " + totaal++;
                    }
                    catch
                    {
                        label9.Text = "misluk: " + ++fail;
                    }
                }
                else if (temp[2] == 0 && temp[3] == 0 && temp[4] == 0)
                {
                    try
                    {
                        int x = (int)temp[0] * pictureBox2.Width / 100 + 350;
                        int y = (int)temp[1] * pictureBox2.Height / 100;
                        panel2.Location = new Point(x, y);
                        label7.Text = "coordinaten: " + totaal2++;
                    }
                    catch
                    {
                        label8.Text = "mislukt: " + ++fail1;
                    }

                }
                else
                {
                    label10.Text = "troep: " + ++fail2;
                }
            }
            catch { label10.Text = "TOTALE ERRORS: " + fail3++; }
        }
    }
}
