package lib.explorer;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;

import androidx.annotation.Nullable;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import androidx.appcompat.app.ActionBar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.appcompat.widget.Toolbar;

import android.view.View;
import android.widget.ProgressBar;


import com.dryeye.app.R;

import lib.OrientActivity;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.StringTokenizer;

import lib.common.ActionCallback;
import lib.common.CComboBox;
import lib.common.CEdit;
import lib.common.OnClickCalback;
import lib.common.button.CMenuButton;
import lib.explorer.common.DirEntry;
import lib.explorer.common.IQuerySelectedPage;
import lib.utils.AGlobals;

public class ExplorerActivity extends OrientActivity implements IQuerySelectedPage {
    protected TaskReadDir _task = null;
    ArrayList<String> dirs = null;
    protected AGlobals.CasheFromExplore cache = null;
    protected RecyclerView recyclerView = null;
    protected GridLayoutManager gridLayoutManager = null;
    protected ProgressBar bar = null;
    protected AdapterExplorerView mAdapter;
    ProgressBar waitCursor;


    File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_explorer);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayOptions(0, ActionBar.DISPLAY_SHOW_TITLE);
        recyclerView = (RecyclerView) findViewById(R.id.recyclerViewImageFiles);
        recyclerView.setHasFixedSize(true);
        gridLayoutManager = new GridLayoutManager(this, 5);
        recyclerView.setLayoutManager(gridLayoutManager);
        String p = AGlobals.ReadIni("CURRENT", "PATH", "explorer.ini");
        if (p != null && p.length() > 0)
            path = new File(p);
        if (!path.exists())
            path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
        waitCursor = (ProgressBar) findViewById(R.id.waitCursor);

        cache = AGlobals.CreateCache("thromb_local");
        CMenuButton btn = findViewById(R.id.idHome);
        btn.SetCallback(this, "OnHome");
        btn = findViewById(R.id.idBack);
        btn.SetCallback(this, "OnBack");

        btn = (CMenuButton) findViewById(R.id.searchFieldSearchButton);
        btn.SetCallback(this, "OnQuery");

        InitDirList();
        CreateAdapter();
        Start();


    }

    public void OnHome()
    {
        Intent data = new Intent();
        setResult(RESULT_CANCELED, data);
        finish();
    }

    public void OnBack()
    {
        if (dirs.size() > 1)
            OnSelectDir(dirs.size() - 2);
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    void InitDirList()
    {
        StringTokenizer st = new StringTokenizer(path.getAbsolutePath(), "/");
        CComboBox box = (CComboBox) findViewById(R.id.menuDir);
        dirs = new ArrayList<String>();

        int i = 0;
        dirs.add("/");
        while (st.hasMoreTokens())
        {
            ++i;
            if (i > 3)
                dirs.add(st.nextToken());
            else
                st.nextToken();
        }

        box.Init(dirs, dirs.size() - 1, new ActionCallback(this, "OnSelectDir", int.class));
    }

    public void OnSelectDir(int index)
    {
        StringTokenizer st = new StringTokenizer(path.getAbsolutePath(), "/");
        ArrayList<String> d = new ArrayList<String>();
        String spath = "/";
        for (int i = 0; i < 3; ++i)
        {
            spath += st.nextToken();
            if (i < 2)
                spath += "/";
        }

        for (int i = 0, l = dirs.size(); i <= index; ++i)
        {
            spath += dirs.get(i);
            if (i < index)
                spath += "/";
        }
        path = new File(spath);
        InitDirList();
        Start();


    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onDestroy()
    {
        AGlobals.SaveIni("CURRENT", "PATH", path.getAbsolutePath(), "explorer.ini");

        super.onDestroy();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public void finish()
    {
        super.finish();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onPause()
    {
        super.onPause();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onResume()
    {
        super.onResume();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    protected void CreateAdapter()
    {
        OnSelect(null);
        if (_task != null)
        {
            try
            {
                _task.close();
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        _task = new TaskReadDir();
        mAdapter = new AdapterExplorerView(gridLayoutManager, cache, _task, new ActionCallback(this, "OnSelect", DirEntry.class));
        recyclerView.setAdapter(mAdapter);


    }

    @Override
    public void onBackPressed()
    {

        if (dirs.size() > 1)
            OnSelectDir(dirs.size() - 2);
        else
            finish();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void OnSelect(DirEntry b)
    {
        try
        {
            FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.actionProperty);

            if (fab != null)
            {

                if (b != null)
                {
                    if (b.fIsDir)
                    {
                        fab.setOnClickListener(null);
                        //  fab.setVisibility(View.GONE);
                        AGlobals.hideFloatingActionButton(fab);
                        path = new File(b.img.url);
                        if (!path.exists())
                            path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
                        InitDirList();
                        Start();
                    } else
                    {
                        //fab.setVisibility(View.VISIBLE);
                        AGlobals.showFloatingActionButton(fab);
                        fab.setOnClickListener(new OnClickCalback(this, "OnClickDownload"));
                    }
                } else
                {
                    fab.setOnClickListener(null);
                    //fab.setVisibility(View.GONE);
                    AGlobals.hideFloatingActionButton(fab);
                }
            }
        } catch (Exception e)
        {
            ;
        }
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public void OnClickDownload(View v)
    {
        Intent data = new Intent();
        DirEntry b = mAdapter.GetSelectedElement();
        String text = b.img.url;
        data.setData(Uri.parse(text));
        setResult(RESULT_OK, data);
        finish();
    }

    public void OnQuery()
    {
        AGlobals.ShowKeyboard(false);
        CEdit editText = (CEdit) findViewById(R.id.searchFieldEditText);
        String query = editText.getText().toString();
        Query(query);

    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public void Query(String query)
    {
        mAdapter.Select(query);
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public void Show() throws NoSuchMethodException
    {
        if (waitCursor != null)
            waitCursor.setVisibility(View.INVISIBLE);

        CreateAdapter();
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

    @Override
    public void Start()
    {
        if (waitCursor != null)
            waitCursor.setVisibility(View.VISIBLE);

        bar = findViewById(R.id.progress_bar);
        cache.cache.ClearMemory();
        cache.result.clear();
        DirLocal th = new DirLocal(this, path, AGlobals.MaskPicture(), bar, cache.result);
        th.execute();

    }
}
