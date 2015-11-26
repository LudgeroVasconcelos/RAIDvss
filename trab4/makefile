BASE_DIR = .
SRC_DIR = $(BASE_DIR)/src
INC_DIR = $(BASE_DIR)/include
OBJ_DIR = $(BASE_DIR)/obj
CFLAGS= -I $(INC_DIR)

all: controller disk

controller: $(OBJ_DIR)/controller.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o $(OBJ_DIR)/request.o
	gcc $(CFLAGS) $(OBJ_DIR)/controller.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o $(OBJ_DIR)/request.o -o controller -lrt -lpthread

disk: $(OBJ_DIR)/diskutils.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o $(OBJ_DIR)/log.o $(OBJ_DIR)/disk.o $(OBJ_DIR)/request.o
	gcc $(CFLAGS) $(OBJ_DIR)/diskutils.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o $(OBJ_DIR)/log.o $(OBJ_DIR)/disk.o $(OBJ_DIR)/request.o -o disk -lrt -lpthread

$(OBJ_DIR)/log.o: $(SRC_DIR)/log.c
	gcc $(CFLAGS) -c $(SRC_DIR)/log.c -o $(OBJ_DIR)/log.o

$(OBJ_DIR)/conffile.o: $(SRC_DIR)/conffile.c
	gcc $(CFLAGS) -c $(SRC_DIR)/conffile.c -o $(OBJ_DIR)/conffile.o

$(OBJ_DIR)/controller.o: $(SRC_DIR)/controller.c
	gcc $(CFLAGS) -c $(SRC_DIR)/controller.c -o $(OBJ_DIR)/controller.o

$(OBJ_DIR)/disk.o: $(SRC_DIR)/disk.c
	gcc $(CFLAGS) -c $(SRC_DIR)/disk.c -o $(OBJ_DIR)/disk.o
