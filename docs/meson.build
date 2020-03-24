custom_target(
        'man-bus1',
        output: 'bus1.7',
        input: 'bus1.7.rst',
        command: [prog_rst2man, '@INPUT@', '@OUTPUT@'],
        install: true,
        install_dir: join_paths(get_option('mandir'), 'man7')
)

custom_target(
        'man-bus1-api',
        output: 'bus1-api.7',
        input: 'bus1-api.7.rst',
        command: [prog_rst2man, '@INPUT@', '@OUTPUT@'],
        install: true,
        install_dir: join_paths(get_option('mandir'), 'man7')
)
