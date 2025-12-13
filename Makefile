CFLAGS=`pkg-config --cflags gtk4` -Wall -g
LIBS=`pkg-config --libs gtk4`

build:
	gcc main.c spreadsheet-view.c -o spreadsheet $(CFLAGS) $(LIBS)

clean:
	rm -f spreadsheet *.o

format:
	clang-format -i *.c *.h
