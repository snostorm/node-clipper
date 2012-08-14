{
  "targets": [
    {
      "target_name": "clipper",
      "sources": [ 
        "src/init.cc"
      ],
      "conditions": [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' #-fno-exceptions
          }
        }]
      ]
    }
  ]
}