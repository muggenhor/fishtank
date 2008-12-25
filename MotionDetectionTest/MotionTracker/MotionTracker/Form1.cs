using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Windows.Forms;
using VideoSource;
using Tiger.Video.VFW;
using motion;

namespace MotionTracker
{
    public partial class Form1 : Form
    {
        // statistics
  //      private const int statLength = 15;
  //      private int statIndex = 0, statReady = 0;
  //      private int[] statCount = new int[statLength];

        private motion.MotionTracker detector = new motion.MotionTracker();
  //      private int detectorType = 4;
        private int intervalsToSave = 0;

        private AVIWriter writer = null;
        private bool saveOnMotion = false;
   
        public Form1()
        {
            InitializeComponent();

            //motion detection
            Camera camera = cameraWindow.Camera;

            // set motion detector to camera
            if (camera != null)
            {
                camera.Lock();
                camera.MotionDetector = detector;

                // reset statistics
 //               statIndex = statReady = 0;
                camera.Unlock();
            }

            CaptureDeviceForm form = new CaptureDeviceForm();

            if (form.ShowDialog(this) == DialogResult.OK)
            {
                // create video source
                CaptureDevice localSource = new CaptureDevice();
                localSource.VideoSource = form.Device;

                // open it
                OpenVideoSource(localSource);
            }
            else
            {
                //open file
                // create video source
                VideoFileSource fileSource = new VideoFileSource();
                fileSource.VideoSource = "C:\\Sin City.avi";
                // open it
                OpenVideoSource(fileSource);
            }
        }



        // Open video source
        private void OpenVideoSource(IVideoSource source)
        {
            // set busy cursor
            this.Cursor = Cursors.WaitCursor;

            // close previous file
            CloseFile();

            // create camera
            Camera camera = new Camera(source, detector);
            // start camera
            camera.Start();

            // attach camera to camera window
            cameraWindow.Camera = camera;

            // reset statistics
  //          statIndex = statReady = 0;

            // set event handlers
     //       camera.NewFrame += new EventHandler(camera_NewFrame);
            camera.Alarm += new EventHandler(camera_Alarm);

            // start timer
            timer.Start();

            this.Cursor = Cursors.Default;
        }

        // Close current file
        private void CloseFile()
        {
            Camera camera = cameraWindow.Camera;

            if (camera != null)
            {
                // detach camera from camera window
                cameraWindow.Camera = null;

                // signal camera to stop
                camera.SignalToStop();
                // wait for the camera
                camera.WaitForStop();

                camera = null;

                if (detector != null)
                    detector.Reset();
            }

            if (writer != null)
            {
                writer.Dispose();
                writer = null;
            }
            intervalsToSave = 0;
        }

        // On alarm
        private void camera_Alarm(object sender, System.EventArgs e)
        {
            // save movie for 5 seconds after motion stops
            intervalsToSave = (int)(5 * (1000 / timer.Interval));
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            label2.Text = detector.MethodCheckX.ToString();
            label3.Text = detector.MethodCheckY.ToString();
            label4.Text = detector.MethodCheckoX.ToString();
            label5.Text = detector.MethodCheckoY.ToString();
        }

        // On new frame
   /*     private void camera_NewFrame(object sender, System.EventArgs e)
        {
            if ((intervalsToSave != 0) && (saveOnMotion == true))
            {
                // lets save the frame
                if (writer == null)
                {
                    // create file name
                    DateTime date = DateTime.Now;
                    String fileName = String.Format("{0}-{1}-{2} {3}-{4}-{5}.avi",
                        date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second);

                    try
                    {
                        // create AVI writer
                        writer = new AVIWriter("wmv3");
                        // open AVI file
                        writer.Open(fileName, cameraWindow.Camera.Width, cameraWindow.Camera.Height);
                    }
                    catch (ApplicationException)
                    {
                        if (writer != null)
                        {
                            writer.Dispose();
                            writer = null;
                        }
                    }
                }

                // save the frame
                Camera camera = cameraWindow.Camera;

                camera.Lock();
                writer.AddFrame(camera.LastFrame);
                camera.Unlock();
            }
        }*/

  /*      private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            writer.Close();
        }*/

        // On timer event - gather statistic
  /*      private void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            textBox1.Text = "Gem. COordinaat: "
            Camera camera = cameraWindow.Camera;

            if (camera != null)
            {
                // get number of frames for the last second
                statCount[statIndex] = camera.FramesReceived;

                // increment indexes
                if (++statIndex >= statLength)
                    statIndex = 0;
                if (statReady < statLength)
                    statReady++;

                float fps = 0;

                // calculate average value
                for (int i = 0; i < statReady; i++)
                {
                    fps += statCount[i];
                }
                fps /= statReady;

                statCount[statIndex] = 0;

            //    fpsPanel.Text = fps.ToString("F2") + " fps";
            }

            // descrease save counter
            if (intervalsToSave > 0)
            {
                if ((--intervalsToSave == 0) && (writer != null))
                {
                    writer.Dispose();
                    writer = null;
                }
            }
        }*/
    }
}
