
#Lanner Case Open Makefile

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

TARGET=cp_tst$(EXT)

all: $(TARGET)

$(TARGET): cp_tst.o ioaccess.o
	-$(CC) -o $(TARGET) cp_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

cp_tst.o: cp_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) cp_tst.o
	-$(RM) ioaccess.o
