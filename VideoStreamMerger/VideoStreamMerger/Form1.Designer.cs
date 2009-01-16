namespace VideoStreamMerger
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
            this.pbImageLinks = new System.Windows.Forms.PictureBox();
            this.pbImageRechts = new System.Windows.Forms.PictureBox();
            this.lbVerschillen = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.listBox1 = new System.Windows.Forms.ListBox();
            ((System.ComponentModel.ISupportInitialize)(this.pbImageLinks)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbImageRechts)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // pbImageLinks
            // 
            this.pbImageLinks.Location = new System.Drawing.Point(300, 283);
            this.pbImageLinks.Name = "pbImageLinks";
            this.pbImageLinks.Size = new System.Drawing.Size(100, 50);
            this.pbImageLinks.TabIndex = 0;
            this.pbImageLinks.TabStop = false;
            // 
            // pbImageRechts
            // 
            this.pbImageRechts.Location = new System.Drawing.Point(172, 283);
            this.pbImageRechts.Name = "pbImageRechts";
            this.pbImageRechts.Size = new System.Drawing.Size(100, 50);
            this.pbImageRechts.TabIndex = 1;
            this.pbImageRechts.TabStop = false;
            // 
            // lbVerschillen
            // 
            this.lbVerschillen.FormattingEnabled = true;
            this.lbVerschillen.Items.AddRange(new object[] {
            "Percentage:"});
            this.lbVerschillen.Location = new System.Drawing.Point(584, 1);
            this.lbVerschillen.Name = "lbVerschillen";
            this.lbVerschillen.Size = new System.Drawing.Size(91, 407);
            this.lbVerschillen.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 293);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "label2";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 320);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "label2";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(3, 1);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(100, 50);
            this.pictureBox1.TabIndex = 7;
            this.pictureBox1.TabStop = false;
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Items.AddRange(new object[] {
            "Verschillen:"});
            this.listBox1.Location = new System.Drawing.Point(487, 1);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(91, 407);
            this.listBox1.TabIndex = 8;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(678, 345);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lbVerschillen);
            this.Controls.Add(this.pbImageRechts);
            this.Controls.Add(this.pbImageLinks);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pbImageLinks)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbImageRechts)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pbImageLinks;
        private System.Windows.Forms.PictureBox pbImageRechts;
        private System.Windows.Forms.ListBox lbVerschillen;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ListBox listBox1;
    }
}

