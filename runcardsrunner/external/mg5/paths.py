# -*- coding: utf-8 -*-
import pathlib

subpkg = pathlib.Path(__file__).absolute().parent

cuts_code = subpkg / "cuts_code"
cuts_variables = subpkg / "cuts_variables"
patches = subpkg / "patches"
