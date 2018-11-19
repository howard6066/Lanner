
#Lanner EEPROM code

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

TARGET=eep_tst$(EXT)

all: $(TARGET)

$(TARGET): eep_tst.o ioaccess.o
	-$(CC) -o $(TARGET) eep_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

eep_tst.o: eep_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) eep_tst.o
	-$(RM) ioaccess.o
