# Runcards for NNPDF

This repository stores all runcards needed to generate PineAPPL grids for the
processes included in NNPDF.

Moreover, it contains the tool (`rr` script and `runcardsrunner` package) to
generate the corresponding PineAPPL grids.

## Installation

There is no released version currently.

### Dev

For development you need the following tools:

- `poetry`, follow [installation
  instructions](https://python-poetry.org/docs/#installation)
- `poetry-dynamic-versioning`, used to manage the version (see
  [repo](https://github.com/mtkennerly/poetry-dynamic-versioning))
- `pre-commit`, to run maintenance hooks before commits (see
  [instructions](https://pre-commit.com/#install))

See [below](#non-python-dependencies) for a few more dependencies (already
available on most systems).

## Documentation

The documentation is not deployed at the moment.
In order to generate it install the project in development, and then do:

```sh
poetry shell
cd docs
make html
make view
```

### Non Python dependencies

Even if the bootstrap script and the installation management try to reduce as
much as possible the amount of dependencies, still a few ingredients have to be
available on the system.

To run the CLI:

- `python` itself
- `pip` available as a module of the `python` that is running `rr` (as usually
  is)
- `curl`

To install `pineappl`:

- `pkg-config`
- `openssl.pc` (e.g. on Debian available in the `libssl-dev` package)

To install `mg5amc@nlo` and its dependencies:

- `gfortran`
- `wget`
