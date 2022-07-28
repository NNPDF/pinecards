# -*- coding: utf-8 -*-
import dataclasses
import enum
import typing

from .external import interface, mg5, positivity, vrap, yad


class Kind(enum.Enum):
    dis = enum.auto()
    positivity = enum.auto()
    ftdy = enum.auto()
    hadronic = enum.auto()


@dataclasses.dataclass
class Info:
    color: str
    external: typing.Type[interface.External]
    kind: Kind


def label(dataset: str) -> Info:
    if yad.is_dis(dataset):
        return Info(color="red", external=yad.Yadism, kind=Kind.dis)
    if positivity.is_positivity(dataset):
        return Info(
            color="yellow", external=positivity.Positivity, kind=Kind.positivity
        )
    if vrap.is_vrap(dataset):
        return Info(color="green", external=vrap.Vrap, kind=Kind.ftdy)

    return Info(color="blue", external=mg5.Mg5, kind=Kind.hadronic)
