import pathlib
import subprocess as sp
import sys


class WhileRedirectedError(RuntimeError):
    def __init__(self, *args, file, **kwargs):
        super().__init__(*args, **kwargs)
        self.file = file.absolute() if isinstance(file, pathlib.Path) else file


class Tee:
    def __init__(self, name):
        self.file = open(name, "w")

    def __enter__(self):
        self.stdout = sys.stdout
        sys.stdout = self

    def __exit__(self, exc_type, exc, _):
        if exc_type is WhileRedirectedError:
            print(f"Error occurred while the output was redirected to '{exc.file}'")
        sys.stdout = self.stdout
        self.file.flush()
        self.file.close()

    def write(self, data):
        self.file.write(data)
        self.stdout.write(data)

    def __getattribute__(self, name):
        if name[0] != "_" and name not in ["file", "stdout", "write"]:
            return super().__getattribute__("file").__getattribute__(name)
        return super().__getattribute__(name)


def subprocess(*args, dest):
    """
    Wrapper to :class:`subprocess.Popen` to print the output to screen and capture it.

    Returns
    -------
        str :
            output
    """
    p = sp.Popen(*args, stdout=sp.PIPE, stderr=sp.STDOUT, cwd=dest)
    output = []

    while True:
        # returns None while subprocess is running
        retcode = p.poll()
        line = p.stdout.readline().decode()[:-1]
        if retcode is not None:
            break
        print(line)
        output.append(line)

    return "\n".join(output)
