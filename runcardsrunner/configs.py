# -*- coding: utf-8 -*-
import copy

configs = None
"Holds loaded configurations"
name = "runcardsrunner.toml"
"Name of the config while (wherever it is placed)"


def defaults(base_configs):
    configs = copy.deepcopy(base_configs)

    return configs
