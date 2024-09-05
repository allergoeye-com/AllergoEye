package lib.common.layout;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.TimeInterpolator;
import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;


import com.dryeye.app.R;


public class ProgressLayout  extends FrameLayout {

    boolean fStop = false;
    int count = 5;
    AnimatedView [] spots = new AnimatedView[count];
    Animator[] animators = new Animator[count];
    CAnimatorListenerAdapter animator;
    public ProgressLayout(@NonNull Context context) {
        super(context);

    }

    public ProgressLayout(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

    }

    public ProgressLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

    }

    public ProgressLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

    }
    public void Init()
    {
        int size = getContext().getResources().getDimensionPixelSize(R.dimen.spot_size);

        int progressWidth = getMeasuredWidth();
        for (int i = 0; i < spots.length; i++)
        {
            AnimatedView v = new AnimatedView(getContext());
            v.setBackgroundResource(R.drawable.dmax_spots_spot);
            v.setTarget(progressWidth);
            v.setXFactor(-1f);
            v.setVisibility(View.INVISIBLE);
            addView(v, size, size);
            spots[i] = v;
        }
        for (AnimatedView view : spots) view.setVisibility(View.VISIBLE);
        animator = new CAnimatorListenerAdapter();
    }
    public void Play()
    {
        animator.Play();
    }


    public void Stop()
    {
        animator.Stop();
    }

    class CAnimatorListenerAdapter  extends AnimatorListenerAdapter {

        boolean fStop = false;
        CAnimatorListenerAdapter()
        {
            animators = new Animator[count];
            for (int i = 0; i < spots.length; i++)
            {
                final AnimatedView animatedView = spots[i];
                Animator move = ObjectAnimator.ofFloat(animatedView, "xFactor", 0, 1);
                move.setDuration(3500);
                move.setInterpolator(new TimeInterpolator() {

                    @Override
                    public float getInterpolation(float input) {
                        return input < 0.5
                                ? (float) Math.pow(input * 2, 0.5) * 0.5f
                                : (float) Math.pow((1.0f - input) * 2.0f, 0.5) * -0.5f + 1;

                    }
                });
                move.setStartDelay(500 * i);
                move.addListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                    //    animatedView.setVisibility(View.INVISIBLE);
                    }

                    @Override
                    public void onAnimationStart(Animator animation) {
                        animatedView.setVisibility(View.VISIBLE);
                    }
                });
                animators[i] = move;
            }

        }




        @Override
        public void onAnimationEnd(Animator animation) {
            if (!fStop) Run();
        }

        void Play() {
            Run();
        }

        void Stop() {
            fStop = true;
        }

        private void Run() {
            AnimatorSet set = new AnimatorSet();
            set.playTogether(animators);
            set.addListener(this);
            set.start();
        }

    }
    class AnimatedView extends View {

        private int target;

        public AnimatedView(Context context) {
            super(context);
        }

        public float getXFactor() {
            return getX() / target;
        }

        public void setXFactor(float xFactor) {
            setX(target * xFactor);
        }

        public void setTarget(int target) {
            this.target = target;
        }

        public int getTarget() {
            return target;
        }
    }
}
