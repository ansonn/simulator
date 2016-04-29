/****************************************************************************
 *
 * Copyright (c) 2010 - 2014 PMC-Sierra, Inc.
 *      All Rights Reserved
 *
 * Distribution of source code or binaries derived from this file is not
 * permitted except as specifically allowed for in the PMC-Sierra
 * Software License agreement.  All copies of this source code
 * modified or unmodified must retain this entire copyright notice and
 * comment as is.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Description                  : Application used to automatically generate
 *                                  ipcmap.h and ipcmap.c
 *
 * $Revision: 17059 $
 * $Date: 2014-05-10 00:31:12 +0300 (Sat, 10 May 2014) $
 * $Author: yelisett $
 * Release $Name: PMC_EFC_2_0_3 $
 *
 ****************************************************************************/

/*
 * main.c
 * This file is part of PBLAZE 4
 *
 * Copyright (C) 2015 - Beijing Memblaze Technology Co., Ltd.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Beijing Memblaze Technology Co., Ltd and its
 * suppliers, if any.  The intellectual and technical concepts
 * contained herein are proprietary to Beijing Memblaze
 * Technology Co., Ltd and its suppliers and may be covered by
 * People's Republic of China and Foreign Patents,patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission
 * is obtained from Beijing Memblaze Technology Co., Ltd.
 *
 * Distribution of source code or binaries derived from this file is not
 * permitted except as specifically allowed By <<Non_Disclosure Agreement>>
 * between your company and Beijing Memblaze Technology Co., Ltd.
 * All copies of this source code modified or unmodified must retain
 * this entire copyright notice and comment as is.
 *
 * You should have received a copy of the <<Non-Disclosure Agreement>>
 * along with this program; if not, Please contact your manager immediately.
 *
 * Description:
 *
 * $Revision:  $
 * $Date:  $
 * $Author:  $
 * Release $Name:  $
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* use '-u' argument to sepecify the unit test manager when needed */
char *UTMgrName = 0;
char *CtrlMgrName = "CTRL_MGR";
int verbose = 0;

#define LOG(fmt, ...) if (verbose) { printf(fmt, ##__VA_ARGS__);}
#define ASSERT(x) if (UTMgrName) { assert(x); }

#define BITS_PER_LONG 32
#if BITS_PER_LONG == 64
typedef unsigned long u64;
#else
typedef unsigned long long u64;
#endif

typedef unsigned int u32;

#define QUEUES_PER_NODE 64

#define MAX_MGR_NAME    16
#define MAX_QUEUE_NAME  32
#define MAX_VAR_NAME    32
#define MAX_QUEUES      512
#define MAX_MANAGERS    64

#define IS_HW_MSGQCTRL      1
#define IS_HW_NO_MSGQCTRL   3

typedef struct
{
    char obManager[MAX_MGR_NAME];
    char ibManager[MAX_MGR_NAME];
    char effectiveObManager[MAX_MGR_NAME];
    char effectiveIbManager[MAX_MGR_NAME];

    char name[MAX_QUEUE_NAME];
    int inboundIndex;
    int outboundIndex;
    int depth;
    int messageSize;
    char buffer[MAX_VAR_NAME];
    unsigned int bufferAddr;
} QUEUE;

typedef struct
{
    char managerName[MAX_MGR_NAME];
    u64 outboundQueueMask;
    u64 usedOutbound;
    u64 inboundQueueMask;
    u64 usedInbound;
    unsigned int address;
    char varName[MAX_VAR_NAME];
    int varOffset;
    int isHardware;
    int matchOutbound;
    int pseudoQueueMap[26]; // a-z
    int maxOutboundMessageBuffers;
    int maxInboundMessageBuffers[QUEUES_PER_NODE];
} MANAGER;

static char *managerFileName = "managers.txt";
static char *configFileName = "config.txt";
static char *mapName = "map";

QUEUE queues[MAX_QUEUES];

MANAGER managers[MAX_MANAGERS];

void displayHeader(FILE *stream)
{
    fprintf(stream, "// This file was automatically generated using %s and %s\n", managerFileName, configFileName);
    fprintf(stream, "/****************************************************************************\n");
    fprintf(stream, "*\n");
    fprintf(stream, "* Copyright (c) 2010 - 2014 PMC-Sierra, Inc.\n");
    fprintf(stream, "*      All Rights Reserved\n");
    fprintf(stream, "*\n");
    fprintf(stream, "* Distribution of source code or binaries derived from this file is not\n");
    fprintf(stream, "* permitted except as specifically allowed for in the PMC-Sierra\n");
    fprintf(stream, "* Software License agreement.  All copies of this source code\n");
    fprintf(stream, "* modified or unmodified must retain this entire copyright notice and\n");
    fprintf(stream, "* comment as is.\n");
    fprintf(stream, "*\n");
    fprintf(stream, "* THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR\n");
    fprintf(stream, "* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED\n");
    fprintf(stream, "* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n");
    fprintf(stream, "*\n");
    fprintf(stream, "* Description: The contents of this file are auto-generated by the \n");
    fprintf(stream, "*              IPCMapGenerator it contains IPC map information.\n");
    fprintf(stream, "*\n");
    fprintf(stream, "* $" "RCSfile" "$\n");
    fprintf(stream, "* $" "Revision" "$\n");
    fprintf(stream, "* $" "Date" "$\n");
    fprintf(stream, "* $" "Author" "$\n");
    fprintf(stream, "* Release $" "Name" "$\n");
    fprintf(stream, "*\n");
    fprintf(stream, "****************************************************************************/\n");
}

/**
 * @brief Obtain a pointer to the manager info structure.
 *
 * @param name Pointer to manager text name.
 *
 * @return Pointer to specified manager data structure on success, else NULL.
 */
MANAGER *getManagerByName(char *name)
{
    int n = 0;
    while (strlen(managers[n].managerName))
    {
        if (strcmp(name, managers[n].managerName) == 0)
            return (&managers[n]);
        n++;
    }
    return (NULL);
}

