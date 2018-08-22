OBJ := server.o show_bmp.o touch.o show_jpg.o video.o

all:
	arm-linux-gcc -c show_jpg.c -o show_jpg.o -I ./libjpeg/ -L ./libjpeg/ 
	arm-linux-gcc -c video.c -o video.o -I ./libjpeg/ -L ./libjpeg/ 
	arm-linux-gcc -c server.c -o server.o -I ./libjpeg/ -L ./libjpeg/  -pthread
	arm-linux-gcc -c show_bmp.c -o show_bmp.o
	arm-linux-gcc -c touch.c -o touch.o
	arm-linux-gcc $(OBJ) -o server -pthread -I ./libjpeg/ -L ./libjpeg/ -ljpeg -lapi_v4l2_arm
	
clean:
	rm *.o