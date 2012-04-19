CC := gcc
OBJS := mongoose/mongoose.o \
cJSON/cjson.o

all: $(OBJS)

cJSON/cjson.o : cJSON/cJSON.c
	$(CC) -c $< -o $@

mongoose/mongoose.o : mongoose/mongoose.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS)
