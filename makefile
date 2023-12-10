target_dir = target
motor_dir = src/motor
bot_dir = src/bot
jogoUI_dir = src/jogoUI


$(target_dir)/motor: $(target_dir)/bot | $(target_dir)
	gcc -c $(motor_dir)/motor_init.c -o $(target_dir)/motor_init.o
	gcc -c $(motor_dir)/motor_signup.c -o $(target_dir)/motor_signup.o
	gcc $(motor_dir)/motor.c $(target_dir)/motor_init.o $(target_dir)/motor_signup.o -o $(target_dir)/motor
	chmod +x $(target_dir)/motor

$(target_dir)/bot: | $(target_dir)
	gcc $(bot_dir)/bot.c -o $(target_dir)/bot
	chmod +x $(target_dir)/bot

$(target_dir)/jogoUI: | $(target_dir)
	gcc $(jogoUI_dir)/jogoUI.c -o $(target_dir)/jogoUI -lncurses
	chmod +x $(target_dir)/jogoUI


all: $(target_dir)/motor $(target_dir)/jogoUI #$(target_dir)/bot


clean:
	rm -f $(target_dir)/*
	echo "Files in target directory were deleted."

clean_objects:
	rm -f $(target_dir)/*.o
	echo "Object files were deleted."


$(target_dir):
	mkdir -p $(target_dir)

