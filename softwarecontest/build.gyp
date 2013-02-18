{
    'targets': [
      {
        'target_name': 'my_target',
        'type': 'executable',
        'sources': [
          'src/hello.cc',
        ],
		'include_dirs': [
			'../lib/libuv/include'
		],
		'msvs_settings': {
		
		'VCLinkerTool': {
            'AdditionalLibraryDirectories': [
              '../lib/libuv/Release',
            ],
			'AdditionalDependencies': [
				'ws2_32.lib',
				'psapi.lib',
				'iphlpapi.lib'
			]
			}
		},
		'libraries': [
			'libuv'
		]
      },
    ],
}