int getLastInbound(char *name, char *obMgrName, char *ibMgrName)
{
    int n;
    int index;

    for (n=0; strlen(managers[n].managerName); n++)
    {
        if (strcmp(name, managers[n].managerName) == 0)
        {
            /*
             * find a free bit
             */
            for (index=0; index<QUEUES_PER_NODE; index++)
            {
                if ((managers[n].inboundQueueMask & (1LL<<index)) == 0)
                {
                    LOG("[%s] getLastInbound with index %u for %s->%s\n", name, index - 1, obMgrName, ibMgrName);
                    return (index - 1);
                }
            }
        }
    }

    return (-1);
}
/**
 * @brief Allocate the next inbound queue index on the specified manager.
 *
 * @param name Pointer to manager name string.
 *
 * @retval -1 No free inbound queues.
 * @retval >= 0 Allocated inbound queue index.
 */
int allocateNextInbound(QUEUE *queues, char *name, char *obMgrName, char *ibMgrName)
{
    int n;
    int index;
    MANAGER *obmgr;
    MANAGER *ibmgr;

    if ((obmgr = getManagerByName(obMgrName)) == NULL)
    {
        fprintf(stderr, "%d: ERROR: Outbound manager %s not defined in manager configuration file!\n", __LINE__, obMgrName);
        exit(-1);
    }

    if ((ibmgr = getManagerByName(ibMgrName)) == NULL)
    {
        fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__,  queues->ibManager);
        exit(-1);
    }

     if (!UTMgrName ||
       (obmgr->isHardware) ||
       (strcmp(queues->effectiveObManager, CtrlMgrName) == 0) ||
       (strcmp(queues->effectiveObManager, UTMgrName) == 0) ||
       (strcmp(queues->effectiveIbManager, CtrlMgrName) && strcmp(queues->effectiveIbManager, UTMgrName)))
     {
        for (n=0; strlen(managers[n].managerName); n++)
        {
            if (strcmp(name, managers[n].managerName) == 0)
            {
                /*
                 * find a free bit
                 */
                for (index=0; index<QUEUES_PER_NODE; index++)
                {
                    if ((managers[n].inboundQueueMask & (1LL<<index)) == 0)
                    {
                        managers[n].inboundQueueMask |= (1LL<<index);
                        managers[n].usedInbound |= (1LL<<index);
                        if (verbose)
                        {
                            printf("[%s] allocate inbound index %d %s->%s with depth %u\n", name, index, obMgrName, ibMgrName,
                                    queues->depth);
                        }
                        return (index);
                    }
                }
            }
        }
     }
     else
     {
        return getLastInbound(name, obMgrName, ibMgrName);
     }

    return (-1);
}


/**
 * @brief Allocate a specific inbound queue index on the specified manager.
 *
 * @param name Pointer to manager name string.
 * @param index Inbound queue to allocate.
 *
 * @return N/A
 */
void allocateInbound(char *name, int index, char *obMgrName, char *ibMgrName)
{
    int n;

    for (n=0; strlen(managers[n].managerName); n++)
    {
        if (strcmp(name, managers[n].managerName) == 0)
        {
            if (managers[n].inboundQueueMask & (1LL << index))
            {
                printf("[%s] Try to allocate inbound with fixed index %d %s->%s\n", name, index, obMgrName, ibMgrName);
                //assert(0);
            }

            managers[n].inboundQueueMask |= (1LL<<index);
            managers[n].usedInbound |= (1LL<<index);
            if (verbose)
            {
                printf("[%s] allocate inbound with fixed index %d %s->%s\n", name, index, obMgrName, ibMgrName);
            }
            break;
        }
    }
}

/**
 * @brief Allocate a specific outbound queue index on the specified manager.
 *
 * @param name Pointer to manager name string.
 * @param index Outbound queue to allocate.
 *
 * @return N/A
 */
void allocateOutbound(char *name, int index, char *obMgrName, char *ibMgrName)
{
    int n;

    for (n=0; strlen(managers[n].managerName); n++)
    {
        if (strcmp(name, managers[n].managerName) == 0)
        {
            if (managers[n].outboundQueueMask & (1LL << index))
            {
                assert(0);
            }

            managers[n].outboundQueueMask |= (1LL<<index);
            managers[n].usedOutbound |= (1LL<<index);
            if (verbose)
            {
                printf("[%s] allocate outbound with fixed index %d %s->%s\n", name, index, obMgrName, ibMgrName);
            }
            break;
        }
    }
}

int getLastOutbound(char *name, char *obMgrName, char *ibMgrName)
{
    int n;
    int index;

    for (n=0; strlen(managers[n].managerName); n++)
    {
        if (strcmp(name, managers[n].managerName) == 0)
        {
            /*
             * find a free bit
             */
            for (index=0; index<QUEUES_PER_NODE; index++)
            {
                if ((managers[n].outboundQueueMask & (1LL<<index)) == 0)
                {
                    LOG("[%s] getLastOutbound with index %u for %s->%s\n", name, index - 1, obMgrName, ibMgrName);
                    return (index - 1);
                }
            }
        }
    }

    return (-1);
}

/**
 * @brief Allocate the next outbound queue index on the specified manager.
 *
 * @param name Pointer to manager name string.
 *
 * @retval -1 No free outbound queues.
 * @retval >= 0 Allocated outbound queue index.
 */
int allocateNextOutbound(QUEUE* queues, char *name, char *obMgrName, char *ibMgrName)
{
    int n;
    int index;
    MANAGER *ibmgr;
    MANAGER *obmgr;

    if ((obmgr = getManagerByName(obMgrName)) == NULL)
    {
        fprintf(stderr, "%d: ERROR: Outbound manager %s not defined in manager configuration file!\n", __LINE__, obMgrName);
        exit(-1);
    }

    if ((ibmgr = getManagerByName(ibMgrName)) == NULL)
    {
        fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__,  queues->ibManager);
        exit(-1);
    }
     if (!UTMgrName ||
         (ibmgr->isHardware) ||
         (strcmp(queues->effectiveIbManager, CtrlMgrName) == 0) ||
         (strcmp(queues->effectiveIbManager, UTMgrName) == 0) ||
           (strcmp(queues->effectiveObManager, CtrlMgrName) && strcmp(queues->effectiveObManager, UTMgrName)))
     {
        for (n=0; strlen(managers[n].managerName); n++)
        {
            if (strcmp(name, managers[n].managerName) == 0)
            {
                /*
                 * find a free bit
                 */
                for (index=0; index<QUEUES_PER_NODE; index++)
                {
                    if ((managers[n].outboundQueueMask & (1LL<<index)) == 0)
                    {
                        managers[n].outboundQueueMask |= (1LL<<index);
                        managers[n].usedOutbound |= (1LL<<index);
                        if (verbose)
                        {
                            printf("[%s] allocate outbound index %d %s->%s\n", name, index, obMgrName, ibMgrName);
                        }
                        return (index);
                    }
                }
            }
        }
     }
     else
     {
         return getLastOutbound(name, obMgrName, ibMgrName);
     }

    return (-1);
}

