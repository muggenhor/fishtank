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

        public event EventHandler frame;
        public delegate void EventHandler(Bitmap frame);
        public event EventHandlerStream stream;
        public delegate void EventHandlerStream(System.IO.MemoryStream ms);

        public VideoInput(IVideoSource source)
            : this(source, 20, 10) { }

        public VideoInput(IVideoSource source, int frames, int pixels)
        {
            bgFrame = new Background(frames, pixels);
            this.source = source;
            this.source.NewFrame += new CameraEventHandler(source_NewFrame);
            this.source.Start();
        }

        void source_NewStream(object sender, CameraEventArgs e)
        {
            if (stream != null)
                stream(e.Stream);
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
            else //background is gemaakt, nu de frame doorsturen
                if (frame != null)
                    frame((Bitmap)e.Bitmap.Clone());
        }

        public void Close()
        {
            // lock
            Monitor.Enter(this);

            if (source != null)
            {
                source.Stop();
            }
            // unlock
            Monitor.Exit(this);
        }

        public bool BackgroundFound { get { return bgGround; } }
        public Bitmap backGround { get { return background; } }
        public IVideoSource Source { get { return source; } }
    }
}
