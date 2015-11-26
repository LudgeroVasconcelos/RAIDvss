BASE_DIR = .
SRC_DIR = $(BASE_DIR)/src
INC_DIR = $(BASE_DIR)/include
OBJ_DIR = $(BASE_DIR)/obj
CFLAGS = -Wall -I $(INC_DIR)

$(BASE_DIR)/controller: $(OBJ_DIR)/controller.o $(OBJ_DIR)/request.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o
	gcc $(CFLAGS) $(OBJ_DIR)/controller.o $(OBJ_DIR)/request.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o -o $(BASE_DIR)/controller

$(BASE_DIR)/disk: $(OBJ_DIR)/disk.o $(OBJ_DIR)/diskutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/ipc.o $(OBJ_DIR)/log.o
	gcc $(CFLAGS) $(OBJ_DIR)/controller.o $(OBJ_DIR)/request.o $(OBJ_DIR)/conffileutils.o $(OBJ_DIR)/conffile.o $(OBJ_DIR)/ipc.o -o $(BASE_DIR)/disk

$(OBJ_DIR)/ipc.o: $(INC_DIR)/ipc.h
	gcc $(CFLAGS) -c $(SRC_DIR)/ipc.c -o $(OBJ_DIR)/ipc.o

$(OBJ_DIR)/request.o: $(INC_DIR)/request.h
	gcc $(CFLAGS) -c $(SRC_DIR)/request.c -o $(OBJ_DIR)/request.o

$(OBJ_DIR)/conffile.o: $(INC_DIR)/raidvss.h $(INC_DIR)/conffile.h $(INC_DIR)/conffileutils.h
	gcc $(CFLAGS) -c $(SRC_DIR)/conffile.c -o $(OBJ_DIR)/conffile.o

$(OBJ_DIR)/conffileutils.o: $(INC_DIR)/raidvss.h $(INC_DIR)/conffileutils.h
	gcc $(CFLAGS) -c $(SRC_DIR)/conffileutils.c -o $(OBJ_DIR)/conffileutils.o

$(OBJ_DIR)/controller.o: $(INC_DIR)/raidvss.h $(INC_DIR)/request.h $(INC_DIR)/conffileutils.h $(INC_DIR)/conffile.h $(INC_DIR)/ipc.h
	gcc $(CFLAGS) -c $(SRC_DIR)/controller.c -o $(OBJ_DIR)/controller.o

$(OBJ_DIR)/disk.o: $(INC_DIR)/raidvss.h $(INC_DIR)/diskutils.h $(INC_DIR)/conffile.h $(INC_DIR)/conffileutils.h $(INC_DIR)/ipc.h $(INC_DIR)/disk.h $(INC_DIR)/log.h
	gcc $(CFLAGS) -c $(SRC_DIR)/disk.c -o $(OBJ_DIR)/disk.o

$(OBJ_DIR)/diskutils.o: $(INC_DIR)/diskutils.h $(INC_DIR)/ipc.h $(INC_DIR)/disk.h
	gcc $(CFLAGS) -c $(SRC_DIR)/diskutils.c -o $(OBJ_DIR)/diskutils.o

$(OBJ_DIR)/log.o: $(INC_DIR)/log.h $(INC_DIR)/raidvss.h $(INC_DIR)/diskutils.h $(INC_DIR)/disk.h
	gcc $(CFLAGS) -c $(SRC_DIR)/log.c -o $(OBJ_DIR)/log.o
	
clean:
	find $(BASE_DIR) -type f -not -name '*.c' -not -name '*.h' -not -name "makefile" -not -name "mkimage" -not -name "vsscfg" -not -name "vsslr" | xargs rm -r

backup:
	tar -czvf backup_`date +%m%d%H%M`.tgz $(SRC_DIR)/*.c  $(INC_DIR)/*.h makefile ./mkimage ./vsscfg ./vsslr
	
restore:
	cp -ur ~/areas_de_grupo/so000/raidvss/* .

