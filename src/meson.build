#
# target: libbus1-api.so
#

libbus1_api_symfile = join_paths(meson.current_source_dir(), 'bus1-api.sym')

libbus1_api_deps = []

libbus1_api_private = static_library(
        'bus1-api-private',
        [
                'api.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libbus1_api_deps,
        pic: true,
)

libbus1_api_shared = shared_library(
        'bus1-api',
        objects: libbus1_api_private.extract_all_objects(),
        dependencies: libbus1_api_deps,
        install: not meson.is_subproject(),
        soversion: 0,
        link_depends: libbus1_api_symfile,
        link_args: [
                '-Wl,--no-undefined',
                '-Wl,--version-script=@0@'.format(libbus1_api_symfile),
        ],
)

libbus1_api_dep = declare_dependency(
        include_directories: include_directories('.'),
        link_with: libbus1_api_private,
        dependencies: libbus1_api_deps,
        version: meson.project_version(),
)

if not meson.is_subproject()
        install_headers('bus1-api.h')

        mod_pkgconfig.generate(
                libraries: libbus1_api_shared,
                version: meson.project_version(),
                name: 'libbus1-api',
                filebase: 'libbus1-api',
                description: project_description,
        )
endif

#
# target: test-*
#

test_api = executable('test-api', ['test-api.c'], link_with: libbus1_api_shared)
test('API Symbol Visibility', test_api)

test_basic = executable('test-basic', ['test-basic.c'], dependencies: libbus1_api_dep)
test('Basic API Behavior', test_basic)
