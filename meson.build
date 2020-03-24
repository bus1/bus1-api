project(
        'bus1-api',
        'c',
        version: '1',
        license: 'Apache',
        default_options: [
                'c_std=c11'
        ],
)
project_description = 'Bindings to the Linux Bus1 Syscall API'

add_project_arguments('-D_GNU_SOURCE', language: 'c')
mod_pkgconfig = import('pkgconfig')
prog_rst2man = find_program('rst2man', 'rst2man.py')

subdir('docs')
subdir('src')
