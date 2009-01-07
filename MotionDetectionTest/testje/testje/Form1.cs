using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.IO;

namespace testje
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            panel1.Size = new Size(5, 5);
            panel2.Size = new Size(5, 5);
            panel1.BackColor = Color.Blue;
            panel2.BackColor = Color.Red;
            timer1.Enabled = true;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
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
            catch { }
        }
    }
}
