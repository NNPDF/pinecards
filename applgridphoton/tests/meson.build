catch_dep = dependency('catch2', fallback : [ 'catch2', 'catch2_dep' ])

# make it a static library because `catch.cpp` contains `main`
libcatch = static_library('catch', 'catch.cpp', dependencies : catch_dep)
libcatch_dep = declare_dependency(dependencies : catch_dep, link_with : libcatch)

tests = [
    'basic',
    'files',
    'storage',
]

foreach t : tests
    exe = executable(t, t + '.cpp', dependencies : [ libcatch_dep, dep ])
    test(t, exe)
endforeach
