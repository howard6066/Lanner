
#Lanner Digital IO code

EXT=
RM=rm -rf
CP=cp -f
BIN_PATH=../bin/

ifneq ($(DJGPP),)

EXT=.exe
RM=del
CP=copy
BIN_PATH=..\bin\

endif 

TARGET=lcm_tst$(EXT)

all: $(TARGET)

$(TARGET): lcm_tst.o ioaccess.o
	-$(CC) -o $(TARGET) lcm_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

dio_tst.o: lcm_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) lcm_tst.o
	-$(RM) ioaccess.o
