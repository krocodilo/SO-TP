target_dir = target
motor_dir = src/motor
bot_dir = src/bot
jogoUI_dir = src/jogoUI


jogoUI: | $(target_dir)
	gcc $(jogoUI_dir)/jogoUI.c -o $(target_dir)/jogoUI -lncurses
	chmod +x $(target_dir)/jogoUI

motor: bot | $(target_dir)
	gcc -c $(motor_dir)/motor_init.c -o $(target_dir)/motor_init.o
	gcc $(motor_dir)/motor.c $(target_dir)/motor_init.o -o $(target_dir)/motor
	chmod +x $(target_dir)/motor

bot: | $(target_dir)
	gcc $(bot_dir)/bot.c -o $(target_dir)/bot
	chmod +x $(target_dir)/bot


all: jogoUI motor bot


clean:
	rm -fR $(target_dir)
	echo "Target directory was deleted."

clean_objects:
	rm -f $(target_dir)/*.o
	echo "Object files were deleted."


$(target_dir):
	mkdir -p $(target_dir)

