using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;

namespace TestEyeDetection
{
    public partial class frmEyeTest : Form
    {
        public frmEyeTest()
        {
            InitializeComponent();
        }

        private Bitmap lastPic;
        private clsFaceDetectionWrapper objFaceDetector;
        private Pen myPen = new Pen(Color.Red, 2.0f);
        Graphics picGraphics;          
        

        private void btnBrowse_Click(object sender, EventArgs e)
        {
             
            try
            {
                if(openFileDialog1.ShowDialog() == DialogResult.OK)
                {                
                    this.Text = "Face and Eye Detection Test";
                    Bitmap bmp = new Bitmap(openFileDialog1.FileName);
                    pictureBox1.Image = (Image) bmp;
                    
                    if(lastPic != null)
                        lastPic.Dispose();
                    lastPic = bmp;

                    this.Width = pictureBox1.Width + 40;
                    this.Height = pictureBox1.Height + 105;
                }
            } 
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Unable to open file!"
                    , MessageBoxButtons.OK,MessageBoxIcon.Error
                    , MessageBoxDefaultButton.Button1);
            }
        }

        void DrawRectangle(int lx, int ly, int rx, int ry)
        {
            picGraphics = pictureBox1.CreateGraphics();
            picGraphics.DrawRectangle(myPen, lx, ly, rx-lx, ry-ly);
        }

        void DrawCross(int x, int y)
        {

        }

 

        private void frmEyeTest_Load(object sender, EventArgs e)
        {
            this.Text = "Face and Eye Detection Test";
            objFaceDetector = clsFaceDetectionWrapper.GetSingletonInstance();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Bitmap bmp = new Bitmap("D:\\FaceTest\\lena1.jpg"); // hier de bitmap laden van cam's

            pictureBox1.Image = (Image)bmp;
            lastPic = bmp;

            if (lastPic != null)
            {
                int faces = objFaceDetector.WrapDetectFaces(lastPic);
                this.Text = "Number of faces : " + faces.ToString();

                unsafe
                {
                    picGraphics = pictureBox1.CreateGraphics();
                    int lx, ly, rx, ry, res;
                    for (int f = 0; f < faces; f++)
                    {
                        objFaceDetector.WrapGetFaceCordinates(f, &lx, &ly, &rx, &ry);
                        lblLX.Text = Convert.ToString(lx);
                        lblRX.Text = Convert.ToString(rx);
                        lblLY.Text = Convert.ToString(ly);
                        lblRY.Text = Convert.ToString(ry);

                        //         picGraphics.DrawRectangle(myPen, lx, ly, rx, ry);

                        //         res = objFaceDetector.WrapGetEyeCordinates(f, &lx, &ly, &rx, &ry);

                        //          if (res > -1)
                        //         {
                        //    picGraphics.DrawLine(myPen, lx - 5, ly, lx + 5, ly);
                        //    picGraphics.DrawLine(myPen, lx, ly - 5, lx, ly + 5);

                        //    picGraphics.DrawLine(myPen, rx - 5, ry, rx + 5, ry);
                        //    picGraphics.DrawLine(myPen, rx, ry - 5, rx, ry + 5);
                        //}
                        //     }
                    }
                    picGraphics.Save();
                    picGraphics.Dispose();
                }
            }
        }


    }
}