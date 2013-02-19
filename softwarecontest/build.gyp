{
    'targets': [
      {
        'target_name': 'game_engine',
        'type': 'executable',
        'sources': [
          'src/hello.cc',
          'src/Card.cpp',
          'src/CardGame.cpp',
          'src/CastleGameBoard.cpp',
          'src/CastlePlayer.cpp',
          'src/GameClient.cpp',
          'src/GameHost.cpp',
          'src/GameLobby.cpp',
          'src/SingleGame.cpp',
          'src/Socket.cpp',
          'src/testharness.cpp'
        ],
		'include_dirs': [
			'src/libuv/include'
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