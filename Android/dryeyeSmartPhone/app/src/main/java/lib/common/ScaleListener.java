package lib.common;

import android.graphics.PointF;
import android.view.ScaleGestureDetector;

public class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        private float mScaleFactor = 1;
        SCALE scale;
        public ScaleListener(SCALE _scale)
        {
            scale = _scale;
        }
        //------------------------------
        //
        //------------------------------
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            float tmp = detector.getScaleFactor();
            mScaleFactor *= tmp;
            mScaleFactor = Math.max(0.01f, Math.min(mScaleFactor, 5.0f));
            if (Math.abs(Math.abs(mScaleFactor) - 1.0f) >= 0.02f)
            {
                scale.focus.x = detector.getFocusX();
                scale.focus.y = detector.getFocusY();
                scale.SetScale(mScaleFactor);
                mScaleFactor = 1.0f;

            }
            return true;
        }
        //------------------------------
        //
        //------------------------------
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {

            scale.start();
            return true;
        }

        //------------------------------
        //
        //------------------------------
        @Override
        public void onScaleEnd(ScaleGestureDetector detector)
        {
            scale.end();
        }

    public static class SCALE {
        public void start ()
        {
            fStart = true; scaleFactor = 1.0f;
        }
        public void scaled(float fscale) { scaleFactor = fscale;}
        public boolean IsScale() { return fStart && Math.abs(Math.abs(scaleFactor) - 1.0f) >= 0.02f; } //scaleFactor != 1.0f; }
        public float GetScale() { float f = scaleFactor; scaleFactor = 1.0f; return f; }
        public void SetScale(float f) { if (fCancelNextScale) {fCancelNextScale = false; f = 1.0f; } scaleFactor = f; }
        public void CancelNextScale() { fCancelNextScale = true; scaleFactor = 1.0f; }
        public void end() { fStart = false; }
        private float scaleFactor = 1.0f;
        boolean fCancelNextScale = false;
        public PointF focus = new PointF(0, 0);
        private boolean fStart = false;

    }
}
