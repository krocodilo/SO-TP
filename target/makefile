motor_dir = ../src/motor
bot_dir = ../src/bot
jogoUI_dir = ../src/jogoUI
common_dir = ../src/common


motor: bot utils.o $(motor_dir)/* $(common_dir)/*
	gcc -c $(motor_dir)/motor_init.c -o motor_init.o
	gcc -c $(motor_dir)/motor_signup.c -o motor_signup.o
	gcc -o motor $(motor_dir)/motor.c utils.o motor_init.o motor_signup.o
	chmod +x motor

bot: $(bot_dir)/*
	gcc $(bot_dir)/bot.c -o bot
	chmod +x bot

jogoUI: $(jogoUI_dir)/* $(common_dir)/*
	gcc $(jogoUI_dir)/jogoUI.c -o jogoUI -lncurses
	chmod +x jogoUI

utils.o:
	gcc -c $(common_dir)/utils.c -o utils.o

all: motor jogoUI


clean: clean_objects
	rm -f motor jogoUI bot

clean_objects:
	rm -f ./*.o


#$(target_dir):
#	mkdir -p $(target_dir)

