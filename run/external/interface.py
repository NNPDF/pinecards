import abc


class External(abc.ABC):
    @abc.abstractmethod
    def run(self):
        pass

    @abc.abstractmethod
    def postprocess(self):
        pass

    @abc.abstractproperty
    def results(self):
        pass
