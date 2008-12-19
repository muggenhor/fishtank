namespace MotionTracker
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
            this.components = new System.ComponentModel.Container();
            this.cameraWindow = new motion.CameraWindow();
            this.panel = new System.Windows.Forms.Panel();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.panel.SuspendLayout();
            this.SuspendLayout();
            // 
            // cameraWindow
            // 
            this.cameraWindow.Camera = null;
            this.cameraWindow.Location = new System.Drawing.Point(3, 3);
            this.cameraWindow.Name = "cameraWindow";
            this.cameraWindow.Size = new System.Drawing.Size(279, 257);
            this.cameraWindow.TabIndex = 0;
            this.cameraWindow.Text = "cameraWindow1";
            // 
            // panel
            // 
            this.panel.Controls.Add(this.cameraWindow);
            this.panel.Location = new System.Drawing.Point(-1, 0);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(285, 263);
            this.panel.TabIndex = 1;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 1000;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 264);
            this.Controls.Add(this.panel);
            this.Name = "Form1";
            this.Text = "Motion Detector - TEST";
            this.panel.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private motion.CameraWindow cameraWindow;
        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.Timer timer;
    }
}

