package lib.common;

import lib.explorer.common.DirEntry;

public interface ISelectedElement {
    DirEntry GetSelectedElement();
    void DeleteSelectedElement();
    boolean IsSelectedElement();
}