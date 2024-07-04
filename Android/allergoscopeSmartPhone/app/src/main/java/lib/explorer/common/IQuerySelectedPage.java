package lib.explorer.common;

import java.util.TimerTask;

public interface IQuerySelectedPage {
    class WaitState extends TimerTask {
        public int state;
        public boolean deleted = false;

        public WaitState(int _state)
        {
            state = _state;
        }
        @Override
        public void run() {

        }
    };


    void Query(String query);
    void Show() throws NoSuchMethodException, Exception;
    void OnShow();
    void OnStart();
    void OnDestroy();

    void Start();
}
