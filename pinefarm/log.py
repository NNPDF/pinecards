# -*- coding: utf-8 -*-
import pathlib
import subprocess as sp
import sys


class WhileRedirectedError(RuntimeError):
    """Error to signal a generic error, while stderr was redirected to file

    Parameters
    ----------
    *args
        arguments passed to :class:`RuntimeError`
    file : str
        path to file to which stderr is redirected
    **kwargs
        keyword arguments passed to :class:`RuntimeError`

    """

    def __init__(self, *args, file, **kwargs):
        super().__init__(*args, **kwargs)
        self.file = file.absolute() if isinstance(file, pathlib.Path) else file


class ChildStream:
    def __init__(self, parent):
        self.parent = parent

    def write(self, data):
        self.parent.write(data, self)

    def __getattribute__(self, name):
        if name[0] != "_" and name not in ["parent", "write"]:
            return super().__getattribute__("parent").__getattribute__(name)
        return super().__getattribute__(name)


class Tee:
    """Context manager to tee stdout to file

    Parameters
    ----------
    name : str or pathlib.Path
        path to redirect stdout to

    """

    def __init__(self, name, stdout=True, stderr=False):
        self.file = open(name, "w")
        self.stdout = ChildStream(self) if stdout else sys.stdout
        self.stderr = ChildStream(self) if stderr else sys.stderr

    def __enter__(self):
        self.stdout_bk = sys.stdout
        self.stderr_bk = sys.stderr
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        return self

    def __exit__(self, exc_type, exc, _):
        if exc_type is WhileRedirectedError:
            self.write(
                f"Error occurred while the output was redirected to '{exc.file}'",
                self.stderr,
            )
        sys.stdout = self.stdout_bk
        sys.stderr = self.stderr_bk
        self.file.flush()
        self.file.close()

    def write(self, data, stream):
        self.file.write(data)
        if stream is self.stdout:
            self.stdout_bk.write(data)
        elif stream is self.stderr:
            self.stderr_bk.write(data)

    def flush(self):
        self.file.flush()

    def __getattribute__(self, name):
        if name[0] != "_" and name not in [
            "file",
            "stdout",
            "stderr",
            "stdout_bk",
            "stderr_bk",
            "write",
        ]:
            return super().__getattribute__("file").__getattribute__(name)
        return super().__getattribute__(name)


def subprocess(*args, cwd, out):
    """Wrapper to :class:`subprocess.Popen` to print the output to screen and capture it.

    Parameters
    ----------
    args
        positional arguments to pass to `subprocess.Popen`
    cwd : path-like or str
        directory where to execute the command
    out : path-like or str
        file to which (also) redirect the output

    Returns
    -------
    str
        output of the command run in the subprocess

    """
    p = sp.Popen(*args, stdout=sp.PIPE, stderr=sp.STDOUT, cwd=cwd)

    try:
        with open(out, "w") as fd:
            while True:
                # returns None while subprocess is running
                retcode = p.poll()
                line = p.stdout.readline().decode()[:-1]
                if retcode is not None:
                    break
                print(line)

                fd.write(line + "\n")
    except Exception:
        raise WhileRedirectedError(file=out)
