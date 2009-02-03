namespace TestEyeDetection
{
    partial class frmEyeTest
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
            this.components = new System.ComponentModel.Container();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.btnDetect = new System.Windows.Forms.Button();
            this.lblLX = new System.Windows.Forms.Label();
            this.lblRX = new System.Windows.Forms.Label();
            this.lblLY = new System.Windows.Forms.Label();
            this.lblRY = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBox1.Location = new System.Drawing.Point(12, 60);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(399, 200);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // btnBrowse
            // 
            this.btnBrowse.Location = new System.Drawing.Point(12, 4);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(75, 31);
            this.btnBrowse.TabIndex = 1;
            this.btnBrowse.Text = "Browse...";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.ForeColor = System.Drawing.SystemColors.ButtonShadow;
            this.label1.Location = new System.Drawing.Point(12, 44);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(300, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "*Browse for an image containing human face and click detect.";
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.Filter = "JPEG Files|*.jpg|Bitmap Files|*.bmp|PNG Files|*.png|GIF Files|*.gif";
            // 
            // btnDetect
            // 
            this.btnDetect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnDetect.Location = new System.Drawing.Point(335, 4);
            this.btnDetect.Name = "btnDetect";
            this.btnDetect.Size = new System.Drawing.Size(75, 31);
            this.btnDetect.TabIndex = 3;
            this.btnDetect.Text = "Detect";
            this.btnDetect.UseVisualStyleBackColor = true;

            // 
            // lblLX
            // 
            this.lblLX.AutoSize = true;
            this.lblLX.Location = new System.Drawing.Point(94, 4);
            this.lblLX.Name = "lblLX";
            this.lblLX.Size = new System.Drawing.Size(13, 13);
            this.lblLX.TabIndex = 4;
            this.lblLX.Text = "0";
            // 
            // lblRX
            // 
            this.lblRX.AutoSize = true;
            this.lblRX.Location = new System.Drawing.Point(94, 22);
            this.lblRX.Name = "lblRX";
            this.lblRX.Size = new System.Drawing.Size(13, 13);
            this.lblRX.TabIndex = 5;
            this.lblRX.Text = "0";
            // 
            // lblLY
            // 
            this.lblLY.AutoSize = true;
            this.lblLY.Location = new System.Drawing.Point(138, 4);
            this.lblLY.Name = "lblLY";
            this.lblLY.Size = new System.Drawing.Size(13, 13);
            this.lblLY.TabIndex = 6;
            this.lblLY.Text = "0";
            // 
            // lblRY
            // 
            this.lblRY.AutoSize = true;
            this.lblRY.Location = new System.Drawing.Point(137, 22);
            this.lblRY.Name = "lblRY";
            this.lblRY.Size = new System.Drawing.Size(13, 13);
            this.lblRY.TabIndex = 7;
            this.lblRY.Text = "0";
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // frmEyeTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(422, 265);
            this.Controls.Add(this.lblRY);
            this.Controls.Add(this.lblLY);
            this.Controls.Add(this.lblRX);
            this.Controls.Add(this.lblLX);
            this.Controls.Add(this.btnDetect);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnBrowse);
            this.Controls.Add(this.pictureBox1);
            this.Name = "frmEyeTest";
            this.Text = "Face and Eye Detection Test";
            this.Load += new System.EventHandler(this.frmEyeTest_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button btnDetect;
        private System.Windows.Forms.Label lblLX;
        private System.Windows.Forms.Label lblRX;
        private System.Windows.Forms.Label lblLY;
        private System.Windows.Forms.Label lblRY;
        private System.Windows.Forms.Timer timer1;
    }
}

