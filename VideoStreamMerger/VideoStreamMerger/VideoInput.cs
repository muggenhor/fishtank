using System;
using System.Drawing;
using System.Threading;
using VideoSource;

namespace VideoStreamMerger
{
    class VideoInput
    {
        private IVideoSource    source= null;
        private Bitmap          newFrame = null, background;
        private Background      bgFrame = null;
        private bool            bgGround = false;

        public VideoInput(IVideoSource source)
        {
            bgFrame = new Background();
            this.source = source;
            source.NewFrame += new CameraEventHandler(source_NewFrame);
            this.source.Start();
        }

        void source_NewFrame(object sender, CameraEventArgs e)
        {
            if (!bgGround)
            {
                try
                {
                    // lock
                    Monitor.Enter(this);

                    // dispose old frame
                    if (newFrame != null)
                    {
                        newFrame.Dispose();
                    }

                    //frame laden
                    newFrame = (Bitmap)e.Bitmap.Clone();

                    //background bijstellen indien nodig
                    if (!bgGround)
                    {
                        bgFrame.EditBackground(newFrame);
                        bgGround = bgFrame.BackgroundCheck();
                        if (bgGround)
                            background = newFrame;
                    }
                    // unlock
                    Monitor.Exit(this);
                }
                catch { }
            }
        }

        //geeft aan of de background is gevonden, zo ja: dan ???
        public bool BackgroundFound { get { return bgGround; } }
        public Bitmap backGround { get { return background; } }
    }
}
