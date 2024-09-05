package com.dryeye.app.card;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.dryeye.app.App;
import com.dryeye.app.MenuActivity;
import com.dryeye.app.R;

import lib.camera.CameraCalculatePreview;
import lib.utils.TreeDir;

import com.dryeye.app.timer.TimerButton;
import com.dryeye.app.wifi.FragmentSyncFile;
import com.google.android.material.floatingactionbutton.FloatingActionButton;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.common.CEdit;
import lib.common.CHandler;
import lib.common.IResultFragmentDlg;
import lib.common.ISelectedElement;
import lib.common.MessageBox;
import lib.common.OnClickCalback;
import lib.common.button.CMenuButton;
import lib.common.layout.CPanelDialog;

import lib.explorer.AdapterExplorerView;
import lib.explorer.DirLocal;
import lib.explorer.TaskReadDir;
import lib.explorer.common.DirEntry;
import lib.explorer.common.IQuerySelectedPage;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import static com.dryeye.app.App.defaultPath;
import static com.dryeye.app.card.ListBoxDlg.IDCREATE;
import static lib.common.CDialog.IDCANCEL;
import static lib.common.CDialog.IDOK;
import static lib.common.CDialog.MB_OKCANCEL;

@SuppressLint("ValidFragment")
public class FragmentExploreCards extends Fragment  implements EditText.OnEditorActionListener, IQuerySelectedPage, ISelectedElement, IResultFragmentDlg {
    View self;
    Runnable runnable = null;
    protected TaskReadDir _task = null;
    protected AGlobals.CasheFromExplore cache = null;
    protected RecyclerView recyclerView = null;
    protected GridLayoutManager gridLayoutManager  = null;
    protected ProgressBar bar = null;
    protected AdapterExplorerView mAdapter;
    CTreeDir listView;
    ProgressBar waitCursor;
    File currentDir = null;
    String cardPath = null;
    ActionCallback onexit;
    ActionCallback onnewtest;
    TreeDir dir;
    CMenuButton rename;
    CMenuButton rsync;
    static public String LastPath = null;
    public String pathJPG = null;

