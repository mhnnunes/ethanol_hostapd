LIBS=ethanol_config_file.o ini.o

all: libs 

libs:
	$(CC) -c ini.c
	$(CC) -c ethanol_config_file.c

config_file: ethanol_config_file.c
	$(CC) -DDEBUG_MAIN -o ethanol_config_file ini.o ethanol_config_file.c

clean:
	rm -rf ethanol_config_file *~ *.swp
