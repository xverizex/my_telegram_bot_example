cmake_minimum_required (VERSION 3.12)

project (my_bot VERSION 0.0)

add_subdirectory (telegram_api_for_bot)

add_executable (my_bot 
	main.c
	)

include_directories (my_bot PUBLIC
	"telegram_api_for_bot/include"
	"telegram_api_for_bot/libcreqhttp/include"
	)

target_link_libraries (my_bot PUBLIC
	tebot
	)
