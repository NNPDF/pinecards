# Runcards for NNPDF

This repository stores all pinecards needed to generate PineAPPL grids for the
processes included in NNPDF.

Moreover, it contains the tool (`pinefarm` script and `pinefarm` package) to
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

See [below](.github/CONTRIBUTING.md#non-python-dependencies) for a few more
dependencies (already available on most systems).

## Documentation

The documentation is not deployed at the moment.
In order to generate it install the project in development, and then do:

```sh
poetry shell
cd docs
make html
make view
```
