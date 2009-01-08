using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Vissen_Project_Object_File_Generator
{
    public partial class Form1 : Form
    {
        private static string pathVis = Application.StartupPath + "/Data/Vissen";
        private static string pathObjects = Application.StartupPath + "/Data/Objecten";
        private static string fileExt = ".oif";

        public Form1()
        {
            InitializeComponent();
        }

        private void CheckDirectory(string path)
        {
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
        }

        private void LaadGegevens(string path)
        {
            FileStream bestand;
            StreamReader sw;

            try
            {
                bestand = new FileStream(path, FileMode.Open, FileAccess.Read);
                sw = new StreamReader(bestand);

                string currentLine;

                try
                {
                    //currentLine = sw.ReadLine();
                    //tbPath.Text = currentLine;

                    currentLine = sw.ReadLine();
                    nudModelSchaal.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudMaxAfwijk.Value = Convert.ToDecimal(currentLine);

                    rbVis.Checked = true;
                    try
                    {
                        currentLine = sw.ReadLine();
                        nudMinSnelheid.Value = Convert.ToDecimal(currentLine);
                        currentLine = sw.ReadLine();
                        nudMaxSnelheid.Value = Convert.ToDecimal(currentLine);
                        currentLine = sw.ReadLine();
                        nudBewegingFrequentie.Value = Convert.ToDecimal(currentLine);
                        currentLine = sw.ReadLine();
                        nudBewegingFactor.Value = Convert.ToDecimal(currentLine);
                        currentLine = sw.ReadLine();
                        nudDraaiSnelheid.Value = Convert.ToDecimal(currentLine);
                        currentLine = sw.ReadLine();
                        nudDraaiVersnelling.Value = Convert.ToDecimal(currentLine);
                    }
                    catch
                    {
                        rbObject.Checked = true;
                    }
                }
                finally
                {
                    sw.Close();
                }
            }
            catch { }
        }

        private void SchrijfGegevens(string path)
        {
            FileStream bestand;
            StreamWriter sw;

            try
            {
                bestand = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write);
                sw = new StreamWriter(bestand);

                try
                {
                    //sw.WriteLine(tbPath.Text);

                    sw.WriteLine(Convert.ToString(nudModelSchaal.Value));
                    sw.WriteLine(Convert.ToString(nudMaxAfwijk.Value));

                    if (rbVis.Checked)
                    {
                        sw.WriteLine(Convert.ToString(nudMinSnelheid.Value));
                        sw.WriteLine(Convert.ToString(nudMaxSnelheid.Value));
                        sw.WriteLine(Convert.ToString(nudBewegingFrequentie.Value));
                        sw.WriteLine(Convert.ToString(nudBewegingFactor.Value));
                        sw.WriteLine(Convert.ToString(nudDraaiSnelheid.Value));
                        sw.WriteLine(Convert.ToString(nudDraaiVersnelling.Value));
                    }
                }
                finally
                {
                    sw.Close();
                }
            }
            catch { }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            /*if (!File.Exists(tbPath.Text))
            {
                MessageBox.Show("De model file bestaat niet", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            string filename = "/" + Path.GetFileNameWithoutExtension(tbPath.Text) + fileExt;*/
            string filename;
            FormText ft = new FormText();
            DialogResult result = ft.ShowDialog();
            if (result != DialogResult.Cancel)
            {
                filename = ft.Textje;
                if (filename == "")
                {
                    return;
                }
            }
            else
            {
                return;
            }

            if (rbVis.Checked)
            {
                CheckDirectory(pathVis);
                SchrijfGegevens(pathVis + "/" + filename + fileExt);
            }
            else
            {
                CheckDirectory(pathObjects);
                SchrijfGegevens(pathObjects + "/" + filename + fileExt);
            }

            Close();
        }

        private void rbObject_CheckedChanged(object sender, EventArgs e)
        {
            gbVis.Enabled = !rbObject.Checked;
            if (rbObject.Checked)
            {
                lblSize.Text = "Plant Hoogte";
            }
            else
            {
                lblSize.Text = "Vis Lengte";
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.InitialDirectory = Application.StartupPath;
            ofd.FileName = "";
            ofd.Multiselect = false;
            ofd.Title = "Import Model";
            ofd.Filter = "Model files|*" + fileExt + ";|" +
                                "All files|*.*";
            DialogResult result = ofd.ShowDialog();
            if (result != DialogResult.Cancel)
            {
                tbPath.Text = ofd.FileName;
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.InitialDirectory = Application.StartupPath;
            ofd.FileName = "";
            ofd.Multiselect = false;
            ofd.Title = "Import File";
            ofd.Filter = //"Wav files|*.wav;|" +
                                "All files|*.*";
            DialogResult result = ofd.ShowDialog();
            if (result != DialogResult.Cancel)
            {
                LaadGegevens(ofd.FileName);
            }
        }
    }
}