    public FragmentExploreCards(ActionCallback _onexit, ActionCallback _onnewtest)
    {
        super();
        onexit = _onexit;
        onnewtest = _onnewtest;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        CPanelDialog test = null;
        try {
            test = ((CPanelDialog)container);
            test.SetOrient("Right");
            test.DisableRequestLayout(true);
            test.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));
            test.fAutoClosed = false;
            self = inflater.inflate(R.layout.fragment_list_card4, container, false);

        }
        catch(Exception p)
        {
            p.printStackTrace();
        }

        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        self.setLayoutParams(new CoordinatorLayout.LayoutParams(metrics.widthPixels, metrics.heightPixels));
        self.measure(0, 0);
        TextView txt = self.findViewById(R.id.textSearch);
      /*
        String str = AGlobals.ReadString("IDS_SEARCH");
        if (str != null)
        {

            txt.setText(str + ":");
            CEdit edit = self.findViewById(R.id.searchFieldEdit);
            edit.setHint(str);
        }

       */
        int x = self.getMeasuredWidth();
        int y = self.getMeasuredHeight();


        recyclerView = self.findViewById(R.id.recyclerViewImageFiles);
        recyclerView.setHasFixedSize(true);
        gridLayoutManager = new GridLayoutManager(getActivity(), 1);
        recyclerView.setLayoutManager(gridLayoutManager);
        listView =  self.findViewById(R.id.idDirList);
        cache = AGlobals.CreateCache("thromb_cards");
        CMenuButton btn = self.findViewById(R.id.idExit);
        btn.SetCallback(this, "OnCancel");
        //  btn = self.findViewById(R.id.idTest);
        //     btn.SetCallback(this, "OnNewTest");

        btn =  self.findViewById(R.id.idTimer);
        btn.Enable(true);
        btn.SetCallback(App.GetInstance(), "OnTimeButton");

        CMenuButton cbtn = self.findViewById(R.id.searchFieldSearchButton);
        CEdit cedit = self.findViewById(R.id.searchFieldEdit);

        cedit.setOnEditorActionListener(this);
        cbtn.SetCallback(this, "OnQuery");
        btn = self.findViewById(R.id.idDelete);
        btn.SetCallback(this, "OnDelete");
        listView.setClickable(true);
        dir = new TreeDir(defaultPath, (new String [] {".ini", ".jpg"}));

      btn.Enable(dir.NumDir(false) > 0);
        OnSelect(false);
        recyclerView.setAdapter(mAdapter);
        currentDir = null;
        if (dir.size() > 0)
        {

            String ph = defaultPath + dir.get(0).GetPath();
            TreeDir.DirNode _dir  = (TreeDir.DirNode)dir.get(0);

            if (dir != null && _dir.childs != null)
                ph = defaultPath + _dir.get(0).GetPath();
            currentDir = new File(ph);
            cardPath = defaultPath + dir.get(0).GetPath();
        }
        listView.Init(dir, new ActionCallback(this, "OnSelectDir", TreeDir.DirNodeVisible.class));
        int level = 0;
    /*    txt = self.findViewById(R.id.idExitText);
        str = AGlobals.ReadString("IDS_EXIT");
        if (str != null)
            txt.setText(str);
        txt = self.findViewById(R.id.idDeleteText);
        str = AGlobals.ReadString("IDS_DELETE");
        if (str != null)
            txt.setText(str);
        txt = self.findViewById(R.id.idRenameText);
        str = AGlobals.ReadString("IDS_RENAME");
        if (str != null)
            txt.setText(str);
*/
        rename = self.findViewById(R.id.idRename);
        rename.SetCallback(this, "OnRename");
        rsync = self.findViewById(R.id.idSync);
        rsync.SetCallback(this, "OnSyncDir");
        if (this.LastPath != null)
            listView.SetSelect(LastPath);

        // listDir.Init(dir, 0, new ActionCallback(this, "OnSelectDir", int.class));
        container.setTranslationY(0);
        container.setTranslationX(0);
       test.Start();

        return self;
    }
    @Override
    public void Start()
    {
        if (waitCursor != null || currentDir == null)
            waitCursor.setVisibility(View.VISIBLE);
        bar = self.findViewById(R.id.progress_bar);
        cache.cache.ClearMemory();
        cache.result.clear();
        DirLocal th = new DirLocal(this, currentDir, AGlobals.MaskPicture(), bar, cache.result, false);
        th.execute();

    }

    /*
    public void OnNewTest()
    {
        (new Handler()).post(new Runnable() {
            @Override
            public void run()
            {
                _OnNewTest();
            }
        });
    }
    public void _OnNewTest()
    {

    }
    */
    @Override
    public void Show() throws NoSuchMethodException {


        if (waitCursor != null)
            waitCursor.setVisibility(View.INVISIBLE);
        CreateAdapter();
        OnSelect(true);
    }

    @Override
    public void OnShow()
    {

    }

    @Override
    public void OnStart()
    {

    }

    @Override
    public void OnDestroy()
    {

    }

    public void OnQuery()
    {
        AGlobals.ShowKeyboard(false);

        CEdit editText = (CEdit) self.findViewById(R.id.searchFieldEdit);
        String query = editText.getText().toString();
        int i = onQuery(query);
        //listDir.SetSel(i);
        if (i >= 0)
            listView.SetSelect("/" + dir.get(i).name);
        OnSelect(i >= 0);

    }
    private int onQuery(String query)
    {


            int numIndex = dir.size();
            int n = 0;
            for (int i = 0; i < numIndex; ++i)
            {
                String line =dir.get(i).name;
                if (line.toLowerCase().contains(query.toLowerCase()))
                    return i;
            }


            return -1;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public void Query(String query)
    {
        int i = onQuery(query);

        OnSelect(i >= 0);
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    protected void CreateAdapter()
    {
        OnSelect(null);
        if (_task != null) {
            try {
                _task.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        _task = new TaskReadDir();
        mAdapter = new AdapterExplorerView( gridLayoutManager,  cache,_task, new ActionCallback(this, "OnSelect", DirEntry.class), R.layout.grid_view_items_1);
        recyclerView.setAdapter(mAdapter);


    }
    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void OnSelect(DirEntry b)
    {
        if (b != null)
            OnSelect(true);
    }
    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void OnSelect(boolean b)
    {
        try {
            FloatingActionButton fab = (FloatingActionButton)self.findViewById(R.id.actionProperty);
            if (fab != null)
            {

                if (b)
                {
                    AGlobals.showFloatingActionButton(fab);
                    DirEntry sel = mAdapter.GetSelectedElement();
                    TreeDir.DirNodeVisible dir = listView.GetSelect();
                    if (dir.level == 2)
                    {
                        if (sel != null ) {
                            fab.setImageDrawable(AGlobals.GetDrawable("drawable/accept"));
                        }
                        else {
                            fab.setImageDrawable(AGlobals.GetDrawable("drawable/camera_go_to"));
                        }
                    }
                    else
                    {
                        fab.setImageDrawable(AGlobals.GetDrawable("drawable/add"));

                    }




                    //              fab.setImageDrawable(AGlobals.GetDrawable("drawable/search"));
                    fab.setOnClickListener(new OnClickCalback(this, "OnClickDownload"));
                }
                else
                {
                    fab.setOnClickListener(null);
                    AGlobals.hideFloatingActionButton(fab);
                }
            }
        }
        catch(Exception e)
        {
            ;
        }
    }

    boolean OnNextStep() throws CException
    {
        int new_level = 0, level = 0;
        level = App.cardPath.Level();
        ListBoxDlg dlg = new ListBoxDlg(getActivity());
        int res;
        if (CameraCalculatePreview.fPirke || dlg.list.size() == 0)
            return App.cardPath.CreateNewTest();
        if (dlg.fInserted && dlg.list.size() == 1)
            res = IDOK;
        else
            res = dlg.DoModal();
        if (res == IDCANCEL)
            return false;
        if (res == IDOK)
            App.cardPath.SelectRel(dlg.list.get(dlg.index).GetPath());
        if (res == IDCREATE)
            return App.cardPath.CreateNewTest();
        new_level = App.cardPath.Level();
        if (new_level == 2)
            return true;
        return OnNextStep();

    }
    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void OnClickDownload(View v)
    {
        try
        {

            DirEntry b = mAdapter.GetSelectedElement();
            TreeDir.DirNodeVisible dir = listView.GetSelect();
            App.cardPath.Init(dir.dir.GetPath());
            pathJPG = b != null ? b.img.url : null;
            if (pathJPG == null && App.cardPath.Level() != CurrentPath.LEVEL_TEST)
                if (!OnNextStep())
                    return;
            LastPath = App.cardPath.RelDir();
            final Activity ac = getActivity();

            runnable = new ActionCallback() {
                @Override
                public void run() {
                    onexit.run(ac, pathJPG);
                }
            };
        /*    if (pathJPG == null && ac instanceof MenuActivity)
            {
                ((MenuActivity)ac).CloseDialog();
                return;
            }

         */
        } catch (CException e)
        {
            e.printStackTrace();
        }
        ((OrientActivity)getActivity()).DialogHide();
    }

    public void OnRenameCard(String[] result_info, TreeDir.DirNodeVisible node)
    {
        String path = App.defaultPath;
        File oldpath = new File(App.defaultPath + node.dir.GetPath());
        //String dirname = (result_info[0] + " " + result_info[1]).trim();
        String dirname = (result_info[1] + "_" + result_info[0]).trim() + "_" + result_info[3];

        String pathinfo = null;
        path += "/" + dirname;
        File newpath = new File(path);
        if (newpath.exists())
            return;
        oldpath.renameTo(newpath);
        LastPath = "/" + dirname;
        node.dir.name = dirname;
        pathinfo = path + "/info.ini";
        AGlobals.SaveIniW("CARD", "NAME", result_info[0], pathinfo);
        AGlobals.SaveIniW("CARD", "FAMILY", result_info[1], pathinfo);
        AGlobals.SaveIniW("CARD", "DATE", result_info[2], pathinfo);
        AGlobals.SaveIniW("CARD", "ID", result_info[3], pathinfo);
        listView.Init(dir, new ActionCallback(this, "OnSelectDir", TreeDir.DirNodeVisible.class));
        listView.SetSelect(LastPath);
        (new CHandler()).Send(new Runnable() {
            @Override
            public void run()
            {
                CreateAdapter();

            }
        });


    }
    public void OnSyncDir()
    {
        ArrayList< String > list = new ArrayList< String > ();
        ArrayList< String > _roi = new ArrayList< String > ();
        TreeDir.DirNodeVisible node = listView.GetSelect();
        String [] mask = new String [1];
        mask[0] = ".jpg";
        ArrayList<TreeDir.FileNode> names = new ArrayList<TreeDir.FileNode> () ;
        node.dir.GetFileList(names, mask);
        for (int i = 0; i < names.size(); ++i)
        {

            String jpg = names.get(i).GetPath();
            list.add(jpg);

            _roi.add(jpg.replace(".jpg", ".roi"));
        }
        ((MenuActivity) getActivity()).OnSyncDir(list, _roi);
    }
    public void OnRename()
    {
        try
        {
            TreeDir.DirNodeVisible node = listView.GetSelect();

            if (node.level == 0)
            {
                ((MenuActivity) getActivity()).OnEditCardFragment(node, new ActionCallback(this, "OnRenameCard", String[].class, TreeDir.DirNodeVisible.class));
            } else if (node.level == 2)
            {

                CurrentPath cardPath = new CurrentPath();
                LastPath = cardPath.RenameTest(node);
                listView.Init(dir, new ActionCallback(this, "OnSelectDir", TreeDir.DirNodeVisible.class));
                listView.SetSelect(LastPath);
                (new CHandler()).Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        CreateAdapter();

                    }
                });
            }

        } catch (CException e)
        {
            e.printStackTrace();
        }
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {


    }


    public void OnCancel()
    {

        ((OrientActivity)getActivity()).DialogHide();
    }
    public void OnDelete()
    {
        DirEntry b = mAdapter.GetSelectedElement();
        if (b != null)
        {
            MessageBox box = new MessageBox(getContext().getString(R.string.ids_delete_image), null, MB_OKCANCEL);
            if (box.DoModal() == IDCANCEL) return;

            String pathImage = b.img.url;
            File f = new File(pathImage);
            if (f.exists())
                f.delete();
            f = new File(pathImage.replace(".jpg", ".roi"));
            if (f.exists())
                f.delete();
            f = new File(pathImage.replace(".jpg", ".res"));
            if (f.exists())
                f.delete();

            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {
                    Start();
                }
            });
            return;
        }
        TreeDir.DirNodeVisible dr = listView.GetSelect();
        if (dr != null)
        {

            MessageBox box = new MessageBox(getContext().getString(R.string.del_sel), dr.dir.GetPath(), MB_OKCANCEL);
            if (box.DoModal() == IDOK)
            {

                {
                    try
                    {
                        currentDir = new File(defaultPath + dr.dir.GetPath());
                        FileUtils.DeleteDir(currentDir);
                        dir.DeleteNode(dr.dir);
                        listView.Init(dir, new ActionCallback(this, "OnSelectDir", TreeDir.DirNodeVisible.class));
                        final TreeDir.DirNodeVisible dr1 = listView.GetSelect();
                        if (dr1 != null)
                        {
                            (new CHandler()).Send(new Runnable() {
                                @Override
                                public void run()
                                {
                                    if (dr1 != null)
                                        OnSelectDir(dr1);
                                    else
                                        CreateAdapter();

                                }
                            });
                        }

                    } catch (Exception e)
                    {
                        e.printStackTrace();
                    }
                }

            }
        }
    }

    public void OnInitDialog()
    {
        int level = -1;
        if (this.LastPath != null)
        {
            TreeDir.DirNodeVisible v = listView.GetSelect();
            level = v.level;

        }
        rename.Enable(level == 0 || level == 2);
        rsync.Enable(level == 2);



    }
    public void OnDestroyDialog()
    {

        recyclerView.setAdapter(null);
        ((OrientActivity)getActivity()).CloseDialog();



    }

    @Override
    public Runnable GetResult() {
        return runnable;
    }




    @Override
    public DirEntry GetSelectedElement() {
        return null;
    }

    @Override
    public void DeleteSelectedElement()
    {

    }

    @Override
    public boolean IsSelectedElement() {
        return false;
    }

    @Override
    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        if (actionId == EditorInfo.IME_ACTION_DONE)
        {
            OnQuery();
            return true;
        }
        return false;
    }

    public void OnSelectDir(TreeDir.DirNodeVisible sel)
    {
        String ph = defaultPath + sel.dir.GetPath();
        rename = self.findViewById(R.id.idRename);
        int level = sel.level;

        rename.Enable(level == 0 || level == 2);
        rsync.Enable(level == 2);

        currentDir = new File(ph);
        AGlobals.rootHandler.Send(new Runnable() {
            @Override
            public void run()
            {
                Start();
            }
        });
    }
}