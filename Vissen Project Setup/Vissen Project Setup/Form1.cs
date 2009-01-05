using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Vissen_Project_Setup
{
    public partial class Form1 : Form
    {
        private static string pathSettings = Application.StartupPath + "\\Settings";
        private static string pathSettingsFile = pathSettings + "\\aquaConfig.txt";

        private List<string> modellenVis;
        private List<string> modellenObject;

        public Form1()
        {
            InitializeComponent();

            LaadProgramma();
        }

        private void CheckDirectory(string path)
        {
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
        }

        private void LaadProgramma()
        {
            //modellen
            string path = Application.StartupPath + "\\Modellen";
            CheckDirectory(path);
            LaadModellen(path);

            //gegevens laden
            CheckDirectory(pathSettings);
            LaadGegevens(pathSettingsFile);
        }

        private void LaadModellen(string path)
        {
            //vissen
            CheckDirectory(path + "\\Vissen");
            DirectoryInfo di = new DirectoryInfo(path + "\\Vissen");
            modellenVis = new List<string>();
            foreach (FileInfo fi in di.GetFiles("*.txt"))
            {
                string filename = Path.GetFileNameWithoutExtension(fi.Name);
                cbVisTypes.Items.Add(filename);
                modellenVis.Add(filename);
            }

            //objecten
            CheckDirectory(path + "\\Objecten");
            di = new DirectoryInfo(path + "\\Objecten");
            modellenObject = new List<string>();
            foreach (FileInfo fi in di.GetFiles("*.txt"))
            {
                string filename = Path.GetFileNameWithoutExtension(fi.Name);
                cbObjectTypes.Items.Add(filename);
                modellenObject.Add(filename);
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
                    //aquarium gegevens
                    currentLine = sw.ReadLine();
                    nudScherm1.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudScherm2.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudScherm3.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudLandHoog.Value = Convert.ToDecimal(currentLine);

                    //scholen
                    currentLine = sw.ReadLine();
                    int max = Convert.ToInt32(currentLine);
                    for (int i = 0; i < max; i++)
                    {
                        currentLine = sw.ReadLine();
                        ListViewItem lvi = new ListViewItem(currentLine);
                        if (!modellenVis.Contains(currentLine))
                        {
                            lvi.BackColor = Color.Red;
                        }
                        currentLine = sw.ReadLine();
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudAantal.Minimum, Math.Min(nudAantal.Maximum, Convert.ToDecimal(currentLine)))));
                        lvScholen.Items.Add(lvi);
                    }

                    //objecten
                    currentLine = sw.ReadLine();
                    max = Convert.ToInt32(currentLine);
                    for (int i = 0; i < max; i++)
                    {
                        currentLine = sw.ReadLine();
                        ListViewItem lvi = new ListViewItem(currentLine);
                        if (!modellenObject.Contains(currentLine))
                        {
                            lvi.BackColor = Color.Red;
                        }
                        currentLine = sw.ReadLine();
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudPlantX.Minimum, Math.Min(nudPlantX.Maximum, Convert.ToDecimal(currentLine)))));
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudPlantY.Minimum, Math.Min(nudPlantY.Maximum, Convert.ToDecimal(currentLine)))));
                        lvObjecten.Items.Add(lvi);
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
                    //aquarium gegevens
                    sw.WriteLine(Convert.ToString(nudScherm1.Value));
                    sw.WriteLine(Convert.ToString(nudScherm2.Value));
                    sw.WriteLine(Convert.ToString(nudScherm3.Value));
                    sw.WriteLine(Convert.ToString(nudLandHoog.Value));

                    //scholen
                    sw.WriteLine(Convert.ToString(lvScholen.Items.Count));
                    for (int i = 0; i < lvScholen.Items.Count; i++)
                    {
                        sw.WriteLine(lvScholen.Items[i].Text);
                        sw.WriteLine(lvScholen.Items[i].SubItems[1].Text);
                    }

                    //objecten
                    sw.WriteLine(Convert.ToString(lvObjecten.Items.Count));
                    for (int i = 0; i < lvObjecten.Items.Count; i++)
                    {
                        sw.WriteLine(lvObjecten.Items[i].Text);
                        sw.WriteLine(lvObjecten.Items[i].SubItems[1].Text);
                        sw.WriteLine(lvObjecten.Items[i].SubItems[2].Text);
                    }
                }
                finally
                {
                    sw.Close();
                }
            }
            catch { }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            CheckDirectory(pathSettings);
            SchrijfGegevens(pathSettingsFile);

            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ListViewItem lvi = new ListViewItem();
            if (modellenVis.Count == 0)
            {
                lvi.Text = "";
                lvi.BackColor = Color.Red;
            }
            else
            {
                lvi.Text = modellenVis[0];
            }
            lvi.SubItems.Add("1");
            lvScholen.Items.Add(lvi);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            lvScholen.SelectedItems[0].Remove();
        }

        private void lvScholen_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            gbInstellingen.Enabled = true;
            if (modellenVis.Contains(lvScholen.SelectedItems[0].Text))
            {
                cbVisTypes.SelectedIndex = modellenVis.IndexOf(lvScholen.SelectedItems[0].Text);
            }
            nudAantal.Value = Convert.ToDecimal(lvScholen.SelectedItems[0].SubItems[1].Text);
        }

        private void cbVisTypes_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            lvScholen.SelectedItems[0].BackColor = SystemColors.Window;
            lvScholen.SelectedItems[0].Text = (string)cbVisTypes.Items[cbVisTypes.SelectedIndex];
        }

        private void nudAantal_ValueChanged(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            lvScholen.SelectedItems[0].SubItems[1].Text = Convert.ToString(nudAantal.Value);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            ListViewItem lvi = new ListViewItem();
            if (modellenObject.Count == 0)
            {
                lvi.Text = "";
                lvi.BackColor = Color.Red;
            }
            else
            {
                lvi.Text = modellenObject[0];
            }
            lvi.SubItems.Add("0");
            lvi.SubItems.Add("0");
            lvObjecten.Items.Add(lvi);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            lvObjecten.SelectedItems[0].Remove();
        }

        private void cbObjectTypes_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            lvObjecten.SelectedItems[0].BackColor = SystemColors.Window;
            lvObjecten.SelectedItems[0].Text = (string)cbObjectTypes.Items[cbObjectTypes.SelectedIndex];
        }

        private void nudPlantX_ValueChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            lvObjecten.SelectedItems[0].SubItems[1].Text = Convert.ToString(nudPlantX.Value);
        }

        private void nudPlantY_ValueChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            lvObjecten.SelectedItems[0].SubItems[2].Text = Convert.ToString(nudPlantY.Value);
        }

        private void lvObjecten_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            gbInstellingen2.Enabled = true;
            if (modellenObject.Contains(lvObjecten.SelectedItems[0].Text))
            {
                cbObjectTypes.SelectedIndex = modellenObject.IndexOf(lvObjecten.SelectedItems[0].Text);
            }
            nudPlantX.Value = Convert.ToDecimal(lvObjecten.SelectedItems[0].SubItems[1].Text);
            nudPlantY.Value = Convert.ToDecimal(lvObjecten.SelectedItems[0].SubItems[2].Text);
        }
    }
}
