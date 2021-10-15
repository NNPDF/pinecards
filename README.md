# Runcards for NNPDF

This repository stores all runcards needed to generate PineAPPL grids for the
processes included in NNPDF.

Moreover, it contains the tool (`rr` script and `runcardsrunner` package) to
generate the corresponding PineAPPL grids.

## Installation

There is no released version currently.

### Dev

For development the dependencies can be bootstrapped by running:

```sh
./rr install
```

## Documentation

The documentation is not deployed at the moment.
In order to generate it install the project in development, and then do:

```sh
poetry shell
cd docs
make html
make view
```
