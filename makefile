target_dir = target
src_dir = src


jogoUI:
	gcc $(src_dir)/jogoUI/jogoUI.c -o $(target_dir)/jogoUI -lncurses
	chmod +X $(target_dir)/jogoUI

motor:
	gcc -c $(src_dir)/motor/motor_init.c -o $(target_dir)/motor_init.o
	gcc $(src_dir)/motor/motor.c $(target_dir)/motor_init.o -o $(target_dir)/motor
	chmod +X $(target_dir)/motor

all: jogoUI motor


clean:
	rm $(target_dir)/*.o
	echo "Cleaned!"
	sleep 2
comp_dir = out
bin_dir = bin
src_dir = src


jogoUI:
	gcc $(src_dir)/jogoUI/jogoUI.c -o $(bin_dir)/jogoUI
	chmod +X $(bin_dir)/jogoUI

motor:
	gcc $(src_dir)/motor/motor.c -o $(bin_dir)/motor
	chmod +X $(bin_dir)/motor

all: jogoUI motor


clean:
	rm $(comp_dir)/*.o
	echo "Cleaned!"
	sleep 2