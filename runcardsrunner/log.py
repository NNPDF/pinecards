import subprocess as sp
import sys


class Tee:
    def __enter__(self, name):
        self.file = open(name, "w")
        self.stdout = sys.stdout
        sys.stdout = self

    def __exit__(self):
        sys.stdout = self.stdout
        self.file.close()

    def write(self, data):
        self.file.write(data)
        self.stdout.write(data)

    def __getattribute__(self, name):
        if name[0] != "_" and name not in ["stdout", "file", "write"]:
            return self.file.__getattribute__(name)
        else:
            raise AttributeError


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
