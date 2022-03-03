## Dependencies

### More on tools

The `runcardsrunner` package is managed by `poetry`. This has been chosen to
automate a series of tasks, and to grant reproducible installations maintaining
a `poetry.lock` file.

It might happen (frequently) that `poetry` complaints about version
dependencies, if inconsistent. This is not `poetry`'s fault, and it's a useful
warning that will prevent to just break when installed by a user with a
different environment.

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
