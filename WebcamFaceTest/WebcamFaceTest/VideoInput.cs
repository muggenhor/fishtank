﻿using System;
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
        private int             frames = 0;
        private int             aFrames = 0;

        public event EventHandler frame;
        public delegate void EventHandler(Bitmap frame);

        //word gebruikt als er geen achtergrond vastgesteld hoeft te worden
        public VideoInput(IVideoSource source)
            : this(source, 1,10,255) { }

        public VideoInput(IVideoSource source, int frames, int pixels, int verschil)
        {
            aFrames = frames;
            bgFrame = new Background(frames, pixels, verschil);
            this.source = source;
            this.source.NewFrame += new CameraEventHandler(source_NewFrame);
            this.source.Start();
        }

        void source_NewFrame(object sender, CameraEventArgs e)
        {
            if (!bgGround)
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
                    if (frames++ < aFrames)
                        bgFrame.EditBackground(newFrame, false);
                    else
                    {
                        bgFrame.EditBackground(newFrame, true);
                        bgGround = bgFrame.BackgroundCheck();
                        if (bgGround)
                            background = newFrame;
                    }
                }
                // unlock
                Monitor.Exit(this);

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
                source.Stop();

            // unlock
            Monitor.Exit(this);
        }

        public bool BackgroundFound { get { return bgGround; } }
        public Bitmap backGround { get { return background; } }
        public IVideoSource Source { get { return source; } }
    }
}
