{
  "targets": [
    {
      "target_name": "clipper",
      "sources": [
        "src/init.cc"
      ],
        'cflags': [ '-fexceptions' ],
        'cflags_cc': [ '-fexceptions' ],

      "conditions": [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' #-fno-exceptions
          }
        }],
        [ 'OS=="linux"', {
            'target_defaults': {
                'cflags': [ '-fexceptions' ],
                'cflags_cc': [ '-fexceptions' ],
            }
        }]
      ]
    }
  ]
}