package lib.common;

import android.graphics.Rect;

import java.util.ArrayList;

/**
 * Created by alex on 25.01.2018.
 */

public interface IActionCalbackClient {
    void Detach(ArrayList<Object> stack);

    void Detach();

    void Init();

}
