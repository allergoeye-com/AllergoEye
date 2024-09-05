package lib.common;

/**
 * Created by alex on 11.11.17.
 */

public interface INotify {
    void OnNotify(long cmd, long wparam, long lparam);
    void OnNotify(Object obj, Object data);
}
