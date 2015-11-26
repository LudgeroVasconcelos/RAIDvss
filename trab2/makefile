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

$(OBJ_DIR)/conffileutils.o: $(SRC_DIR)/conffileutils.c
	gcc $(CFLAGS) -c $(SRC_DIR)/conffileutils.c -o $(OBJ_DIR)/conffileutils.o

$(OBJ_DIR)/diskutils.o: $(SRC_DIR)/diskutils.c
	gcc $(CFLAGS) -c $(SRC_DIR)/diskutils.c -o $(OBJ_DIR)/diskutils.o

$(OBJ_DIR)/request.o: $(SRC_DIR)/request.c
	gcc $(CFLAGS) -c $(SRC_DIR)/request.c -o $(OBJ_DIR)/request.o
