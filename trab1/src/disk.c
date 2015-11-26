#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#include "raidvss.h"
#include "diskutils.h"
#include "conffile.h"
#include "conffileutils.h"
#include "ipc.h"
#include "disk.h"
#include "log.h"

/* Atraso em microsegundos para o disco se deslocar um sector */

#define DELAYSECTOR 10000

sector_t csector,seekdest;
direction_t cdir;
policy_t policy;
int disksize;

char* confname;
char* diskname;

