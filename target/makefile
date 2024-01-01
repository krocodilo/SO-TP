motor_dir = ../src/motor
bot_dir = ../src/bot
jogoUI_dir = ../src/jogoUI
common_dir = ../src/common


all: motor jogoUI

motor: bot utils.o $(motor_dir)/* $(common_dir)/*
	gcc -c $(motor_dir)/motor_init.c -o motor_init.o
	gcc -c $(motor_dir)/motor_signup.c -o motor_signup.o
	gcc -c $(motor_dir)/motor_game.c -o motor_game.o -pthread
	gcc -c $(motor_dir)/motor_bots.c -o motor_bots.o
	gcc -c $(motor_dir)/motor_adminConsole.c -o motor_adminConsole.o -pthread
	gcc -c $(motor_dir)/motor_backend.c -o motor_backend.o -pthread -lncurses
	gcc -pthread -o motor $(motor_dir)/motor.c utils.o motor_init.o motor_signup.o motor_game.o motor_bots.o motor_backend.o motor_adminConsole.o

bot: $(bot_dir)/*
	gcc $(bot_dir)/bot.c -o bot

jogoUI: utils.o $(jogoUI_dir)/* $(common_dir)/*
	gcc -c $(jogoUI_dir)/windows_ncurses.c -o windows_ncurses.o -lncurses
	gcc -c $(jogoUI_dir)/jogoUI_backend.c -o jogoUI_backend.o
	gcc -o jogoUI $(jogoUI_dir)/jogoUI.c utils.o windows_ncurses.o jogoUI_backend.o -lncurses -pthread

utils.o: $(common_dir)/utils.c $(common_dir)/utils.h
	gcc -c $(common_dir)/utils.c -o utils.o



clean: clean_objects
	rm -f motor jogoUI bot

clean_objects:
	rm -f ./*.o
