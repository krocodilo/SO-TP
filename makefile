target_dir = target
src_dir = src
bot_dir = src/bot

jogoUI: | $(target_dir)
	gcc $(src_dir)/jogoUI/jogoUI.c -o $(target_dir)/jogoUI -lncurses
	chmod +x $(target_dir)/jogoUI

motor: | $(target_dir)
	gcc -c $(src_dir)/motor/motor_init.c -o $(target_dir)/motor_init.o
	gcc $(src_dir)/motor/motor.c $(target_dir)/motor_init.o -o $(target_dir)/motor
	chmod +x $(target_dir)/motor

bot: | $(target_dir)
	gcc $(bot_dir)/bot.c -o $(target_dir)/bot
	chmod +x $(target_dir)/bot

all: jogoUI motor bot

clean:
	rm -f $(target_dir)/*.o $(target_dir)/jogoUI $(target_dir)/motor $(target_dir)/bot
	echo "Cleaned!"
	sleep 2

$(target_dir):
	mkdir -p $(target_dir)

