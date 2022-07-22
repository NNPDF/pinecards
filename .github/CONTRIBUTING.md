## Dependencies

### More on tools

The `runcardsrunner` package is managed by `poetry`. This has been chosen to
automate a series of tasks, and to grant reproducible installations maintaining
a `poetry.lock` file.

It might happen (frequently) that `poetry` complains about version
dependencies, if inconsistent. This is not `poetry`'s fault, and it's a useful
warning that will prevent to just break when installed by an user with a
different environment.