/**
 * @brief Obtain a pointer to a queue structure.
 *
 * @param obManager Queue outbound manager name.
 * @param ibManager Queue inbound manager name.
 *
 * @return Pointer to queue structure on success, else NULL.
 */
QUEUE *findQueue(char *obManager, char *ibManager)
{
    int n;
    //    printf("%s: %s->%s\n", __FUNCTION__, obManager, ibManager);
    for (n=0; strlen(queues[n].obManager); n++)
    {
        if ((strcmp(obManager, queues[n].obManager) == 0) && (strcmp(ibManager, queues[n].ibManager) == 0))
            return (&queues[n]);
    }
    return (NULL);
}

int allocateNextMatchedOutbound(char *managerA, char *managerB)
{
    MANAGER *mgrA;
    MANAGER *mgrB;
    int n;

    if (UTMgrName)
    {
        assert(0);
    }

    if ((mgrA = getManagerByName(managerA)) == NULL)
    {
        printf("Could not find manager name %s\n", managerA);
        exit(-1);
    }

    if ((mgrB = getManagerByName(managerB)) == NULL)
    {
        printf("Could not find manager name %s\n", managerB);
        exit(-1);
    }

    for (n=0; n<QUEUES_PER_NODE; n++)
    {
        if (((mgrA->outboundQueueMask & (1LL<<n)) == 0) && ((mgrB->outboundQueueMask & (1LL<<n)) == 0))
        {
            /*
             * Allocate and return.
             */
            mgrA->outboundQueueMask |= (1LL<<n);
            mgrB->outboundQueueMask |= (1LL<<n);
            mgrA->usedOutbound |= (1LL<<n);
            mgrB->usedOutbound |= (1LL<<n);
            return (n);
        }
    }

    return (-1);
}

int allocateNextMatchedInbound(char *managerA, char *managerB)
{
    MANAGER *mgrA;
    MANAGER *mgrB;
    int n;

    if (UTMgrName)
    {
        assert(0);
    }

    if ((mgrA = getManagerByName(managerA)) == NULL)
    {
        printf("Could not find manager name %s\n", managerA);
        exit(-1);
    }

    if ((mgrB = getManagerByName(managerB)) == NULL)
    {
        printf("Could not find manager name %s\n", managerB);
        exit(-1);
    }

    for (n=0; n<QUEUES_PER_NODE; n++)
    {
        if (((mgrA->inboundQueueMask & (1LL<<n)) == 0) && ((mgrB->inboundQueueMask & (1LL<<n)) == 0))
        {
            /*
             * Allocate and return.
             */
            mgrA->inboundQueueMask |= (1LL<<n);
            mgrB->inboundQueueMask |= (1LL<<n);
            mgrA->usedInbound |= (1LL<<n);
            mgrB->usedInbound |= (1LL<<n);
            return (n);
        }
    }

    return (-1);
}

int bitCount(int val)
{
    int count = 0;

    while (val)
    {
        val &= val-1;
        count++;
    }

    return (count);
}

