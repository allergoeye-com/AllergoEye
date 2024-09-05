package lib.colorpicker;

import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;


import com.dryeye.app.R;

import lib.common.ActionCallback;
import lib.common.CDialog;
import lib.utils.AGlobals;

public class ModalColorDialog extends CDialog {
    View view;
    TextView colorCtrl;
    public int colorResult;
    public ModalColorDialog(int color) {
        this(color, THEME_HOLO_DARK);
    }


    public ModalColorDialog(int color, int thema) {
        super(AGlobals.currentActivity, thema);
        colorResult = color;
        view  = LayoutInflater.from(getContext()).inflate(R.layout.dialog_current_color, null);

        colorCtrl = view.findViewById(R.id.currentColor);
        ColorPicker picker = (ColorPicker)view.findViewById(R.id.selectColorPicker);

        this.setView(view);
        AddButton(BUTTON_NEGATIVE, getContext().getString(R.string.cancel), IDCANCEL);
        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);

    }
    @Override
    public void OnInitDialog()
    {
        ColorPicker picker = (ColorPicker)view.findViewById(R.id.selectColorPicker);
        picker.InitRow();
        picker.requestLayout();
        picker.oncolorChanged = new ActionCallback(this, "colorChanged", int.class);
        picker.oncolorChanging = new ActionCallback(this, "colorChanged", int.class);
        picker.SetColor(colorResult);

    }
    public void SetCurrent(int color)
    {
        _SetCurrent(color);
    }

    public void _SetCurrent(int color)
    {

        colorResult = color;
        AGlobals.SetTextColor(colorCtrl, color);

    }
    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    public void OnLongClickColor()
    {

//        ThreadColorsDlg dlg = new ThreadColorsDlg(this, CEOSColorTool.GetPalleteColors(), currName);
  //      dlg.show();

    }
    public void colorChanged(int color) {

        _SetCurrent(color);
    }
    @Override
    protected void OnCommand(int id)
    {
        ColorPicker picker = (ColorPicker)view.findViewById(R.id.selectColorPicker);
        picker.oncolorChanged = null;
        super.OnCommand(id);
    }

    public void OnSelectPaletteColor(String[] palette, int index) {
        colorChanged(Color.parseColor(palette[index + 1]));
    }
    class CLongClickListener implements View.OnLongClickListener {
        @Override
        public boolean onLongClick(View v) {
            OnLongClickColor();
            return true;
        }
    }
}
