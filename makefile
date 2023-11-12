comp_dir = out
bin_dir = bin
src_dir = src


jogoUI:
	gcc $(src_dir)/jogoUI/jogoUI.c -o $(bin_dir)/jogoUI -lncurses
	chmod +X $(bin_dir)/jogoUI

motor:
	gcc $(src_dir)/motor/motor.c -o $(bin_dir)/motor
	chmod +X $(bin_dir)/motor

all: jogoUI motor


clean:
	rm $(comp_dir)/*.o
	echo "Cleaned!"
	sleep 2