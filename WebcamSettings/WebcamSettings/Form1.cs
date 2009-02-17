using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace WebcamSettings
{
    public partial class Form1 : Form
    {
        private string[] data;

        public Form1()
        {
            InitializeComponent();

            //je mag niet resizen
            this.MaximizeBox = this.MinimizeBox = false;
            this.MaximumSize = this.MinimumSize = this.Size;

            //data uit textbestand halen            
            GegevensOphalen();
        }

        private void GegevensOphalen()
        {
            data = new string[30];
            StreamReader sr = null;
            //openen
            try
            {
                sr = File.OpenText("settings.txt");
            }
            catch
            {
                MessageBox.Show("Kan geen settings.txt vinden, er is nu nieuwe aangemaakt met de default waardes");
                StreamWriter sw = new StreamWriter("settings.txt", true);
                sw.WriteLine(0); sw.WriteLine(1); sw.WriteLine(2);
                sw.WriteLine(3); sw.WriteLine("127.0.0.1"); sw.WriteLine(7781);
                sw.WriteLine(7778); sw.WriteLine(7779); sw.WriteLine(7780);
                sw.WriteLine(10); sw.WriteLine(25); sw.WriteLine(20);
                sw.WriteLine(500); sw.WriteLine(25); sw.WriteLine(5);
                sw.WriteLine(80); sw.WriteLine(0.8); sw.WriteLine(30);
                sw.WriteLine(2); sw.WriteLine(100);
                sw.WriteLine(100); sw.WriteLine(100); sw.WriteLine(100);
                sw.WriteLine(100); sw.WriteLine(100); sw.WriteLine(100);
                sw.WriteLine(100); sw.WriteLine(100);
                sw.WriteLine(1); sw.WriteLine(160); sw.Close();
                sr = File.OpenText("settings.txt");
            }
            //algemene instellingen
            tbWebKort.Text = data[0] = sr.ReadLine();
            tbWebLangL.Text = data[1] = sr.ReadLine();
            tbWebLangM.Text = data[2] = sr.ReadLine();
            tbWebLangR.Text = data[3] = sr.ReadLine();
            tbIpAddres.Text = data[4] = sr.ReadLine();
            tbPoortFace.Text = data[5] = sr.ReadLine();
            tbPoortKort.Text = data[6] = sr.ReadLine();
            tbPoortLang.Text = data[7] = sr.ReadLine();
            tbPoortMotion.Text = data[8] = sr.ReadLine();
            //uitgebreide instellingen
            tbAchtAantalPixels.Text = data[9] = sr.ReadLine();
            tbAchtFrames.Text = data[10] = sr.ReadLine();
            tbAchtVersPixels.Text = data[11] = sr.ReadLine();
            tbBeweInterval.Text = data[12] = sr.ReadLine();
            tbBeweVersPixels.Text = data[13] = sr.ReadLine();
            tbStreamAantalKolom.Text = data[14] = sr.ReadLine();
            tbStreamKolomHoogte.Text = data[15] = sr.ReadLine();
            tbStreamPerc.Text = data[16] = Convert.ToString(Convert.ToDouble(sr.ReadLine()) * 100.0);
            tbStreamVersPixels.Text = data[17] = sr.ReadLine();
            doWebcams.Text = data[18] = sr.ReadLine();
            tbFaceInterval.Text = data[19] = sr.ReadLine();
            tbBeeKortBov.Text = data[20] = sr.ReadLine();
            tbBeeKortOnd.Text = data[21] = sr.ReadLine();
            tbBeeKortLin.Text = data[22] = sr.ReadLine();
            tbBeeKortRec.Text = data[23] = sr.ReadLine();
            tbBeeLanBov.Text = data[24] = sr.ReadLine();
            tbBeeLanOnd.Text = data[25] = sr.ReadLine();
            tbBeeLanLin.Text = data[26] = sr.ReadLine();
            tbBeeLanRec.Text = data[27] = sr.ReadLine();
            if ((data[28] = sr.ReadLine()) == "1") //handmatig
            {
                cbBeeHand.Checked = true;
                label17.Visible = label18.Visible = label19.Visible = label22.Visible = false;
                tbStreamAantalKolom.Visible = tbStreamKolomHoogte.Visible = tbStreamPerc.Visible = tbStreamVersPixels.Visible = false;
                label17.MouseHover -= label1_MouseHover;
                label18.MouseHover -= label1_MouseHover;
                label19.MouseHover -= label1_MouseHover;
                label22.MouseHover -= label1_MouseHover;
                tbStreamVersPixels.MouseHover -= tbWebKort_MouseHover;
                tbStreamAantalKolom.MouseHover -= tbWebKort_MouseHover;
                tbStreamKolomHoogte.MouseHover -= tbWebKort_MouseHover;
                tbStreamPerc.MouseHover -= tbWebKort_MouseHover;
            }
            else
            {
                cbBeeHand.Checked = false;
                label40.Visible = tbBeeTot.Visible = false;
                tbBeeTot.MouseHover -= tbWebKort_MouseHover;
                label40.MouseHover -= label1_MouseHover;
            }
            tbBeeTot.Text = data[29] = sr.ReadLine();
            //afsluiten
            sr.Close();
        }

        private void btSaveUitgebreid_Click(object sender, EventArgs e)
        {
            //algemene instellingen opslaan
            if (((Button)sender).Name == "btSaveAlgemeen")
            {
                int[] datat = new int[9];
 
                //kijken of alles klopt
                //heel simpel kijken of het allemaal getallen zijn
                try
                {
                    datat[0] = Convert.ToInt32(tbWebKort.Text);
                    datat[1] = Convert.ToInt32(tbWebLangL.Text);
                    datat[2] = Convert.ToInt32(tbWebLangM.Text);
                    datat[3] = Convert.ToInt32(tbWebLangR.Text);
                    datat[5] = Convert.ToInt32(tbPoortFace.Text);
                    datat[6] = Convert.ToInt32(tbPoortKort.Text);
                    datat[7] = Convert.ToInt32(tbPoortLang.Text);
                    datat[8] = Convert.ToInt32(tbPoortMotion.Text);   
                }
                catch 
                { 
                    MessageBox.Show("Alleen getallen zijn toegestaan."); 
                    return; 
                }
                //elke webcam mag maar 1 keer voorkomen
                for (int x = 0; x < 3; x++)
                    for (int y = x+1; y < 4; y++)
                        if (datat[x] == datat[y])
                        {
                            MessageBox.Show("Elke webcam mag maar een keer gebruikt worden"); 
                            return;
                        }
                for (int i = 5; i < 9; i++)
                    if (datat[i] < 1 || datat[i] > 9999) { MessageBox.Show("Je moet een poort gebruiken tussen 1 en 9999"); return; }
                //instellingen zijn 'acceptabel'
                data[0] = tbWebKort.Text;
                data[1] = tbWebLangL.Text;
                data[2] = tbWebLangM.Text;
                data[3] = tbWebLangR.Text;
                data[4] = tbIpAddres.Text;
                data[5] = tbPoortFace.Text;
                data[6] = tbPoortKort.Text;
                data[7] = tbPoortLang.Text;
                data[8] = tbPoortMotion.Text; 
            }
            //uitgebreide instellingen opslaan
            else if (((Button)sender).Name == "btSaveUitgebreid")
            {
                int[] datat = new int[10];

                //kijken of alles klopt
                //heel simpel kijken of het allemaal getallen zijn
                try
                {
                    datat[0] = Convert.ToInt32(tbAchtAantalPixels.Text);
                    datat[1] = Convert.ToInt32(tbAchtFrames.Text);
                    datat[2] = Convert.ToInt32(tbAchtVersPixels.Text);
                    datat[3] = Convert.ToInt32(tbBeweInterval.Text);
                    datat[4] = Convert.ToInt32(tbBeweVersPixels.Text);
                    datat[5] = Convert.ToInt32(tbStreamAantalKolom.Text); 
                    datat[6] = Convert.ToInt32(tbStreamKolomHoogte.Text);
                    datat[7] = Convert.ToInt32(tbStreamVersPixels.Text);
                    datat[8] = Convert.ToInt32(tbStreamPerc.Text);
                    datat[9] = Convert.ToInt32(tbFaceInterval.Text);
                }
                catch
                {
                    MessageBox.Show("Alleen getallen zijn toegestaan.");
                    return;
                }
                if (datat[0] < 1 || datat[0] > 20) { MessageBox.Show("Aantal pixels van de achtergrond moet minimaal 1 zijn en maximaal 20"); return; }
                if (datat[1] < 1) { MessageBox.Show("Aantal frames voor het bepalen van de achtergrond moet minimaal 1 zijn"); return;}
                if (datat[2] < 0 || datat[2] > 255) { MessageBox.Show("Het verschil tussen de pixels bij het bepalen van de achtergond moet tussen de 0 en de 255 liggen"); return; }
                if (datat[3] < 1) { MessageBox.Show("De interval van de timer voor de bewegegingdetectie moet minimaal 1 miliseconde zijn"); return; }
                if (datat[4] < 0 || datat[4] > 255) { MessageBox.Show("Het verschil tussen de pixels bij zoeken van beweging moet tussen de 0 en de 255 liggen"); return; }
                if (datat[5] < 1 || datat[5] > 100) { MessageBox.Show("Het aantal kolommen dat word gebruikt bij het maken van 1 stream moet tussen de 1 en de 100 liggen"); return; }
                if (datat[6] < 1 || datat[6] > 100) { MessageBox.Show("Het percentage van een kolom dat gebruikt word moet tussen de 1 en de 100 liggen"); return; }
                if (datat[7] < 0 || datat[7] > 255) { MessageBox.Show("Het verschil tussen de pixels bij het maken van 1 stream moet tussen de 0 en de 255 liggen"); return; }
                if (datat[8] < 0 || datat[8] > 100) { MessageBox.Show("Het percentage dat hetzelfde moet zijn bij het maken van 1 steam moet tussen de 1 en de 100 liggen"); return; }
                if (datat[9] < 1) { MessageBox.Show("De interval van de timer voor de gezichtsdetectie moet minimaal 1 miliseconde zijn"); return; }
                else data[16] = Convert.ToString((double)datat[8] / 100.0);
                        //instellingen zijn 'acceptabel'
                data[9] = tbAchtAantalPixels.Text;
                data[10] = tbAchtFrames.Text;
                data[11] = tbAchtVersPixels.Text;
                data[12] = tbBeweInterval.Text;
                data[13] = tbBeweVersPixels.Text;
                data[14] = tbStreamAantalKolom.Text; 
                data[15] = tbStreamKolomHoogte.Text;
                data[17] = tbStreamVersPixels.Text;
                data[18] = doWebcams.Text;
                data[19] = tbFaceInterval.Text;
                
            }
            //beeld instellingen
            else if (((Button)sender).Name == "btSaveBeeld")
            {
                int[] datat = new int[9];

                //kijken of alles klopt
                //heel simpel kijken of het allemaal getallen zijn
                try
                {
                    datat[0] = Convert.ToInt32(tbBeeKortBov.Text);
                    datat[1] = Convert.ToInt32(tbBeeKortOnd.Text);
                    datat[2] = Convert.ToInt32(tbBeeKortLin.Text);
                    datat[3] = Convert.ToInt32(tbBeeKortRec.Text);
                    datat[4] = Convert.ToInt32(tbBeeLanBov.Text);
                    datat[5] = Convert.ToInt32(tbBeeLanOnd.Text);
                    datat[6] = Convert.ToInt32(tbBeeLanLin.Text);
                    datat[7] = Convert.ToInt32(tbBeeLanRec.Text);
                    datat[8] = Convert.ToInt32(tbBeeTot.Text);
                }
                catch
                {
                    MessageBox.Show("Alleen getallen zijn toegestaan.");
                    return;
                }
                for (int i=0; i<9; i++)
                    if ((datat[i] % 4) != 0)
                    {
                        MessageBox.Show("Alleen getallen die deelbaar zijn door 4 zijn toegstaan.");
                        return;
                    }

                if (cbBeeHand.Checked) data[28] = "1";
                else data[28] = "0";

                data[20] = tbBeeKortBov.Text;
                data[21] = tbBeeKortOnd.Text;
                data[22] = tbBeeKortLin.Text;
                data[23] = tbBeeKortRec.Text;
                data[24] = tbBeeLanBov.Text;
                data[25] = tbBeeLanOnd.Text;
                data[26] = tbBeeLanLin.Text;
                data[27] = tbBeeLanRec.Text;
                data[29] = tbBeeTot.Text;
            }
            //textbestand opnieuw schrijven
            FileInfo file = new FileInfo("settings.txt");
            StreamWriter sw = file.CreateText();
            foreach (string regel in data)
                sw.WriteLine(regel);
            sw.Close();
            labHelp.Text = labHelp1.Text = labHelp2.Text = "Gegevens opgeslagen";
        }

        private void btVorigUitgebreid_Click(object sender, EventArgs e)
        {
            GegevensOphalen();
            labHelp.Text = labHelp1.Text = labHelp2.Text = "Gegevens opgehaald";
        }

        private void label1_MouseHover(object sender, EventArgs e)
        {
            switch (((Label)sender).Name.Substring(((Label)sender).Name.Length - 2))
            {
                case "l1": //webcam korte zijde
                    labHelp.Text = "De webcam die aan de korte zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "l2": //webcam lange zijde links
                    labHelp.Text = "De webcam die links aan de lange zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "l3": //webcam lange zijde midden
                    labHelp.Text = "De webcam die midden aan de lange zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "l4": //webcam lange zijde rechts
                    labHelp.Text = "De webcam die rechts aan de lange zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "l9": //ipaddres
                    labHelp.Text = "Het ipaddres van de pc waar het Vissen Project op draait, waarschijnlijk dezelfde pc (127.0.0.1)";
                    break;
                case "l5": //poort beweging
                    labHelp.Text = "De poort voor Bewegingdetectie,\n\rwaarschijnlijk 7780";
                    break;
                case "l8": //poort gezicht
                    labHelp.Text = "De poort voor Gezichtsherkenning,\n\rwaarschijnlijk 7781";
                    break;
                case "l6": //poort lange zijde
                    labHelp.Text = "De poort voor de videobeelden voor de lange zijde,\n\rwaarschijnlijk 7779";
                    break;
                case "14": //achtergrond - aantal pixels
                    labHelp1.Text = "Hoeveel pixels er worden overgeslagen bij het zoeken van de achtergrond, moet tussen de 1 en de 20 liggen";
                    break;
                case "10": //poort korte zijde
                    labHelp.Text = "De poort voor de videobeelden voor de kort zijde,\n\rwaarschijnlijk 7778";
                    break;
                case "12": //achtergrond - aantal frames
                    labHelp1.Text = "Het aantal frames waar een achtergrond uit gehaald word, hoe meer hoe betrouwbaarder de achtergrond";
                    break;
                case "21": //achtergrond - verschil pixels
                    labHelp1.Text = "Het maximale verschil tussen de pixels bij vergelijken,\n\rmoet tussen de 0 en de 255 liggen";
                    break;
                case "23": //beweging - tijdsinterval
                    labHelp1.Text = "De tijdsinterval waarmee er op beweging gekeken word, interval is in miliseconden";
                    break;
                case "20": //beweging - verschil pixels
                    labHelp1.Text = "Het maximale verschil tussen de pixels bij vergelijken,\n\rmoet tussen de 0 en de 255 liggen";
                    break;
                case "18": //samenvoegen - aantal kolommen
                    labHelp1.Text = "Het aantal kolommen waarmee de webcambeelden vergeleken worden, moet tussen 1 en 50 liggen";
                    break;
                case "17": //samenvoegen - hoogte van kolom
                    labHelp1.Text = "Hoeveel er van elke kolom vergeleken moet worden, moet tussen de 0 en de 100";
                    break;
                case "19": //samenvoegen - percentage hetzelfde
                    labHelp1.Text = "Hoe precies de webcambeelden hetzelfde moeten zijn,\n\rmoet tussen de 0 en de 100";
                    break;
                case "22": //samevoegen - pixelverschil
                    labHelp1.Text = "Het maximale verschil tussen de pixels bij vergelijken, moet tussen de 0 en de 255 liggen";
                    break;
                case "25": //aantal webcams
                    labHelp1.Text = "2 Of 3 webcambeelden samenvoegen en door- streamen (bij 2 word de middelste camera weggelaten)";
                    break;
                case "26": //gezicht - tijdsinterval
                    labHelp1.Text = "De tijdsinterval waarmee er op gezichten gekeken word, interval is in miliseconden";
                    break;
                case "29": //kort - onder
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de onderkant van de alleenstaande webcam.";
                    break;
                case "30": //kort - boven
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de bovenkant van de alleenstaande webcam.";
                    break;
                case "31": //kort - links
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de linkerkant van de alleenstaande webcam.";
                    break;
                case "32": //kort - rechts
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de rechterkant van de alleenstaande webcam.";
                    break;
                case "34": //lang - onder
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de onderkant van de meerdere webcams.";
                    break;
                case "35": //lang - boven
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de bovenkant van de meerdere webcams.";
                    break;
                case "36": //lang - links
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de linkerkant van de meerdere webcams.";
                    break;
                case "37": //lang - rechts
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de rechterkant van de meerdere webcams.";
                    break;
                case "39": //checkbox handmatig vergelijken
                    labHelp2.Text = "Of het vergelijken tussen de linker- en rechterstream automatisch moet of handmatig.";
                    break;
                case "40": //handmatig vergelijken
                    labHelp2.Text = "Hoeveel er van elke kant van de linker en rechterwebcam verwijderd moet worden om er 1 vloeiende beeld van te maken.";
                    break;
            }
        }

        private void tbWebKort_MouseHover(object sender, EventArgs e)
        {
            switch (((TextBox)sender).Name)
            {
                case "tbWebKort": //webcam korte zijde
                    labHelp.Text = "De webcam die aan de korte zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "tbWebLangL": //webcam lange zijde links
                    labHelp.Text = "De webcam die links aan de lange zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "tbWebLangM": //webcam lange zijde midden
                    labHelp.Text = "De webcam die midden aan de lange zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "tbWebLangR": //webcam lange zijde rechts
                    labHelp.Text = "De webcam die rechts aan de lange zijde hangt,\n\reen getal tussen de 0 en de 3";
                    break;
                case "tbIpAddres": //ipaddres
                    labHelp.Text = "Het ipaddres van de pc waar het Vissen Project op draait, waarschijnlijk dezelfde pc (127.0.0.1)";
                    break;
                case "tbPoortMotion": //poort beweging
                    labHelp.Text = "De poort voor Bewegingdetectie,\n\rwaarschijnlijk 7780";
                    break;
                case "tbPoortFace": //poort gezicht
                    labHelp.Text = "De poort voor Gezichtsherkenning,\n\rwaarschijnlijk 7781";
                    break;
                case "tbPoortLang": //poort lange zijde
                    labHelp.Text = "De poort voor de videobeelden voor de lange zijde,\n\rwaarschijnlijk 7779";
                    break;
                case "tbPoortKort": //poort korte zijde
                    labHelp.Text = "De poort voor de videobeelden voor de kort zijde,\n\rwaarschijnlijk 7778";
                    break;
                case "tbAchtAantalPixels": //achtergrond - aantal pixels
                    labHelp1.Text = "Hoeveel pixels er worden overgeslagen bij het zoeken van de achtergrond, moet tussen de 1 en de 20 liggen";
                    break;
                case "tbAchtFrames": //achtergrond - aantal frames
                    labHelp1.Text = "Het aantal frames waar een achtergrond uit gehaald word, hoe meer hoe betrouwbaarder de achtergrond";
                    break;
                case "tbAchtVersPixels": //achtergrond - verschil pixels
                    labHelp1.Text = "Het maximale verschil tussen de pixels bij vergelijken,\n\rmoet tussen de 0 en de 255 liggen";
                    break;
                case "tbBeweInterval": //beweging - tijdsinterval
                    labHelp1.Text = "De tijdsinterval waarmee er op beweging gekeken word, interval is in miliseconden";
                    break;
                case "tbBeweVersPixels": //beweging - verschil pixels
                    labHelp1.Text = "Het maximale verschil tussen de pixels bij vergelijken,\n\rmoet tussen de 0 en de 255 liggen";
                    break;
                case "tbStreamAantalKolom": //samenvoegen - aantal kolommen
                    labHelp1.Text = "Het aantal kolommen waarmee de webcambeelden vergeleken worden, moet tussen 1 en 50 liggen";
                    break;
                case "tbStreamKolomHoogte": //samenvoegen - hoogte van kolom
                    labHelp1.Text = "Hoeveel er van elke kolom vergeleken moet worden, moet tussen de 0 en de 100";
                    break;
                case "tbStreamPerc": //samenvoegen - percentage hetzelfde
                    labHelp1.Text = "Hoe precies de webcambeelden hetzelfde moeten zijn,\n\rmoet tussen de 0 en de 100";
                    break;
                case "tbStreamVersPixels": //samevoegen - pixelverschil
                    labHelp1.Text = "Het maximale verschil tussen de pixels bij vergelijken, moet tussen de 0 en de 255 liggen";
                    break;
                case "doWebcams":
                    labHelp1.Text = "Of de beelden van 2 of 3 webcambeelden doorgestreamd worden\n\r(middelste camera word bij 2 weggelaten)";
                    break;
                case "tbFaceInterval": //gezicht- tijdsinterval
                    labHelp1.Text = "De tijdsinterval waarmee er op gezichten gekeken word, interval is in miliseconden";
                    break;
                case "tbBeeKortOnd": //kort - onder
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de onderkant van de alleenstaande webcam.";
                    break;
                case "tbBeeKortBov": //kort - boven
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de bovenkant van de alleenstaande webcam.";
                    break;
                case "tbBeeKortLin": //kort - links
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de linkerkant van de alleenstaande webcam.";
                    break;
                case "tbBeeKortRec": //kort - rechts
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de rechterkant van de alleenstaande webcam.";
                    break;
                case "tbBeeLanOnd": //lang - onder
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de onderkant van de meerdere webcams.";
                    break;
                case "tbBeeLanBov": //lang - boven
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de bovenkant van de meerdere webcams.";
                    break;
                case "tbBeeLanLin": //lang - links
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de linkerkant van de meerdere webcams.";
                    break;
                case "tbBeeLanRec": //lang - rechts
                    labHelp2.Text = "Hoeveel beeld er word afgesneden aan de rechterkant van de meerdere webcams.";
                    break;
                case "tbBeeTot": //handmatig vergelijken
                    labHelp2.Text = "Hoeveel er van elke kant van de linker en rechterwebcam verwijderd moet worden om er 1 vloeiende beeld van te maken.";
                    break;
            }
        }

        private void cbBeeHand_CheckedChanged(object sender, EventArgs e)
        {
            if (cbBeeHand.Checked) //handmatig
            {
                label17.Visible = label18.Visible = label19.Visible = label22.Visible = false;
                tbStreamAantalKolom.Visible = tbStreamKolomHoogte.Visible = tbStreamPerc.Visible = tbStreamVersPixels.Visible = false;
                label17.MouseHover -= label1_MouseHover;
                label18.MouseHover -= label1_MouseHover;
                label19.MouseHover -= label1_MouseHover;
                label22.MouseHover -= label1_MouseHover;
                tbStreamVersPixels.MouseHover -= tbWebKort_MouseHover;
                tbStreamAantalKolom.MouseHover -= tbWebKort_MouseHover;
                tbStreamKolomHoogte.MouseHover -= tbWebKort_MouseHover;
                tbStreamPerc.MouseHover -= tbWebKort_MouseHover;

                label40.Visible = tbBeeTot.Visible = true;
                tbBeeTot.MouseHover += tbWebKort_MouseHover;
                label40.MouseHover += label1_MouseHover;
            }
            else //automatisch
            {
                label40.Visible = tbBeeTot.Visible = false;
                tbBeeTot.MouseHover -= tbWebKort_MouseHover;
                label40.MouseHover -= label1_MouseHover;

                label17.Visible = label18.Visible = label19.Visible = label22.Visible = true;
                tbStreamAantalKolom.Visible = tbStreamKolomHoogte.Visible = tbStreamPerc.Visible = tbStreamVersPixels.Visible = true;
                label17.MouseHover += label1_MouseHover;
                label16.MouseHover += label1_MouseHover;
                label19.MouseHover += label1_MouseHover;
                label22.MouseHover += label1_MouseHover;
                tbStreamVersPixels.MouseHover += tbWebKort_MouseHover;
                tbStreamAantalKolom.MouseHover += tbWebKort_MouseHover;
                tbStreamKolomHoogte.MouseHover += tbWebKort_MouseHover;
                tbStreamPerc.MouseHover += tbWebKort_MouseHover;
            }
        }

        private void cbBeeHand_MouseHover(object sender, EventArgs e)
        {
             labHelp2.Text = "Of het vergelijken tussen de linker- en rechterstream automatisch moet of handmatig.";
        }
    }
}
