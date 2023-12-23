motor_dir = ../src/motor
bot_dir = ../src/bot
jogoUI_dir = ../src/jogoUI
common_dir = ../src/common


all: motor jogoUI jogoUI2

motor: bot utils.o $(motor_dir)/* $(common_dir)/*
	gcc -c $(motor_dir)/motor_init.c -o motor_init.o
	gcc -c $(motor_dir)/motor_signup.c -o motor_signup.o
	gcc -pthread -o motor $(motor_dir)/motor.c utils.o motor_init.o motor_signup.o

bot: $(bot_dir)/*
	gcc $(bot_dir)/bot.c -o bot

jogoUI: $(jogoUI_dir)/* $(common_dir)/* 
	gcc -c $(jogoUI_dir)/windows_ncurses.c -o windows_ncurses.o -lncurses
	gcc -o jogoUI $(jogoUI_dir)/jogoUI.c windows_ncurses.o -lncurses

utils.o: $(common_dir)/utils.c $(common_dir)/utils.h
	gcc -c $(common_dir)/utils.c -o utils.o



clean: clean_objects
	rm -f motor jogoUI bot

clean_objects:
	rm -f ./*.o




jogoUI2: utils.o $(jogoUI_dir)/* $(common_dir)/*
	gcc $(jogoUI_dir)/jogoUI2.c utils.o -o jogoUI
