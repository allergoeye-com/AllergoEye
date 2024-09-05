package lib.common;

import androidx.recyclerview.widget.RecyclerView;

public interface IAdapter extends ISelectedElement {
    RecyclerView.Adapter Adapter();

    boolean IsInvalid();

    boolean Invalidate();

    void NotifyDataSetChanged();

    void Select(String word);

    void DowloadImage(int w, int h);
}
