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

        private List<string> objectenVis;
        private List<string> modellenVis;
        private List<string> objectenObject;
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
            string path = Application.StartupPath + "\\Data";
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
            objectenVis = new List<string>();
            foreach (FileInfo fi in di.GetFiles("*.oif"))
            {
                string filename = Path.GetFileNameWithoutExtension(fi.Name);
                cbVisObjects.Items.Add(filename);
                objectenVis.Add(filename);
            }

            CheckDirectory(path + "/Vissen/Modellen");
            di = new DirectoryInfo(path + "/Vissen/Modellen");
            modellenVis = new List<string>();
            foreach (FileInfo fi in di.GetFiles("*.txt"))
            {
                string filename = Path.GetFileNameWithoutExtension(fi.Name);
                cbVisModels.Items.Add(filename);
                modellenVis.Add(filename);
            }

            //objecten
            CheckDirectory(path + "\\Objecten");
            di = new DirectoryInfo(path + "\\Objecten");
            objectenObject = new List<string>();
            foreach (FileInfo fi in di.GetFiles("*.oif"))
            {
                string filename = Path.GetFileNameWithoutExtension(fi.Name);
                cbObjectObjects.Items.Add(filename);
                objectenObject.Add(filename);
            }

            CheckDirectory(path + "/Objecten/Modellen");
            di = new DirectoryInfo(path + "/Objecten/Modellen");
            modellenObject = new List<string>();
            foreach (FileInfo fi in di.GetFiles("*.txt"))
            {
                string filename = Path.GetFileNameWithoutExtension(fi.Name);
                cbObjectModels.Items.Add(filename);
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
                    //Scherm resolutie
                    currentLine = sw.ReadLine();
                    nudScherm1.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudScherm2.Value = Convert.ToDecimal(currentLine);

                    //Scherm Positie
                    currentLine = sw.ReadLine();
                    nudSchermX.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudSchermY.Value = Convert.ToDecimal(currentLine);

                    //Aquarium groote
                    currentLine = sw.ReadLine();
                    nudAquarium1.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudAquarium2.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudAquarium3.Value = Convert.ToDecimal(currentLine);

                    //Zwem regio
                    currentLine = sw.ReadLine();
                    nudRegio1.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudRegio2.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudRegio3.Value = Convert.ToDecimal(currentLine);

                    //Balk
                    currentLine = sw.ReadLine();
                    nudBalk.Value = Convert.ToDecimal(currentLine);

                    //Camera opties
                    currentLine = sw.ReadLine();
                    nudCamera1.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudCamera2.Value = Convert.ToDecimal(currentLine);
                    currentLine = sw.ReadLine();
                    nudCamera3.Value = Convert.ToDecimal(currentLine);

                    //Land hoogte
                    currentLine = sw.ReadLine();
                    nudLandHoog.Value = Convert.ToDecimal(currentLine);

                    //Scholen
                    currentLine = sw.ReadLine();
                    int max = Convert.ToInt32(currentLine);
                    for (int i = 0; i < max; i++)
                    {
                        currentLine = sw.ReadLine();
                        string currentLine2 = sw.ReadLine();
                        ListViewItem lvi = new ListViewItem(currentLine2);
                        if (!objectenVis.Contains(currentLine2))
                        {
                            lvi.BackColor = Color.Red;
                        }
                        
                        lvi.SubItems.Add(currentLine);
                        if (!modellenVis.Contains(currentLine))
                        {
                            lvi.SubItems[1].BackColor = Color.Red;
                        }
                        currentLine = sw.ReadLine();
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudAantal.Minimum, Math.Min(nudAantal.Maximum, Convert.ToDecimal(currentLine)))));
                        lvScholen.Items.Add(lvi);
                    }

                    //Objecten
                    currentLine = sw.ReadLine();
                    max = Convert.ToInt32(currentLine);
                    for (int i = 0; i < max; i++)
                    {
                        currentLine = sw.ReadLine();
                        string currentLine2 = sw.ReadLine();
                        ListViewItem lvi = new ListViewItem(currentLine2);
                        if (!objectenObject.Contains(currentLine2))
                        {
                            lvi.BackColor = Color.Red;
                        }

                        lvi.SubItems.Add(currentLine);
                        if (!modellenVis.Contains(currentLine))
                        {
                            lvi.SubItems[1].BackColor = Color.Red;
                        }
                        currentLine = sw.ReadLine();
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudPlantX.Minimum, Math.Min(nudPlantX.Maximum, Convert.ToDecimal(currentLine)))));
                        currentLine = sw.ReadLine();
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudPlantY.Minimum, Math.Min(nudPlantY.Maximum, Convert.ToDecimal(currentLine)))));
                        lvObjecten.Items.Add(lvi);
                    }

                    //Bubbels
                    currentLine = sw.ReadLine();
                    max = Convert.ToInt32(currentLine);
                    for (int i = 0; i < max; i++)
                    {
                        ListViewItem lvi = new ListViewItem();
                        currentLine = sw.ReadLine();
                        lvi.Text = Convert.ToString(Math.Max(nudPlantX.Minimum, Math.Min(nudPlantX.Maximum, Convert.ToDecimal(currentLine))));
                        currentLine = sw.ReadLine();
                        lvi.SubItems.Add(Convert.ToString(Math.Max(nudPlantY.Minimum, Math.Min(nudPlantY.Maximum, Convert.ToDecimal(currentLine)))));
                        lvBubbels.Items.Add(lvi);
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
                bestand = new FileStream(path, FileMode.Create, FileAccess.Write);
                sw = new StreamWriter(bestand);

                try
                {
                    //Scherm resolutie
                    sw.WriteLine(Convert.ToString(nudScherm1.Value));
                    sw.WriteLine(Convert.ToString(nudScherm2.Value));

                    //Scherm Positie
                    sw.WriteLine(Convert.ToString(nudSchermX.Value));
                    sw.WriteLine(Convert.ToString(nudSchermY.Value));

                    //Aquarium groote
                    sw.WriteLine(Convert.ToString(nudAquarium1.Value));
                    sw.WriteLine(Convert.ToString(nudAquarium2.Value));
                    sw.WriteLine(Convert.ToString(nudAquarium3.Value));

                    //Zwem regio
                    sw.WriteLine(Convert.ToString(nudRegio1.Value));
                    sw.WriteLine(Convert.ToString(nudRegio2.Value));
                    sw.WriteLine(Convert.ToString(nudRegio3.Value));

                    //Balk
                    sw.WriteLine(Convert.ToString(nudBalk.Value));

                    //Camera opties
                    sw.WriteLine(Convert.ToString(nudCamera1.Value));
                    sw.WriteLine(Convert.ToString(nudCamera2.Value));
                    sw.WriteLine(Convert.ToString(nudCamera3.Value));

                    //Land hoogte
                    sw.WriteLine(Convert.ToString(nudLandHoog.Value));

                    //Scholen
                    sw.WriteLine(Convert.ToString(lvScholen.Items.Count));
                    for (int i = 0; i < lvScholen.Items.Count; i++)
                    {
                        sw.WriteLine(lvScholen.Items[i].SubItems[1].Text);
                        sw.WriteLine(lvScholen.Items[i].Text);
                        sw.WriteLine(lvScholen.Items[i].SubItems[2].Text);
                    }

                    //Objecten
                    sw.WriteLine(Convert.ToString(lvObjecten.Items.Count));
                    for (int i = 0; i < lvObjecten.Items.Count; i++)
                    {
                        sw.WriteLine(lvObjecten.Items[i].SubItems[1].Text);
                        sw.WriteLine(lvObjecten.Items[i].Text);
                        sw.WriteLine(lvObjecten.Items[i].SubItems[2].Text);
                        sw.WriteLine(lvObjecten.Items[i].SubItems[3].Text);
                    }

                    //Bubbels
                    sw.WriteLine(Convert.ToString(lvBubbels.Items.Count));
                    for (int i = 0; i < lvBubbels.Items.Count; i++)
                    {
                        sw.WriteLine(lvBubbels.Items[i].Text);
                        sw.WriteLine(lvBubbels.Items[i].SubItems[1].Text);
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
            if (objectenVis.Count == 0)
            {
                lvi.Text = "";
                lvi.BackColor = Color.Red;
            }
            else
            {
                lvi.Text = objectenVis[0];
            }
            if (modellenVis.Count == 0)
            {
                lvi.SubItems.Add(modellenVis[0]).BackColor = Color.Red;
            }
            else
            {
                lvi.SubItems.Add(modellenVis[0]);
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
            
            if (objectenVis.Contains(lvScholen.SelectedItems[0].Text))
            {
                cbVisObjects.SelectedIndex = objectenVis.IndexOf(lvScholen.SelectedItems[0].Text);
            }
            if (modellenVis.Contains(lvScholen.SelectedItems[0].SubItems[1].Text))
            {
                cbVisModels.SelectedIndex = modellenVis.IndexOf(lvScholen.SelectedItems[0].SubItems[1].Text);
            }
            nudAantal.Value = Convert.ToDecimal(lvScholen.SelectedItems[0].SubItems[2].Text);
        }

        private void cbVisTypes_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            lvScholen.SelectedItems[0].BackColor = SystemColors.Window;
            lvScholen.SelectedItems[0].Text = (string)cbVisObjects.Items[cbVisObjects.SelectedIndex];
        }

        private void nudAantal_ValueChanged(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            lvScholen.SelectedItems[0].SubItems[2].Text = Convert.ToString(nudAantal.Value);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            ListViewItem lvi = new ListViewItem();
            if (objectenObject.Count == 0)
            {
                lvi.Text = "";
                lvi.BackColor = Color.Red;
            }
            else
            {
                lvi.Text = objectenObject[0];
            }
            if (modellenObject.Count == 0)
            {
                lvi.SubItems.Add("").BackColor = Color.Red;
            }
            else
            {
                lvi.SubItems.Add(modellenObject[0]);
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
            lvObjecten.SelectedItems[0].Text = (string)cbObjectObjects.Items[cbObjectObjects.SelectedIndex];
        }

        private void nudPlantX_ValueChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            lvObjecten.SelectedItems[0].SubItems[2].Text = Convert.ToString(nudPlantX.Value);
        }

        private void nudPlantY_ValueChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            lvObjecten.SelectedItems[0].SubItems[3].Text = Convert.ToString(nudPlantY.Value);
        }

        private void cbObjectModels_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            if (cbObjectModels.SelectedIndex < 0)
            {
                return;
            }
            lvObjecten.SelectedItems[0].SubItems[1].BackColor = SystemColors.Window;
            lvObjecten.SelectedItems[0].SubItems[1].Text = (string)cbObjectModels.Items[cbObjectModels.SelectedIndex];
        }

        private void cbVisModels_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvScholen.SelectedItems.Count == 0)
            {
                gbInstellingen.Enabled = false;
                return;
            }
            if (cbVisModels.SelectedIndex < 0)
            {
                return;
            }
            lvScholen.SelectedItems[0].SubItems[1].BackColor = SystemColors.Window;
            lvScholen.SelectedItems[0].SubItems[1].Text = (string)cbVisModels.Items[cbVisModels.SelectedIndex];
        }

        private void button7_Click(object sender, EventArgs e)
        {
            nudRegio1.Value = nudAquarium1.Value - numericUpDown4.Value * 2;
            nudRegio2.Value = nudAquarium2.Value - numericUpDown4.Value * 2;
            nudRegio3.Value = nudAquarium3.Value - numericUpDown4.Value * 2;
        }

        private void button8_Click(object sender, EventArgs e)
        {
            nudAquarium3.Value = nudAquarium1.Value / 2;
            nudAquarium2.Value = (nudScherm2.Value / nudScherm1.Value) * nudAquarium3.Value;
        }

        private void button11_Click(object sender, EventArgs e)
        {
            ListViewItem lvi = new ListViewItem();
            lvi.Text = "0";
            lvi.SubItems.Add("0");
            lvBubbels.Items.Add(lvi);
        }

        private void button12_Click(object sender, EventArgs e)
        {
            if (lvBubbels.SelectedItems.Count == 0)
            {
                gbInstellingen3.Enabled = false;
                return;
            }
            lvBubbels.SelectedItems[0].Remove();
        }

        private void lvBubbels_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvBubbels.SelectedItems.Count == 0)
            {
                gbInstellingen3.Enabled = false;
                return;
            }
            gbInstellingen3.Enabled = true;

            nudBubbelX.Value = Convert.ToDecimal(lvBubbels.SelectedItems[0].Text);
            nudBubbelY.Value = Convert.ToDecimal(lvBubbels.SelectedItems[0].SubItems[1].Text);
        }

        private void numericUpDown7_ValueChanged(object sender, EventArgs e)
        {
            if (lvBubbels.SelectedItems.Count == 0)
            {
                gbInstellingen3.Enabled = false;
                return;
            }
            lvBubbels.SelectedItems[0].SubItems[1].Text = Convert.ToString(nudBubbelY.Value);
        }

        private void nudBubbelX_ValueChanged(object sender, EventArgs e)
        {
            if (lvBubbels.SelectedItems.Count == 0)
            {
                gbInstellingen3.Enabled = false;
                return;
            }
            lvBubbels.SelectedItems[0].Text = Convert.ToString(nudBubbelX.Value);
        }

        private void lvObjecten_SelectedIndexChanged_1(object sender, EventArgs e)
        {
            if (lvObjecten.SelectedItems.Count == 0)
            {
                gbInstellingen2.Enabled = false;
                return;
            }
            gbInstellingen2.Enabled = true;

            if (objectenObject.Contains(lvObjecten.SelectedItems[0].Text))
            {
                cbObjectObjects.SelectedIndex = objectenObject.IndexOf(lvObjecten.SelectedItems[0].Text);
            }
            if (modellenObject.Contains(lvObjecten.SelectedItems[0].SubItems[1].Text))
            {
                cbObjectModels.SelectedIndex = modellenObject.IndexOf(lvObjecten.SelectedItems[0].SubItems[1].Text);
            }
            nudPlantX.Value = Convert.ToDecimal(lvObjecten.SelectedItems[0].SubItems[2].Text);
            nudPlantY.Value = Convert.ToDecimal(lvObjecten.SelectedItems[0].SubItems[3].Text);
        }
    }
}