int writeMap(FILE *inFile)
{
    char varName[MAX_VAR_NAME];
    unsigned int addr;
    char buffer[128];
    char queueIndex[128];
    char *endPtr;
    FILE *headerFile;
    MANAGER *obmgr;
    MANAGER *ibmgr;
    int obIndex;
    int ibIndex;
    QUEUE *q;
    int n;
    int a;

    /*
     * Read all the lines of the file into the queue structure array.
     */
    n = 0;
    while (fgets(buffer, sizeof(buffer), inFile))
    {
        if (buffer[0] == '#' || buffer[0] == '\r' || buffer[0] == '\n' || (strncmp(buffer, "//", 2) == 0))
            continue;

        varName[0] = 0;

        if (sscanf(buffer, "%[^ \t] %[^ \t] %[^ \t] %d %d %s", queues[n].name, queues[n].obManager, queues[n].ibManager,
                &queues[n].depth, &queues[n].messageSize, varName) < 5)
            continue;

        if ((sscanf(queues[n].ibManager, "%[^:]:%[a-z0-9]\n", buffer, queueIndex)) == 2)
        {
            ibIndex = strtol(queueIndex, &endPtr, 10);
            if (endPtr == queueIndex)
            {
                //                printf("Could not convert inbound queue ID to decimal, it must be a pseudo queue ID (%s)\n", queueIndex);
                queues[n].inboundIndex = (int) queueIndex[0];
            }
            else
            {
                //                printf("ibindex = %d\n", ibIndex);
                queues[n].inboundIndex = ibIndex;
            }
        }
        else
            queues[n].inboundIndex = -1;

        strcpy(queues[n].ibManager, buffer);

        if ((sscanf(queues[n].obManager, "%[^:]:%[a-z0-9]\n", buffer, queueIndex)) == 2)
        {
            obIndex = strtol(queueIndex, &endPtr, 10);
            if (endPtr == queueIndex)
            {
                //                printf("Could not convert outbound queue ID to decimal, it must be a pseudo queue ID (%s)\n", queueIndex);
                queues[n].outboundIndex = (int) queueIndex[0];
            }
            else
            {
                //                printf("obindex[%d] = %d\n", n, obIndex);
                queues[n].outboundIndex = obIndex;
            }

        }
        else
            queues[n].outboundIndex = -1;

        strcpy(queues[n].obManager, buffer);

        strcpy(queues[n].effectiveObManager, queues[n].obManager);
        if (UTMgrName && strcmp(queues[n].ibManager, UTMgrName) == 0)
        {
            MANAGER *mgr = getManagerByName(queues[n].obManager);
            if (mgr && !mgr->isHardware) {
                strcpy(queues[n].effectiveObManager, CtrlMgrName);
            }
        }

        strcpy(queues[n].effectiveIbManager, queues[n].ibManager);
        if (UTMgrName && strcmp(queues[n].obManager, UTMgrName) == 0)
        {
            MANAGER *mgr = getManagerByName(queues[n].ibManager);
            if (mgr && !mgr->isHardware)
            {
                strcpy(queues[n].effectiveIbManager, CtrlMgrName);
            }
        }

        if ((ibmgr = getManagerByName(queues[n].ibManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__, queues[n].ibManager);
            exit(-1);
        }

        /*
         * Address or variable name?
         */
        addr = (unsigned int) -1;
        sscanf(varName, "0x%x", &addr);

        if (addr == (unsigned int) -1)
        {
            /*
             * Only initialize once.
             */
            if (strlen(ibmgr->varName) == 0)
            {
                //                   if(strncmp(varName, "::", 2) == 0)
                //                       printf("%s is an alias!\n", varName);
                strcpy(ibmgr->varName, varName);
                ibmgr->varOffset = 0;
                strcpy(queues[n].buffer, "");
                //                printf("Initialized %s to %s\n", ibmgr->managerName, ibmgr->varName);
            }
        }
        else
        {
            /*
             * Only initialize once.
             */
            if (ibmgr->address == 0)
            {
                strcpy(ibmgr->varName, "");
                strcpy(queues[n].buffer, "");
                ibmgr->address = addr;
                //                printf("Initialized %s to 0x%08x\n", ibmgr->managerName, ibmgr->address);
            }
        }

        /*
         * Checks to make sure the specified number of outbound buffers does
         * not exceed the maximum defined for the outbound manager.
         */
        if ((obmgr = getManagerByName(queues[n].obManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Outbound manager %s not defined in manager configuration file!\n",__LINE__, queues[n].obManager);
            exit(-1);
        }

        if (queues[n].depth > obmgr->maxOutboundMessageBuffers)
        {
            fprintf(stderr, "%d: ERROR: Maximmum number of outbound message buffers (%d) excedded for %s to %s!\n", __LINE__,
                    obmgr->maxOutboundMessageBuffers, obmgr->managerName, ibmgr->managerName);
            exit(-1);
        }

        n++;
    }

    /*
     * Calculate the size of the inbound message buffer required.
     */
    for (a=0; a<n; a++)
    {
        MANAGER *aliasMgr = NULL;

        //if ((ibmgr = getManagerByName(queues[a].ibManager)) == NULL)
        if ((ibmgr = getManagerByName(queues[a].effectiveIbManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__, queues[a].ibManager);
            exit(-1);
        }

        //printf("byu: allocate buffer address for %s (%s)\n", queues[a].effectiveIbManager, queues[a].ibManager);

        if (strlen(ibmgr->varName) == 0)
        {
            /*
             * Using absolute value.
             */
            //printf("byu:Allocate buffer from %s at offset 0x%x\n", queues[a].effectiveIbManager, ibmgr->address);
            sprintf(queues[a].buffer, "(void *) 0x%08x", ibmgr->address);
            ibmgr->address += (queues[a].messageSize * queues[a].depth);
        }
        else
        {
            /*
             * Using variable name.
             */
            if (strncmp(ibmgr->varName, "::", 2) == 0)
            {
                ASSERT(0);
                if ((aliasMgr = getManagerByName(&ibmgr->varName[2])) == NULL)
                {
                    fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__, &ibmgr->varName[2]);
                    exit(-1);
                }
                if (strlen(aliasMgr->varName) == 0)
                {
                    sprintf(queues[a].buffer, "(void *) 0x%08x", aliasMgr->address);
                    aliasMgr->address += (queues[a].messageSize * queues[a].depth);
                }
                else
                {
                    sprintf(queues[a].buffer, "%s[0x%x]", aliasMgr->varName, aliasMgr->varOffset);
                    aliasMgr->varOffset += (queues[a].messageSize * queues[a].depth);
                }
            }
            else
            {
                sprintf(queues[a].buffer, "%s[0x%x]", ibmgr->varName, ibmgr->varOffset);
                //printf("byu: Allocate buffer from %s at offset 0x%x\n", queues[a].effectiveIbManager, ibmgr->varOffset);
                ibmgr->varOffset += (queues[a].messageSize * queues[a].depth);
                //ASSERT(0);
            }
        }
    }

    /*
     * Process queues that require matched queue indexes first.
     */
    for (a=0; a<n; a++)
    {
        MANAGER *aliasMgr = NULL;

        if ((obmgr = getManagerByName(queues[a].obManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Outbound manager %s not defined in manager configuration file!\n", __LINE__, queues[a].obManager);
            exit(-1);
        }

        if ((ibmgr = getManagerByName(queues[a].ibManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__, queues[a].ibManager);
            exit(-1);
        }

        /*
         * See if the outbound indexes need to match between the managers.
         */
        if (obmgr->matchOutbound || ibmgr->matchOutbound)
        {
            if (UTMgrName)
            {
                assert(0);
            }
            /*
             * Locate the queue in the opposite direction to this one.
             */
            if ((q = findQueue(queues[a].ibManager, queues[a].obManager)) == NULL)
            {
                fprintf(stderr, "%d: ERROR: Could not find queue in opposite direction for %s->%s!\n", __LINE__, queues[a].ibManager,
                        queues[a].obManager);
                exit(-1);
            }

            /*
             * If the queue we just found already has an outbound index
             * so do we, if it doesnt allocate a matched one for both queues.
             */
            if (q->outboundIndex == -1)
            {
                if ((obIndex = allocateNextMatchedOutbound(q->obManager, queues[a].obManager)) == -1)
                {
                    fprintf(stderr, "%d: ERROR: Could not allocate matched outbound index for %s->%s\n", __LINE__, queues[a].obManager,
                            q->obManager);
                    exit(-1);
                }
                q->outboundIndex = obIndex;
                queues[a].outboundIndex = obIndex;
            }

            /*
             * If the queue we just found already has an inbound index
             * so do we, if it doesnt allocate a matched one for both queues.
             */
            if (q->inboundIndex == -1)
            {
                if ((ibIndex = allocateNextMatchedInbound(q->effectiveIbManager, queues[a].ibManager)) == -1)
                {
                    fprintf(stderr, "%d: ERROR: Could not allocate matched outbound index for %s->%s\n", __LINE__, queues[a].obManager,
                            q->obManager);
                    exit(-1);
                }
                q->inboundIndex = ibIndex;
                queues[a].inboundIndex = ibIndex;

                if ((ibmgr->maxInboundMessageBuffers[q->inboundIndex] -= queues[a].depth) < 0)
                {
                    fprintf(stderr, "%d: ERROR: Inbound queue %d for %s is oversubscribed!\n", __LINE__, q->inboundIndex,
                            aliasMgr ? aliasMgr->managerName : ibmgr->managerName);
                    exit(-1);
                }
            }
        }
    }

    /*
     * Process the queues with specific inbound/outbound indexes.
     */
    for (a=0; a<n; a++)
    {
        MANAGER *aliasMgr = NULL;
        q = NULL;

        if ((obmgr = getManagerByName(queues[a].obManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Outbound manager %s not defined in manager configuration file!\n", __LINE__, queues[a].obManager);
            exit(-1);
        }

        if ((ibmgr = getManagerByName(queues[a].ibManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__,  queues[a].ibManager);
            exit(-1);
        }

        if (queues[a].inboundIndex == -1)
        {
        }
        else if (queues[a].inboundIndex >= 'a' && queues[a].inboundIndex <= 'z')
        {
        }
        else
        {
         if (!UTMgrName ||
           (obmgr->isHardware) ||
           (strcmp(queues[a].effectiveObManager, CtrlMgrName) == 0) ||
           (strcmp(queues[a].effectiveObManager, UTMgrName) == 0) ||
           (strcmp(queues[a].effectiveObManager, "PCIE_MGR") == 0) ||
           (strcmp(queues[a].effectiveIbManager, CtrlMgrName) && strcmp(queues[a].effectiveIbManager, UTMgrName)))
         {
            allocateInbound(queues[a].effectiveIbManager, queues[a].inboundIndex, queues[a].obManager, queues[a].ibManager);

            if ((ibmgr->maxInboundMessageBuffers[queues[a].inboundIndex] -= queues[a].depth) < 0)
            {
                fprintf(stderr, "%d: ERROR: Inbound queue %d for %s is oversubscribed!\n", __LINE__, queues[a].inboundIndex,
                        aliasMgr ? aliasMgr->managerName : ibmgr->managerName);
                exit(-1);
            }
         }
         else
         {
             LOG("[%s] Skip the allocate for fixed inbound index %u for %s->%s\n", queues[a].effectiveIbManager, queues[a].inboundIndex, queues[a].obManager, queues[a].ibManager);
         }
        }

        if (queues[a].outboundIndex == -1)
        {
        }
        else if (queues[a].outboundIndex >= 'a' && queues[a].outboundIndex <= 'z')
        {
        }
        else
        {
         if (!UTMgrName ||
           (ibmgr->isHardware) ||
           (strcmp(queues[a].effectiveIbManager, CtrlMgrName) == 0) ||
           (strcmp(queues[a].effectiveIbManager, UTMgrName) == 0) ||
           (strcmp(queues[a].effectiveObManager, CtrlMgrName) && strcmp(queues[a].effectiveObManager, UTMgrName))
           )
         {
            allocateOutbound(queues[a].effectiveObManager, queues[a].outboundIndex, queues[a].obManager, queues[a].ibManager);
         }
         else
         {
             LOG("[%s] Skip the allocate for fixed outbound index %u for %s->%s\n", queues[a].effectiveIbManager, queues[a].inboundIndex, queues[a].obManager, queues[a].ibManager);
         }
        }
    }
    /*
     * Process all of the other queues.
     */
    for (a=0; a<n; a++)
    {
        MANAGER *aliasMgr = NULL;
        q = NULL;

        if ((obmgr = getManagerByName(queues[a].obManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Outbound manager %s not defined in manager configuration file!\n", __LINE__, queues[a].obManager);
            exit(-1);
        }

        if ((ibmgr = getManagerByName(queues[a].ibManager)) == NULL)
        {
            fprintf(stderr, "%d: ERROR: Inbound manager %s not defined in manager configuration file!\n", __LINE__,  queues[a].ibManager);
            exit(-1);
        }

        if (queues[a].inboundIndex == -1)
        {
         if (!UTMgrName ||
           (obmgr->isHardware) ||
           (strcmp(queues[a].effectiveObManager, CtrlMgrName) == 0) ||
           (strcmp(queues[a].effectiveObManager, UTMgrName) == 0) ||
           (strcmp(queues[a].effectiveIbManager, CtrlMgrName) && strcmp(queues[a].effectiveIbManager, UTMgrName)))
         {
          if ((queues[a].inboundIndex = allocateNextInbound(&queues[a], queues[a].effectiveIbManager, queues[a].obManager, queues[a].ibManager)) == -1)
          {
            fprintf(stderr, "%d: ERROR: No free inbound IPC ports for %s->%s!\n", __LINE__, queues[a].obManager, queues[a].ibManager);
            exit(-1);
          }

          if ((!UTMgrName || (strcmp(queues[a].effectiveIbManager, CtrlMgrName) == 0) || (strcmp(queues[a].effectiveIbManager, UTMgrName) == 0)) &&
                  ((ibmgr->maxInboundMessageBuffers[queues[a].inboundIndex] -= queues[a].depth) < 0))
          {
            fprintf(stderr, "%d: ERROR: Inbound queue %d for %s is oversubscribed!(maxInboundMessageBuffers is %d)\n", __LINE__, queues[a].inboundIndex,
                aliasMgr ? aliasMgr->managerName : ibmgr->managerName,
                ibmgr->maxInboundMessageBuffers[queues[a].inboundIndex]);
            exit(-1);
          }
        }
         else
         {
             // here caused the bug that IB_LOOKUP_MGR02CTRL_MGR_IDX same with IB_GC_MGR02CTRL_MGR_IDX
             queues[a].inboundIndex = getLastInbound(queues[a].effectiveIbManager, queues[a].obManager, queues[a].ibManager);
         }
        }
        else if (queues[a].inboundIndex >= 'a' && queues[a].inboundIndex <= 'z')
        {
            if (ibmgr->pseudoQueueMap[queues[a].inboundIndex - 'a'] < 0)
            {
                if ((ibmgr->pseudoQueueMap[queues[a].inboundIndex - 'a'] = allocateNextInbound(&queues[a], queues[a].effectiveIbManager, queues[a].obManager, queues[a].ibManager)) == -1)
                {
                    fprintf(stderr, "%d: ERROR: No free inbound IPC ports for %s->%s!\n", __LINE__, queues[a].obManager, ibmgr->managerName);
                    exit(-1);
                }
            }
            queues[a].inboundIndex = ibmgr->pseudoQueueMap[queues[a].inboundIndex - 'a'];

            if ((ibmgr->maxInboundMessageBuffers[queues[a].inboundIndex] -= queues[a].depth) < 0)
            {
                fprintf(stderr, "%d: ERROR: Inbound queue %d for %s is oversubscribed!\n", __LINE__, queues[a].inboundIndex,
                        aliasMgr ? aliasMgr->managerName : ibmgr->managerName);
                exit(-1);
            }
        }


        if (queues[a].outboundIndex == -1)
        {
         if (!UTMgrName ||
             (ibmgr->isHardware) ||
             (strcmp(queues[a].effectiveIbManager, CtrlMgrName) == 0) ||
             (strcmp(queues[a].effectiveIbManager, UTMgrName) == 0) ||
               (strcmp(queues[a].effectiveObManager, CtrlMgrName) && strcmp(queues[a].effectiveObManager, UTMgrName)))
         {
          if ((queues[a].outboundIndex = allocateNextOutbound(&queues[a], queues[a].effectiveObManager, queues[a].obManager, queues[a].ibManager)) == -1)
          {
            fprintf(stderr, "%d: ERROR: No free outbound IPC ports for %s->%s!\n",__LINE__, queues[a].obManager, ibmgr->managerName);
            exit(-1);
          }
        }
         else
         {
             queues[a].outboundIndex = getLastOutbound(queues[a].effectiveIbManager, queues[a].obManager, queues[a].ibManager);
         }
        }
        else if (queues[a].outboundIndex >= 'a' && queues[a].outboundIndex <= 'z')
        {
            if (obmgr->pseudoQueueMap[queues[a].outboundIndex - 'a'] < 0)
            {
                if ((obmgr->pseudoQueueMap[queues[a].outboundIndex - 'a'] = allocateNextOutbound(&queues[a], queues[a].effectiveObManager, queues[a].obManager, queues[a].ibManager)) == -1)
                {
                    fprintf(stderr, "%d: ERROR: No free outbound IPC ports for %s->%s!\n", __LINE__, obmgr->managerName, queues[a].ibManager);
                    exit(-1);
                }
            }
            queues[a].outboundIndex = obmgr->pseudoQueueMap[queues[a].outboundIndex - 'a'];
        }
    }

    if ((headerFile = fopen("ipcmap.h", "w")) == NULL)
    {
        fprintf(stderr, "ERROR: Could not open ipcmap.h!\n");
        exit(-1);
    }

    displayHeader(headerFile);

    fprintf(headerFile, "#ifndef IPC_MAP_H\n");
    fprintf(headerFile, "#define IPC_MAP_H\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "#include <nodeid.h>\n");
    fprintf(headerFile, "#include <ipc.h>\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "#ifdef __cplusplus\n");
    fprintf(headerFile, "extern          \"C\" {\n");
    fprintf(headerFile, "#endif\n");
    fprintf(headerFile, "extern const ipc_config_t map[];\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "/**\n");
    fprintf(headerFile, " * @file\n");
    fprintf(headerFile, " * @brief IPC Message queue #defines.\n");
    fprintf(headerFile, " * \n");
    fprintf(headerFile, " * The contents of this file are auto-generated by the IPCMapGenerator. It should not be modified\n");
    fprintf(headerFile, " * by hand.\n");
    fprintf(headerFile, " */\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "/**\n");
    fprintf(headerFile, " * @brief Index of the memory network inbound IPC message queue.\n");
    fprintf(headerFile, " */\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "#define IB_DATANETWORK_IDX            0\n");
    fprintf(headerFile, "\n");

    fprintf(headerFile, "\n");
    fprintf(headerFile, "/*\n");
    fprintf(headerFile, " * Inbound and outbound queue indexes\n");
    fprintf(headerFile, " */\n");
    for (a=0; a<n; a++)
    {
        if (queues[a].name[0] != '*')
        {
            sprintf(buffer, "#define IB_%s2%s_%s_IDX", queues[a].obManager, queues[a].ibManager, queues[a].name);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].inboundIndex);

            sprintf(buffer, "#define OB_%s2%s_%s_IDX", queues[a].obManager, queues[a].ibManager, queues[a].name);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].outboundIndex);
        }
        else
        {
            sprintf(buffer, "#define IB_%s2%s_IDX", queues[a].obManager, queues[a].ibManager);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].inboundIndex);
            fprintf(stdout, "%-35s    %d\n", buffer, queues[a].inboundIndex);

            sprintf(buffer, "#define OB_%s2%s_IDX", queues[a].obManager, queues[a].ibManager);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].outboundIndex);
            fprintf(stdout, "%-35s    %d\n", buffer, queues[a].outboundIndex);
        }
    }

    fprintf(headerFile, "\n");
    fprintf(headerFile, "/*\n");
    fprintf(headerFile, " * Queue depths and message sizes\n");
    fprintf(headerFile, " */\n");
    for (a=0; a<n; a++)
    {
        if (queues[a].name[0] != '*')
        {
            sprintf(buffer, "#define %s2%s_%s_DEPTH", queues[a].obManager, queues[a].ibManager, queues[a].name);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].depth);

            sprintf(buffer, "#define %s2%s_%s_MSG_SIZE", queues[a].obManager, queues[a].ibManager, queues[a].name);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].messageSize);
        }
        else
        {
            sprintf(buffer, "#define %s2%s_DEPTH", queues[a].obManager, queues[a].ibManager);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].depth);

            sprintf(buffer, "#define %s2%s_MSG_SIZE", queues[a].obManager, queues[a].ibManager);
            fprintf(headerFile, "%-35s    %d\n", buffer, queues[a].messageSize);
        }
    }
    fprintf(headerFile, "\n");
    fprintf(headerFile, "/*\n");
    fprintf(headerFile, " * Maximum number of inbound and outbound message\n");
    fprintf(headerFile, " * queues on each node.\n");
    fprintf(headerFile, " */\n");
    for (a=0; strlen(managers[a].managerName); a++)
    {
        sprintf(buffer, "#define %s_MAX_OB_IDX", managers[a].managerName);
        fprintf(headerFile, "%-35s    %d\n", buffer, bitCount(managers[a].usedOutbound));
        sprintf(buffer, "#define %s_MAX_IB_IDX", managers[a].managerName);
        fprintf(headerFile, "%-35s    %d\n", buffer, bitCount(managers[a].usedInbound));
    }

    fprintf(headerFile, "\n");
    fprintf(headerFile, "/*\n");
    fprintf(headerFile, " * Size of message memory (in bytes) required by each node.\n");
    fprintf(headerFile, " */\n");

    for (a=0; strlen(managers[a].managerName); a++)
    {
        sprintf(buffer, "#define %s_MAX_MSG", managers[a].managerName);
        if (strlen(managers[a].varName))
            fprintf(headerFile, "%-35s    0x%x\n", buffer, managers[a].varOffset);
        else
            fprintf(headerFile, "%-35s    0x%x\n", buffer, managers[a].address & 0x1ffff);
    }

    fprintf(headerFile, "\n");
    fprintf(headerFile, "/*\n");
    fprintf(headerFile, " * Bit masks used to enable outbound/inbound interrupts\n");
    fprintf(headerFile, " */\n");
    for (a=0; strlen(managers[a].managerName); a++)
    {
        sprintf(buffer, "#define %s_IB_INT_ENABLE_MASK", managers[a].managerName);
        fprintf(headerFile, "%-35s    0x%08x\n", buffer, ~((u32)managers[a].usedInbound));
        sprintf(buffer, "#define %s_OB_INT_ENABLE_MASK", managers[a].managerName);
        fprintf(headerFile, "%-35s    0x%08x\n", buffer, ~((u32)managers[a].usedOutbound));
    }

    fprintf(headerFile, "\n");
    fprintf(headerFile, "/*\n");
    fprintf(headerFile, " * Destination node type, hardware functional block or CPU core\n");
    fprintf(headerFile, " */\n");
    fprintf(headerFile, "#define IS_CPU    0\n");
    fprintf(headerFile, "#define OB_IS_HW    1\n");
    fprintf(headerFile, "#define IB_IS_HW    2\n");
    fprintf(headerFile, "#define IS_HW_NO_MSGQCTL    3\n");
    fprintf(headerFile, "#define IB_OB_ARE_HW    4\n");

    printf(" * @note Auto-generated, modify at your own risk!\n");
    printf(" */\n");
    printf("const ipc_config_t %s[] = {\n", mapName);

    for (a=0; a<n; a++)
    {
        MANAGER *ibMgr, *obMgr;
        ibMgr = getManagerByName(queues[a].ibManager);
        obMgr = getManagerByName(queues[a].obManager);

        ASSERT(ibMgr);
        ASSERT(obMgr);

        if (!UTMgrName ||
                (ibMgr->isHardware && (strcmp(queues[a].effectiveObManager, CtrlMgrName) == 0 || strcmp(queues[a].effectiveObManager, UTMgrName) == 0)) ||
                (obMgr->isHardware && (strcmp(queues[a].effectiveIbManager, CtrlMgrName) == 0 || strcmp(queues[a].effectiveIbManager, UTMgrName) == 0)) ||
                (strcmp(queues[a].effectiveIbManager, CtrlMgrName) == 0 && strcmp(queues[a].effectiveObManager, UTMgrName) == 0) ||
                (strcmp(queues[a].effectiveObManager, CtrlMgrName) == 0 && strcmp(queues[a].effectiveIbManager, UTMgrName) == 0))
        {
            if ((getManagerByName(queues[a].ibManager)->isHardware == IS_HW_MSGQCTRL) &&
                (getManagerByName(queues[a].obManager)->isHardware == IS_HW_MSGQCTRL))
                sprintf(buffer, "{ IB_OB_ARE_HW, ");
            else if (getManagerByName(queues[a].ibManager)->isHardware == IS_HW_MSGQCTRL)
                sprintf(buffer, "{ IB_IS_HW, ");
            else if (getManagerByName(queues[a].obManager)->isHardware == IS_HW_MSGQCTRL)
                sprintf(buffer, "{ OB_IS_HW, ");
            else if (getManagerByName(queues[a].obManager)->isHardware == IS_HW_NO_MSGQCTRL)
                sprintf(buffer, "{ IS_HW_NO_MSGQCTL, ");
            else if (getManagerByName(queues[a].ibManager)->isHardware == IS_HW_NO_MSGQCTRL)
                sprintf(buffer, "{ IS_HW_NO_MSGQCTL, ");
            else
                sprintf(buffer, "{ IS_CPU, ");
            printf("%-20s", buffer);

            if (queues[a].name[0] != '*')
                sprintf(buffer, "%s2%s_%s_DEPTH,", queues[a].obManager, queues[a].ibManager, queues[a].name);
            else
                sprintf(buffer, "%s2%s_DEPTH,", queues[a].obManager, queues[a].ibManager);
            printf("%-20s", buffer);

            //sprintf(buffer, "%s_NODEID,", queues[a].obManager);
            sprintf(buffer, "%s_NODEID,", queues[a].effectiveObManager);
            printf("%-20s", buffer);

            if (queues[a].name[0] != '*')
                sprintf(buffer, "OB_%s2%s_%s_IDX,", queues[a].obManager, queues[a].ibManager, queues[a].name);
            else
                sprintf(buffer, "OB_%s2%s_IDX,", queues[a].obManager, queues[a].ibManager);
            printf("%-20s", buffer);

            //sprintf(buffer, "%s_NODEID,", queues[a].ibManager);
            sprintf(buffer, "%s_NODEID,", queues[a].effectiveIbManager);
            printf("%-20s", buffer);

            if (queues[a].name[0] != '*')
                sprintf(buffer, "IB_%s2%s_%s_IDX,", queues[a].obManager, queues[a].ibManager, queues[a].name);
            else
                sprintf(buffer, "IB_%s2%s_IDX,", queues[a].obManager, queues[a].ibManager);
            printf("%-20s", buffer);

            sprintf(buffer, "%s,", queues[a].buffer);
            printf("%-20s", buffer);

            if (queues[a].name[0] != '*')
                sprintf(buffer, "%s2%s_%s_MSG_SIZE", queues[a].obManager, queues[a].ibManager, queues[a].name);
            else
                sprintf(buffer, "%s2%s_MSG_SIZE", queues[a].obManager, queues[a].ibManager);
            printf("%-20s", buffer);

            printf(" },\n");
        }
    }

    printf("\n");
    printf("// End of the IPC map\n");
    printf("{ 0, 0, 0x00, 0x00, 0, 0, 0x00000000, 0 }\n");
    printf("};\n");

    fprintf(headerFile, "\n");
    fprintf(headerFile, "#ifdef __cplusplus\n");
    fprintf(headerFile, "}\n");
    fprintf(headerFile, "#endif\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "#endif\n");

    fclose(headerFile);
    return (0);
}

int loadManagerNames(char *fileName)
{
    FILE *inFile;
    char line[128];
    int n;
    int a;
    int maxInboundMessageBuffers;

    if ((inFile = fopen(fileName, "r")) == NULL)
    {
        fprintf(stderr, "ERROR: Could not open manager config file %s!\n", fileName);
        exit(-1);
    }

    memset(managers, 0, sizeof(managers));

    displayHeader(stdout);

    printf("#include <ipc.h>\n");
    printf("#include <ipcmap.h>\n");
    printf("#include <nodeid.h>\n\n");

    printf("/**\n");
    printf(" * @brief Message buffer memory.\n");
    printf(" * This variable should be declared locally by each manager.\n");
    printf(" */\n");
    printf("extern char messageBuffer[];\n\n");

    printf("/**\n");
    printf(" * @brief IPC configuration map.\n");
    printf(" *\n");
    printf(" * This table was automatically generated using %s and %s.\n", managerFileName, configFileName);
    printf(" *\n");

    n = 0;
    while (fgets(line, sizeof(line), inFile))
    {
        if (line[0] == '#' || line[0] == '\r' || line[0] == '\n' || (strncmp(line, "//", 2) == 0))
            continue;

        managers[n].isHardware = 0;
        managers[n].matchOutbound = 0;
#if BITS_PER_LONG == 64
        sscanf(line, "%s %d %d 0x%lx 0x%lx %d %d", managers[n].managerName, &managers[n].isHardware, &managers[n].matchOutbound, &managers[n].outboundQueueMask, &managers[n].inboundQueueMask, &managers[n].maxOutboundMessageBuffers, &maxInboundMessageBuffers);
        //        printf("0x%lx\n", managers[n].outboundQueueMask);
#else
        sscanf(line, "%s %d %d 0x%llx 0x%llx %d %d", managers[n].managerName, &managers[n].isHardware, &managers[n].matchOutbound,
                &managers[n].outboundQueueMask, &managers[n].inboundQueueMask, &managers[n].maxOutboundMessageBuffers,
                &maxInboundMessageBuffers);
#endif
        if (managers[n].managerName[0] == '#')
            continue;

        if (managers[n].matchOutbound)
            printf(" * - %s outbound and inbound port ID's will be matched\n", managers[n].managerName);

        for (a=0; a<26; a++)
            managers[n].pseudoQueueMap[a] = -1;

        /*
         * Initialize the total number of inbound message buffer slots for each manager.
         */
        for (a=0; a<QUEUES_PER_NODE; a++)
            managers[n].maxInboundMessageBuffers[a] = maxInboundMessageBuffers;

        //
        // If not hardware, inbound queue 0 is the memory
        // network completion queue. Mark it as used.
        //
        if (!managers[n].isHardware)
            managers[n].usedInbound |=1;

        n++;
    }
    printf(" *\n");

    fclose(inFile);
    return (0);
}

int main(int argc, char *argv[])
{
    FILE *inFile;
    int rv = 0;
    int n;

    memset(queues, 0, sizeof(queues));
    memset(managers, 0, sizeof(managers));

    for (n=1; n<argc; n++)
    {
        if (strcmp(argv[n], "-u") == 0)
        {
            n++;
            UTMgrName = argv[n];
        }
        else if(strcmp(argv[n], "-v") == 0)
        {
           verbose = 1;
           n++;
        }
        else if (strcmp(argv[n], "-m") == 0)
        {
            n++;
            managerFileName = argv[n];
        }
        else if (strcmp(argv[n], "-c") == 0)
        {
            n++;
            configFileName = argv[n];
        }
        else if (strcmp(argv[n], "-n") == 0)
        {
            n++;
            mapName = argv[n];
        }
        else if (strcmp(argv[n], "-h") == 0)
        {
            printf("Usage:\n");
            printf("%s [-m manager_config_file_name] [-c IPC_config_file_name] [-n map_variable_name]\n", argv[0]);
            return (0);
        }
    }

    if (loadManagerNames(managerFileName) < 0)
        return (-1);

    if ((inFile = fopen(configFileName, "r")) == NULL)
    {
        fprintf(stderr, "ERROR: Could not open IPC config file %s!\n", configFileName);
        return (-1);
    }

    if (writeMap(inFile) < 0)
        rv = -1;

    fclose(inFile);

    system("pause");

    return (rv);
}
