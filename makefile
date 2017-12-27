TARGET=main
CC=clang
CC?=clang
CC_FLAGS=
CC_EXTRA?=
CC_FLAGS+=$(CC_EXTRA)

.DEFAULT:all clean

.PHONY:all clean $(TARGET) help

all:$(TARGET)

.c.o:
	$(CC) $(CC_FLAGS) -c $< -o $@ 

android:
	export NDK_PROJECT_PATH=`pwd`
	ndk-build

$(TARGET): main.o
	$(CC) $^ $(LD_FLAGS) -o $@

clean:
	rm -f *.o *~ $(TARGET)

help:
	@echo "all is the default target"
	@echo "there is delete."
	@echo "there is clean."
