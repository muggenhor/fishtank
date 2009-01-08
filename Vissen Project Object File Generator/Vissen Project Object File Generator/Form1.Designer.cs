namespace Vissen_Project_Object_File_Generator
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panel1 = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this.tbPath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.rbObject = new System.Windows.Forms.RadioButton();
            this.rbVis = new System.Windows.Forms.RadioButton();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.nudMaxAfwijk = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.nudModelSchaal = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.gbVis = new System.Windows.Forms.GroupBox();
            this.nudDraaiVersnelling = new System.Windows.Forms.NumericUpDown();
            this.label12 = new System.Windows.Forms.Label();
            this.nudDraaiSnelheid = new System.Windows.Forms.NumericUpDown();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.nudBewegingFactor = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.nudBewegingFrequentie = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.nudMinSnelheid = new System.Windows.Forms.NumericUpDown();
            this.label9 = new System.Windows.Forms.Label();
            this.nudMaxSnelheid = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxAfwijk)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudModelSchaal)).BeginInit();
            this.gbVis.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudDraaiVersnelling)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudDraaiSnelheid)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBewegingFactor)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBewegingFrequentie)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMinSnelheid)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxSnelheid)).BeginInit();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.button1);
            this.panel1.Controls.Add(this.tbPath);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(301, 61);
            this.panel1.TabIndex = 0;
            this.panel1.Visible = false;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(12, 32);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 2;
            this.button1.Text = "Zoeken";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // tbPath
            // 
            this.tbPath.Location = new System.Drawing.Point(73, 6);
            this.tbPath.Name = "tbPath";
            this.tbPath.Size = new System.Drawing.Size(207, 20);
            this.tbPath.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Model File";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.rbObject);
            this.groupBox1.Controls.Add(this.rbVis);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 61);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(301, 64);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Ik maak een...";
            // 
            // rbObject
            // 
            this.rbObject.AutoSize = true;
            this.rbObject.Location = new System.Drawing.Point(6, 42);
            this.rbObject.Name = "rbObject";
            this.rbObject.Size = new System.Drawing.Size(56, 17);
            this.rbObject.TabIndex = 1;
            this.rbObject.Text = "Object";
            this.rbObject.UseVisualStyleBackColor = true;
            this.rbObject.CheckedChanged += new System.EventHandler(this.rbObject_CheckedChanged);
            // 
            // rbVis
            // 
            this.rbVis.AutoSize = true;
            this.rbVis.Checked = true;
            this.rbVis.Location = new System.Drawing.Point(6, 19);
            this.rbVis.Name = "rbVis";
            this.rbVis.Size = new System.Drawing.Size(39, 17);
            this.rbVis.TabIndex = 0;
            this.rbVis.TabStop = true;
            this.rbVis.Text = "Vis";
            this.rbVis.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.nudMaxAfwijk);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.nudModelSchaal);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(0, 125);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(301, 71);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Model";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(193, 47);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(15, 13);
            this.label5.TabIndex = 6;
            this.label5.Text = "%";
            // 
            // nudMaxAfwijk
            // 
            this.nudMaxAfwijk.Location = new System.Drawing.Point(122, 45);
            this.nudMaxAfwijk.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nudMaxAfwijk.Name = "nudMaxAfwijk";
            this.nudMaxAfwijk.Size = new System.Drawing.Size(65, 20);
            this.nudMaxAfwijk.TabIndex = 5;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 47);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(72, 13);
            this.label4.TabIndex = 4;
            this.label4.Text = "Max Afwijking";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(193, 21);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(15, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "%";
            // 
            // nudModelSchaal
            // 
            this.nudModelSchaal.Location = new System.Drawing.Point(122, 19);
            this.nudModelSchaal.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nudModelSchaal.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudModelSchaal.Name = "nudModelSchaal";
            this.nudModelSchaal.Size = new System.Drawing.Size(65, 20);
            this.nudModelSchaal.TabIndex = 2;
            this.nudModelSchaal.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 21);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Model Schaal";
            // 
            // gbVis
            // 
            this.gbVis.Controls.Add(this.nudDraaiVersnelling);
            this.gbVis.Controls.Add(this.label12);
            this.gbVis.Controls.Add(this.nudDraaiSnelheid);
            this.gbVis.Controls.Add(this.label11);
            this.gbVis.Controls.Add(this.label10);
            this.gbVis.Controls.Add(this.nudBewegingFactor);
            this.gbVis.Controls.Add(this.label8);
            this.gbVis.Controls.Add(this.nudBewegingFrequentie);
            this.gbVis.Controls.Add(this.label6);
            this.gbVis.Controls.Add(this.nudMinSnelheid);
            this.gbVis.Controls.Add(this.label9);
            this.gbVis.Controls.Add(this.nudMaxSnelheid);
            this.gbVis.Controls.Add(this.label7);
            this.gbVis.Dock = System.Windows.Forms.DockStyle.Top;
            this.gbVis.Location = new System.Drawing.Point(0, 196);
            this.gbVis.Name = "gbVis";
            this.gbVis.Size = new System.Drawing.Size(301, 178);
            this.gbVis.TabIndex = 3;
            this.gbVis.TabStop = false;
            this.gbVis.Text = "Vis";
            // 
            // nudDraaiVersnelling
            // 
            this.nudDraaiVersnelling.Location = new System.Drawing.Point(122, 149);
            this.nudDraaiVersnelling.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nudDraaiVersnelling.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudDraaiVersnelling.Name = "nudDraaiVersnelling";
            this.nudDraaiVersnelling.Size = new System.Drawing.Size(65, 20);
            this.nudDraaiVersnelling.TabIndex = 23;
            this.nudDraaiVersnelling.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(12, 151);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(85, 13);
            this.label12.TabIndex = 22;
            this.label12.Text = "Draai versnelling";
            // 
            // nudDraaiSnelheid
            // 
            this.nudDraaiSnelheid.Location = new System.Drawing.Point(122, 123);
            this.nudDraaiSnelheid.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudDraaiSnelheid.Name = "nudDraaiSnelheid";
            this.nudDraaiSnelheid.Size = new System.Drawing.Size(65, 20);
            this.nudDraaiSnelheid.TabIndex = 21;
            this.nudDraaiSnelheid.Value = new decimal(new int[] {
            20,
            0,
            0,
            0});
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(12, 125);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(74, 13);
            this.label11.TabIndex = 20;
            this.label11.Text = "Draai snelheid";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(193, 99);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(104, 13);
            this.label10.TabIndex = 19;
            this.label10.Text = "(0 = geen beweging)";
            // 
            // nudBewegingFactor
            // 
            this.nudBewegingFactor.Location = new System.Drawing.Point(122, 97);
            this.nudBewegingFactor.Name = "nudBewegingFactor";
            this.nudBewegingFactor.Size = new System.Drawing.Size(65, 20);
            this.nudBewegingFactor.TabIndex = 18;
            this.nudBewegingFactor.Value = new decimal(new int[] {
            20,
            0,
            0,
            0});
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 99);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(84, 13);
            this.label8.TabIndex = 17;
            this.label8.Text = "Beweging factor";
            // 
            // nudBewegingFrequentie
            // 
            this.nudBewegingFrequentie.Location = new System.Drawing.Point(122, 71);
            this.nudBewegingFrequentie.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudBewegingFrequentie.Name = "nudBewegingFrequentie";
            this.nudBewegingFrequentie.Size = new System.Drawing.Size(65, 20);
            this.nudBewegingFrequentie.TabIndex = 16;
            this.nudBewegingFrequentie.Value = new decimal(new int[] {
            15,
            0,
            0,
            0});
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 73);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(104, 13);
            this.label6.TabIndex = 15;
            this.label6.Text = "Beweging frequentie";
            // 
            // nudMinSnelheid
            // 
            this.nudMinSnelheid.Location = new System.Drawing.Point(122, 19);
            this.nudMinSnelheid.Name = "nudMinSnelheid";
            this.nudMinSnelheid.Size = new System.Drawing.Size(65, 20);
            this.nudMinSnelheid.TabIndex = 14;
            this.nudMinSnelheid.Value = new decimal(new int[] {
            15,
            0,
            0,
            0});
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(12, 21);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(68, 13);
            this.label9.TabIndex = 13;
            this.label9.Text = "Min Snelheid";
            // 
            // nudMaxSnelheid
            // 
            this.nudMaxSnelheid.Location = new System.Drawing.Point(122, 45);
            this.nudMaxSnelheid.Name = "nudMaxSnelheid";
            this.nudMaxSnelheid.Size = new System.Drawing.Size(65, 20);
            this.nudMaxSnelheid.TabIndex = 11;
            this.nudMaxSnelheid.Value = new decimal(new int[] {
            20,
            0,
            0,
            0});
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 47);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 13);
            this.label7.TabIndex = 10;
            this.label7.Text = "Max Snelheid";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(214, 322);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "Annuleren";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(133, 322);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 5;
            this.button3.Text = "Opslaan";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(52, 322);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(75, 23);
            this.button4.TabIndex = 6;
            this.button4.Text = "Open";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(301, 353);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.gbVis);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "Form1";
            this.Text = "Form1";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxAfwijk)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudModelSchaal)).EndInit();
            this.gbVis.ResumeLayout(false);
            this.gbVis.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudDraaiVersnelling)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudDraaiSnelheid)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBewegingFactor)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBewegingFrequentie)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMinSnelheid)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxSnelheid)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox tbPath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton rbObject;
        private System.Windows.Forms.RadioButton rbVis;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown nudModelSchaal;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown nudMaxAfwijk;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox gbVis;
        private System.Windows.Forms.NumericUpDown nudMinSnelheid;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown nudMaxSnelheid;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown nudBewegingFrequentie;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown nudBewegingFactor;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown nudDraaiVersnelling;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.NumericUpDown nudDraaiSnelheid;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
    }
}

