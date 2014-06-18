/* File: guidanceSystem.c
 * Description: Bot guidance system file
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #include <stdio.h>
 #include <prjCommon.h>
 #include <hal/motor.h>
 #include <guidanceSystem.h>
 #include <whiteLineFollower.h>
 #include <assert.h>
 #include <string.h>

 /* Globals */
 
 static BotLocation thisBotLocation;
 static BotOrientation thisBotOrientation;

 void printMap(Map *pMap);
 void printPath(Path *p);
  
 static UINT intSqrt(ULINT x) {
    register ULINT op, res, one;

    op = x;
    res = 0ul;

    /* "one" starts at the highest power of four <= than the argument. */
    one = 1ul << 30;  /* second-to-top bit set */
    while (one > op) one >>= 2;

    while (one != 0ul) {
        if (op >= res + one) {
            op -= res + one;
            res += one << 1;  // <-- faster than 2 * one
        }
        res >>= 1;
        one >>= 2;
    }
    return (UINT)res;
 }
 
 static UINT getEuclideanDistance(UINT x1, UINT y1, UINT x2, UINT y2) {
    ULINT magX, magY;
    
    if(x1 > x2) {
        magX = (ULINT)(x1 - x2);
    }
    else {
        magX = (ULINT)(x2 - x1);
    }

    if(y1 > y2) {
        magY = (ULINT)(y1 - y2);
    }
    else {
        magY = (ULINT)(y2 - y1);
    }
    
    return intSqrt(magX*magX + magY*magY);
 }
 
 static STATUS getPositionMetaInfo(Map *pMap, UINT posX, UINT posY,
                                    PositionMetaInfo *pInfo) {
    UINT idx, idx2;
                                    
    ASSERT(pMap != NULL);
    ASSERT(pInfo != NULL);
                   
    /* Check whether given position is AT_NODE */
    for(idx = 0; idx < pMap->nodeCount; idx ++) {
        if((pMap->nodeList[idx].posX == posX) && 
            (pMap->nodeList[idx].posY == posY)) {
            pInfo->loc = AT_NODE;
            pInfo->nodeA = pInfo->nodeB = idx;
            return STATUS_OK;
        }
    }
    
    /* Otherwise check all edges in the graph */
    for(idx = 1; idx < pMap->nodeCount; idx ++) {
        for(idx2 = 0; idx2 < idx; idx2 ++) {
            /* Checking only lower triangle in weightMatrix (since symmetric) */
            if(pMap->weightMatrix[idx][idx2] < INFINITY) {
                if(pMap->nodeList[idx].posY == pMap->nodeList[idx2].posY) {
                    /* Edge is horizontal */
                    /* Check given point is on this edge */
                    if((pMap->nodeList[idx].posY == posY) && 
                       (
                        ((pMap->nodeList[idx].posX < posX) && (posX < pMap->nodeList[idx2].posX))
                        ||
                        ((pMap->nodeList[idx2].posX < posX) && (posX < pMap->nodeList[idx].posX))
                       )
                      ) {
                        pInfo->loc = ON_EDGE;
                        pInfo->nodeA = idx;
                        pInfo->nodeB = idx2;
                        return STATUS_OK;
                    }
                }
                if(pMap->nodeList[idx].posX == pMap->nodeList[idx2].posX) {
                    /* Edge is vertical */
                    /* Check given point is on this edge */
                    if((pMap->nodeList[idx].posX == posX) && 
                       (
                        ((pMap->nodeList[idx].posY < posY) && (posY < pMap->nodeList[idx2].posY))
                        ||
                        ((pMap->nodeList[idx2].posY < posY) && (posY < pMap->nodeList[idx].posY))
                       )
                      ) {
                        pInfo->loc = ON_EDGE;
                        pInfo->nodeA = idx;
                        pInfo->nodeB = idx2;
                        return STATUS_OK;
                    }
                }
            }
        }
    }
    
    /* Not found on edge */
    pInfo->loc = OUTSIDE_EDGE;
    pInfo->nodeA = pInfo->nodeB = 0;    /* nodes are irrelevant for this case */

    return STATUS_OK;
 }
 
 static void floydWarshall(Map *pMap) {
	UINT k, i, j;
	UINT d_temp[MAX_NODES][MAX_NODES], p_temp[MAX_NODES][MAX_NODES];

    ASSERT(pMap != NULL);
    
	/* Initialize path matrix */
	for (i = 0; i < pMap->nodeCount; i++) {
		for (j = 0; j < pMap->nodeCount; j++) {
            if((i == j) || (pMap->distanceMatrix[i][j] == INFINITY))
                pMap->pathMatrix[i][j] = 0;
            else
                pMap->pathMatrix[i][j] = i;
		}
	}

	for (k = 0; k < pMap->nodeCount; k++) {
		for (i = 0; i < pMap->nodeCount; i++) {
			for (j = 0; j < pMap->nodeCount; j++) {
				d_temp[i][j] =
				    MIN(pMap->distanceMatrix[i][j],
					pMap->distanceMatrix[i][k] + pMap->distanceMatrix[k][j]);
				if(pMap->distanceMatrix[i][j] <= pMap->distanceMatrix[i][k] + pMap->distanceMatrix[k][j]) {
				    p_temp[i][j] = pMap->pathMatrix[i][j];
				}
				else {
				    p_temp[i][j] = pMap->pathMatrix[k][j];
				}				    
			}
		}

		for (i = 0; i < pMap->nodeCount; i++) {
			for (j = 0; j < pMap->nodeCount; j++) {
				pMap->distanceMatrix[i][j] = d_temp[i][j];
				pMap->pathMatrix[i][j] = p_temp[i][j];
			}
		}
	}
 }

 static STATUS loadMap(FILE *fp, Map *map) {
    UINT idx, idx2;
	UINT nodeA, nodeB;
	char ch;
	
	ASSERT(fp != NULL);
	ASSERT(map != NULL);

	//printf("Loading map from file...\n");

	/* No. of nodes in the graph */

	fscanf(fp, "%d", &map->nodeCount);    
    ASSERT(map->nodeCount < MAX_NODES);

    //printf("Map Nodes: %d\n", map->nodeCount);
    //printf("\nNode List:\n");
	
	/* Read node list with their co-ordinate information */    

	for(idx = 0; idx < map->nodeCount; idx++) {
		fscanf(fp, "%d %d %c", &map->nodeList[idx].posX, &map->nodeList[idx].posY, &ch);
	    if(ch == 'c' || ch == 'C')
	        map->nodeList[idx].isCheckpoint = 1;
	    else
    	    map->nodeList[idx].isCheckpoint = 0;
        
        if(map->nodeList[idx].isCheckpoint != 0) {
            //printf("Node[%u]: (%u, %u) C\n", idx, map->nodeList[idx].posX, map->nodeList[idx].posY);
        }
        else {
            //printf("Node[%u]: (%u, %u) N\n", idx, map->nodeList[idx].posX, map->nodeList[idx].posY);
        }
	}

	/* Initialize weightMatrix */
    for(idx = 0; idx < map->nodeCount; idx ++) {
        for(idx2 = 0; idx2 < map->nodeCount; idx2 ++) {
            map->weightMatrix[idx][idx2] = INFINITY;
        }
    }
    
    //printf("\nEdge List:\n");

	/* Read edges from the map and calculate their weights using co-ordinates */
    fscanf(fp, "%d %d", &nodeA, &nodeB);

    while (!(nodeA == 0 && nodeB == 0)) {
		/* nodeA = 0, nodeB = 0 is the end marker entry in map file */
		
		ASSERT(nodeA >= 0);
		ASSERT(nodeB >= 0);
		ASSERT(nodeA < MAX_NODES);
		ASSERT(nodeB < MAX_NODES);

        /* Fill edge weights as Euclidean distances */
        map->weightMatrix[nodeA][nodeB] = map->weightMatrix[nodeB][nodeA] = 
            getEuclideanDistance(map->nodeList[nodeA].posX, map->nodeList[nodeA].posY,
                                map->nodeList[nodeB].posX, map->nodeList[nodeB].posY);

        /* Read next edge */
        fscanf(fp, "%d %d", &nodeA, &nodeB);

        //printf("%d <---> %d\n", nodeA, nodeB);
	}

    for(idx = 0; idx < map->nodeCount; idx ++) {
        for(idx2 = 0; idx2 < map->nodeCount; idx2 ++) {
            map->distanceMatrix[idx][idx2] = map->weightMatrix[idx][idx2];
        }
    }
    
    //printf("Map loaded successfully!\n");

    //printf("Computing all shortest paths over the map...\n");
    floydWarshall(map);
    //printf("Finished computing all shortest paths.\n");
    
	return STATUS_OK;
 }

static UINT getNearestNode(Map *pMap, UINT x, UINT y){
    UINT minDist, minIdx;
    UINT idx, dist;
    
    ASSERT(pMap != NULL);
    
    minDist = INFINITY;
    minIdx = 0;
    
    for(idx = 0; idx < pMap->nodeCount; idx ++) {

        // Find Euclidean distance
        dist = getEuclideanDistance(x, y, pMap->nodeList[idx].posX, 
            pMap->nodeList[idx].posY);

        if(dist < minDist) {
            minDist = dist;
            minIdx = idx;
        }
    }
    
    return minIdx;
}
 
 STATUS initBotGuidanceSystem(FILE *fp, Map *pMap) {
 	
	ASSERT(fp != NULL); 
    ASSERT(pMap != NULL);
    
	/* Initialize current location */
	
	thisBotLocation.posX = thisBotLocation.posY = 0;
	thisBotLocation.errorX = thisBotLocation.errorY = EPSILON;
	thisBotOrientation = EASTWARD;
	
	/* Load map */
	
	if(loadMap(fp, pMap) != STATUS_OK) {
		/* Error loading map */
		return !STATUS_OK;
	}
	
	return STATUS_OK;
 }
 
 /***** Debug routines *****/
 void printPath(Path *p) {
    UINT i;
    
    //printf("Path: ");
    for(i = 0; i < p->length; i++){
        //printf("%d, ", p->nodes[i]);
    }
    //printf("%d\n", p->nodes[i]);
    //printf("Length: %u\n", p->length);
    //printf("Distance: %u\n", p->distance);
}

 void printMap(Map *pMap) {
    UINT idx, idx2;	
    
    ASSERT(pMap != NULL);
    
    //printf("nodeCount: %u\n", pMap->nodeCount);
	
    for(idx = 0; idx < pMap->nodeCount; idx ++) {
        //printf("node #%02u: %4u %4u ", idx, pMap->nodeList[idx].posX,
            //pMap->nodeList[idx].posY);		
        if(pMap->nodeList[idx].isCheckpoint == TRUE) {
            //printf("C\n");			
        }
        else {
            //printf("N\n");			
        }
    }
    
    /* Print weight matrix */
    //printf("\nWeight matrix:\n");
	
    for(idx = 0; idx < pMap->nodeCount; idx ++) {
        for(idx2 = 0; idx2 < pMap->nodeCount; idx2 ++) {
            if(pMap->weightMatrix[idx][idx2] >= INFINITY) {
                //printf("INF ");				
            }
            else{
                //printf("%3u ", pMap->weightMatrix[idx][idx2]);				
            }
        }
        //printf("\n");		
    }

    /* Print distance matrix */
    //printf("\nDistance matrix:\n");	
    for(idx = 0; idx < pMap->nodeCount; idx ++) {
        for(idx2 = 0; idx2 < pMap->nodeCount; idx2 ++) {
            if(pMap->distanceMatrix[idx][idx2] >= INFINITY) {
                //printf("INF ");				
            }
            else{
                //printf("%3u ", pMap->distanceMatrix[idx][idx2]);				
            }
        }
        //printf("\n");		
    }

    /* Print path matrix */
    //printf("\nPath matrix:\n");
	for(idx = 0; idx < pMap->nodeCount; idx ++) {
        for(idx2 = 0; idx2 < pMap->nodeCount; idx2 ++) {
            if(pMap->pathMatrix[idx][idx2] >= INFINITY) {
                //printf("INF ");
		    }
            else{
                //printf("%3u ", pMap->pathMatrix[idx][idx2]);				
            }
        }
        //printf("\n");
    }
}

 static int getAngleFromOrientation(BotOrientation or) {
    const int angle[] = {0, 90, 180, 270};
    ASSERT((or == EASTWARD) || (or == NORTHWARD) || (or == WESTWARD) ||
           (or == SOUTHWARD));
    return angle[or];
 }
static Path* copyPath(Path *lval, Path *rval) {
    UINT idx;
    UINT *lptr, *rptr;

    ASSERT(lval != NULL);
    ASSERT(rval != NULL);

    lptr = lval->nodes;
    rptr = rval->nodes;
        
    for(idx = 0; idx < MAX_NODES; idx ++) {
        *lptr = *rptr;
        lptr ++;
        rptr ++;
    }
    lval->length = rval->length;
    lval->distance = rval->distance;
    
    return rval;
}

STATUS getShortestPath(Map *pMap, UINT srcNode, UINT destNode, Path *shortestPath) {
    UINT idx, prevNode;
    UINT computedPath[MAX_NODES];
    UINT *pNode;
    
    ASSERT(pMap != NULL);
    ASSERT(shortestPath != NULL);
    ASSERT(0 <= srcNode);
    ASSERT(srcNode < pMap->nodeCount);
    ASSERT(0 <= destNode);
    ASSERT(destNode < pMap->nodeCount);
    
    idx = 0;
    computedPath[0] = destNode;
    prevNode = destNode;
    while(prevNode != srcNode) {
        idx = idx + 1;
        prevNode = pMap->pathMatrix[srcNode][prevNode];
        computedPath[idx] = prevNode;

        ASSERT(idx < MAX_NODES);
    }
    
    /* computedPath[idx] is source node */
    shortestPath->length = 0;
    shortestPath->distance = 0;
    pNode = shortestPath->nodes;
    
    while(idx > 0) {
        *pNode = computedPath[idx];
        pNode ++;
        shortestPath->length ++;
        shortestPath->distance += pMap->weightMatrix[computedPath[idx]][computedPath[idx-1]];
        idx --;
    }

    ASSERT(idx == 0);
    *pNode = computedPath[0];
    
    return STATUS_OK;
}

STATUS analyzeShortestRoute(Map *pMap, UINT x1, UINT y1, UINT x2, UINT y2, 
    Path *path) {
    Path pathAA, pathAB, pathBA, pathBB;
    PositionMetaInfo info1, info2;
    UINT distAA, distAB, distBA, distBB;
    STATUS ret;

    ASSERT(pMap != NULL);
    ASSERT(path != NULL);
    
    /* Obtain locality meta information */
    ret = getPositionMetaInfo(pMap, x1, y1, &info1);
    ASSERT(ret == STATUS_OK);

    ret = getPositionMetaInfo(pMap, x2, y2, &info2);
    ASSERT(ret == STATUS_OK);
    
    if(info1.loc == OUTSIDE_EDGE || info2.loc == OUTSIDE_EDGE) {
        /* Bot cannot be in outside white line */
        return !STATUS_OK;
    }
    
    /* There are four cases possible with locality of points */    
    /* Case #1: Point1 : AT_NODE, Point2 : AT_NODE */
    /* Case #2: Point1 : AT_NODE, Point2 : ON_EDGE */
    /* Case #3: Point1 : ON_EDGE, Point2 : AT_NODE */
    /* Case #4: Point1 : ON_EDGE, Point2 : ON_EDGE */
    /* Analyse each case */
    
    /* Case #1: Point1 : AT_NODE, Point2 : AT_NODE */
    if(info1.loc == AT_NODE && info2.loc == AT_NODE) {
        return getShortestPath(pMap, info1.nodeA, info2.nodeA, path);
    }
    
    /* Case #2: Point1 : AT_NODE, Point2 : ON_EDGE */
    if(info1.loc == AT_NODE && info2.loc == ON_EDGE) {
        ret = getShortestPath(pMap, info1.nodeA, info2.nodeA, &pathAA);
        ASSERT(ret == STATUS_OK);
        ret = getShortestPath(pMap, info1.nodeA, info2.nodeB, &pathAB);
        ASSERT(ret == STATUS_OK);
        
        distAA = pathAA.distance + getEuclideanDistance(
                            pMap->nodeList[info2.nodeA].posX,
                            pMap->nodeList[info2.nodeA].posY, x2, y2);
                            
        distAB = pathAB.distance + getEuclideanDistance(
                            pMap->nodeList[info2.nodeB].posX,
                            pMap->nodeList[info2.nodeB].posY, x2, y2);
        if(distAA < distAB) {
            copyPath(path, &pathAA);
            return STATUS_OK;
        }
        else{
            copyPath(path, &pathAB);
            return STATUS_OK;
        }
    }
    
    /* Case #3: Point1 : ON_EDGE, Point2 : AT_NODE */    
    if(info1.loc == ON_EDGE && info2.loc == AT_NODE) {
        ret = getShortestPath(pMap, info1.nodeA, info2.nodeA, &pathAA);
        ASSERT(ret == STATUS_OK);
        ret = getShortestPath(pMap, info1.nodeB, info2.nodeA, &pathBA);
        ASSERT(ret == STATUS_OK);
        
        distAA = pathAA.distance + getEuclideanDistance(
                            pMap->nodeList[info1.nodeA].posX,
                            pMap->nodeList[info1.nodeA].posY, x1, y1);
                            
        distBA = pathBA.distance + getEuclideanDistance(
                            pMap->nodeList[info1.nodeB].posX,
                            pMap->nodeList[info1.nodeB].posY, x1, y1);
        if(distAA < distBA) {
            copyPath(path, &pathAA);
            return STATUS_OK;
        }
        else{
            copyPath(path, &pathBA);
            return STATUS_OK;
        }
    }
    
    /* Case #4: Point1 : ON_EDGE, Point2 : ON_EDGE */    
    if(info1.loc == ON_EDGE && info2.loc == ON_EDGE) {
        ret = getShortestPath(pMap, info1.nodeA, info2.nodeA, &pathAA);
        ASSERT(ret == STATUS_OK);
        ret = getShortestPath(pMap, info1.nodeA, info2.nodeB, &pathAB);
        ASSERT(ret == STATUS_OK);
        ret = getShortestPath(pMap, info1.nodeB, info2.nodeA, &pathBA);
        ASSERT(ret == STATUS_OK);
        ret = getShortestPath(pMap, info1.nodeB, info2.nodeB, &pathBB);
        ASSERT(ret == STATUS_OK);
        
        distAA = pathAA.distance
                 + getEuclideanDistance(x1, y1,
                            pMap->nodeList[info1.nodeA].posX,
                            pMap->nodeList[info1.nodeA].posY)
                 + getEuclideanDistance(x2, y2,
                            pMap->nodeList[info2.nodeA].posX,
                            pMap->nodeList[info2.nodeA].posY);
        distAB = pathAB.distance
                 + getEuclideanDistance(x1, y1,
                            pMap->nodeList[info1.nodeA].posX,
                            pMap->nodeList[info1.nodeA].posY)
                 + getEuclideanDistance(x2, y2,
                            pMap->nodeList[info2.nodeB].posX,
                            pMap->nodeList[info2.nodeB].posY);
        distBA = pathBA.distance
                 + getEuclideanDistance(x1, y1,
                            pMap->nodeList[info1.nodeB].posX,
                            pMap->nodeList[info1.nodeB].posY)
                 + getEuclideanDistance(x2, y2,
                            pMap->nodeList[info2.nodeA].posX,
                            pMap->nodeList[info2.nodeA].posY);
        distBB = pathBB.distance
                 + getEuclideanDistance(x1, y1,
                            pMap->nodeList[info1.nodeB].posX,
                            pMap->nodeList[info1.nodeB].posY)
                 + getEuclideanDistance(x2, y2,
                            pMap->nodeList[info2.nodeB].posX,
                            pMap->nodeList[info2.nodeB].posY);
                            
        /* Return minimum one */
        if(distAA <= distAB && distAA <= distBA && distAA <= distBB) {
            /* Is distAA minimum? */
            copyPath(path, &pathAA);
            return STATUS_OK;
        }
        if(distAB <= distAA && distAB <= distBA && distAB <= distBB) {
            /* Is distAB minimum? */
            copyPath(path, &pathAB);
            return STATUS_OK;
        }
        if(distBA <= distAA && distBA <= distAB && distBA <= distBB) {
            /* Is distBA minimum? */
            copyPath(path, &pathBA);
            return STATUS_OK;
        }
        if(distBB <= distAA && distBB <= distAB && distBB <= distBA) {
            /* Is distBB minimum? */
            copyPath(path, &pathBB);
            return STATUS_OK;
        }
    }
    return !STATUS_OK;
}

STATUS computeOrientation(UINT x1, UINT y1, UINT x2, UINT y2, BotOrientation *ort) {
    ASSERT(ort != NULL);
    
    if( (x1 < x2) && (y1 == y2) ) {
        *ort = EASTWARD;
    }
    else if( (x1 > x2) && (y1 == y2) ) {
        *ort = WESTWARD;
    }
    else if( (x1 == x2) && (y1 < y2) ) {
        *ort = NORTHWARD;
    }
    else if( (x1 == x2) && (y1 > y2) ) {
        *ort = SOUTHWARD;
    }
    else {
        /* Fatal Logical Error */        
        ASSERT(0);
    }
    
    return STATUS_OK;
}

STATUS setBotOrientation(BotOrientation nextOrientation) {
    int rotation;
    
    if(thisBotOrientation != nextOrientation) {
        rotation = getAngleFromOrientation(nextOrientation) - 
            getAngleFromOrientation(thisBotOrientation);
            
        ASSERT(rotation != 0);
        if(rotation > 180) { rotation = rotation - 360;}
        else if(rotation < -180) { rotation = rotation + 360;}

        if(rotation > 0) {
            //printf("rotateBot(LEFT, %d);\n", rotation);
			rotateBot(LEFT, rotation);
        }
        else {
            //printf("rotateBot(RIGHT, %d);\n", -rotation);
			rotateBot(RIGHT, -rotation);
        }
         
        thisBotOrientation = nextOrientation;            
    }
    return STATUS_OK;
}

STATUS gotoPosition(Map *pMap, UINT posX, UINT posY) {
    UINT idx;
    BotLocation nextLocation;
    BotOrientation nextOrientation, lookAheadOrientation;
    Path shortestPath;
    PositionMetaInfo locInfo, destLocInfo;
	STATUS ret;
	UINT dist;
  
    ASSERT(pMap != NULL);

	// debug start
	//printf("===============================\n");
	//printf("Current location: (%d, %d)\n", thisBotLocation.posX, thisBotLocation.posY);	
	//printf("Goto location: (%d, %d)\n", posX, posY);
	//printf("===============================\n");	
    // debug end
 
   /* Step #1: Are you already at destination ? */
    
    if(thisBotLocation.posX == posX && thisBotLocation.posY == posY) {
        return STATUS_OK;
    }
    
    /* Step #2: Get shortest path from current location to destination */
    
    ret = analyzeShortestRoute(pMap, thisBotLocation.posX, thisBotLocation.posY,
                            posX, posY, &shortestPath);
    ASSERT(ret == STATUS_OK);

    ret = getPositionMetaInfo(pMap, thisBotLocation.posX, thisBotLocation.posY, &locInfo);
    ASSERT(ret == STATUS_OK);

    ret = getPositionMetaInfo(pMap, posX, posY, &destLocInfo);
    ASSERT(ret == STATUS_OK);
    
	// debug start
    //printf("------ Analyzed Path -------\n");
	printPath(&shortestPath);
    //printf("----------------------------\n");	
	//debug end
	
	/* Step #3: Is your destination on the same edge? */
	if(
	    destLocInfo.loc == ON_EDGE && 
	    (
	      (
	        locInfo.loc == AT_NODE && 
	        (locInfo.nodeA == destLocInfo.nodeA || locInfo.nodeA == destLocInfo.nodeB)
	      ) 
	      ||
	      (
	        locInfo.loc == ON_EDGE && 
	        (
	          (locInfo.nodeA == destLocInfo.nodeA && locInfo.nodeB == destLocInfo.nodeB)
	          ||
  	          (locInfo.nodeA == destLocInfo.nodeB && locInfo.nodeB == destLocInfo.nodeA)
  	        )	      
	      )
	    )
	  )
	{
	    /* Destination is on same edge. Go directly there. */

        dist = getEuclideanDistance(thisBotLocation.posX, thisBotLocation.posY,
                posX, posY);
        ASSERT(dist != 0);        

        /* Set orientation */
        ret = computeOrientation(thisBotLocation.posX, thisBotLocation.posY,
                        posX, posY, &nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        ret = setBotOrientation(nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        /* Move Bot */
        ret = moveForwardFollwingLineByDistance(dist, TRUE);
        ASSERT(ret == STATUS_OK);
        //printf("moveForwardFollwingLineByDistance(%u);\n", dist);
        
        /* Update location */
        switch(nextOrientation) {
        case EASTWARD:  thisBotLocation.posX += dist; break;
        case NORTHWARD: thisBotLocation.posY += dist; break;
        case WESTWARD:  thisBotLocation.posX -= dist; break;
        case SOUTHWARD: thisBotLocation.posY -= dist; break;
        default: ASSERT(0);
        }

        ASSERT(thisBotLocation.posX == posX && thisBotLocation.posY == posY);
        return STATUS_OK;   
	}
		
    /* Step #4: Go from current location to start of shortest path */
    
    if(!(locInfo.loc == AT_NODE && locInfo.nodeA == shortestPath.nodes[0])) {
        /* You are not already at source node in shortest path */       
        ret = computeOrientation(thisBotLocation.posX, thisBotLocation.posY,
            pMap->nodeList[shortestPath.nodes[0]].posX, 
            pMap->nodeList[shortestPath.nodes[0]].posY, &nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        ret = setBotOrientation(nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        /* Calculate orientation for future move. If orientation is the same,
           Bot need not stop at end of next move */
        lookAheadOrientation = UNKNOWN_ORIENTATION;
        if(shortestPath.length != 0) {
            ret = computeOrientation(
            pMap->nodeList[shortestPath.nodes[0]].posX, 
            pMap->nodeList[shortestPath.nodes[0]].posY,
            pMap->nodeList[shortestPath.nodes[1]].posX, 
            pMap->nodeList[shortestPath.nodes[1]].posY, &lookAheadOrientation);
            ASSERT(ret == STATUS_OK);
        }
        
        if(lookAheadOrientation != nextOrientation) {
            //printf("moveForwardFollwingLineByCheckpoint(1, STOP);\n");
            moveForwardFollwingLineByCheckpoint(1, TRUE);
        }
        else {
            //printf("moveForwardFollwingLineByCheckpoint(1, NO_STOP);\n");
            moveForwardFollwingLineByCheckpoint(1, FALSE);
        }
        
        /* Update current location */
        thisBotLocation.posX = pMap->nodeList[shortestPath.nodes[0]].posX;
        thisBotLocation.posY = pMap->nodeList[shortestPath.nodes[0]].posY;
    }


    /* Step #5: Move along shortest path */

    /* LOOP INVARIANT: Bot is standing at location of the node shortestPath.nodes[idx]
     */
    for(idx = 0; idx < shortestPath.length; idx ++) {
		/* Assert loop invariant */
		ASSERT((thisBotLocation.posX == pMap->nodeList[shortestPath.nodes[idx]].posX) &&
		       (thisBotLocation.posY == pMap->nodeList[shortestPath.nodes[idx]].posY));

        /* Step #5.a: Retrieve next node location */

        nextLocation.posX = pMap->nodeList[shortestPath.nodes[idx+1]].posX;
        nextLocation.posY = pMap->nodeList[shortestPath.nodes[idx+1]].posY;

        if(pMap->nodeList[idx+1].isCheckpoint == TRUE) {
            /* Error in locating checkpoint is known */
            nextLocation.errorX = EPSILON;
            nextLocation.errorY = EPSILON;
        }
        else {
            /* TODO:  Error in locating checkpoint is to be computed */
            /* Since all nodes are checkpoints, this condition shall never occur
             */
             ASSERT(0);
        }
        
		// debug start
        //printf("----------------------------\n");
		//printf("Current location: (%d, %d)\n", thisBotLocation.posX, thisBotLocation.posY);
		//printf("Next location: (%d, %d)\n", nextLocation.posX, nextLocation.posY);        
		// debug end

        /* Step #5.b: Set Bot orientation */
        
        ret = computeOrientation(thisBotLocation.posX, thisBotLocation.posY,
                        nextLocation.posX, nextLocation.posY, &nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        ret = setBotOrientation(nextOrientation);
        ASSERT(ret == STATUS_OK);


		// debug start
		//printf("thisBotOrientation: %d\n", thisBotOrientation);		
		//printf("nextOrientation: %d\n", nextOrientation);
        //printf("----------------------------\n");
		// debug end

        /* Step #5.c: Compute look ahead orientation */        
        /* Calculate orientation for future move. If orientation is the same,
           Bot need not stop at end of next move */
        lookAheadOrientation = UNKNOWN_ORIENTATION;
        if((idx+2) <= shortestPath.length) {
            ret = computeOrientation(
                nextLocation.posX, nextLocation.posY,
                pMap->nodeList[shortestPath.nodes[idx+2]].posX, 
                pMap->nodeList[shortestPath.nodes[idx+2]].posY, &lookAheadOrientation);
            ASSERT(ret == STATUS_OK);
        }
       
        /* Step #5.d: Follow the white line till next node */
        if(lookAheadOrientation != nextOrientation) {
            //printf("moveForwardFollwingLineByCheckpoint(1, STOP);\n");
            moveForwardFollwingLineByCheckpoint(1, TRUE);
        }
        else {
            //printf("moveForwardFollwingLineByCheckpoint(1, NO_STOP);\n");
            moveForwardFollwingLineByCheckpoint(1, FALSE);
        }		

        thisBotLocation = nextLocation;
    }

	/* Assert loop invariant */
    ASSERT((thisBotLocation.posX == pMap->nodeList[shortestPath.nodes[idx]].posX) &&
           (thisBotLocation.posY == pMap->nodeList[shortestPath.nodes[idx]].posY));


    /* Bot is currently standing at end node in shortest path */
    ASSERT(thisBotLocation.posX == posX || thisBotLocation.posY == posY);
    
    /* Step #6: Go from end of shortest path to desired location */
    dist = getEuclideanDistance(thisBotLocation.posX, thisBotLocation.posY,
                posX, posY);
                
    if(dist != 0) {
        /* Set orientation */
        ret = computeOrientation(thisBotLocation.posX, thisBotLocation.posY,
                        posX, posY, &nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        ret = setBotOrientation(nextOrientation);
        ASSERT(ret == STATUS_OK);
        
        /* Move Bot */
        ret = moveForwardFollwingLineByDistance(dist, TRUE);
        ASSERT(ret == STATUS_OK);
        //printf("moveForwardFollwingLineByDistance(%u);\n", dist);
        
        /* Update location */
        switch(nextOrientation) {
        case EASTWARD:  thisBotLocation.posX += dist; break;
        case NORTHWARD: thisBotLocation.posY += dist; break;
        case WESTWARD:  thisBotLocation.posX -= dist; break;
        case SOUTHWARD: thisBotLocation.posY -= dist; break;
        default: ASSERT(0);
        }
    }

    ASSERT(thisBotLocation.posX == posX && thisBotLocation.posY == posY);
    
	return STATUS_OK;
}

STATUS gotoForward(Map *pMap, UINT distInMm) {
    UINT posX, posY;
    PositionMetaInfo info;
    STATUS ret;

    ASSERT(pMap != NULL);
    
    posX = thisBotLocation.posX;
    posY = thisBotLocation.posY;
    
    switch(thisBotOrientation) {
        case EASTWARD: posX += distInMm; break;
        case NORTHWARD: posY += distInMm; break;
        case WESTWARD: posX -= distInMm; break;
        case SOUTHWARD: posY -= distInMm; break;
        default: ASSERT(0);                        
    }
    
    /* Check if position in on the map */
    ret = getPositionMetaInfo(pMap, posX, posY, &info);
    ASSERT(ret == STATUS_OK);

    if(info.loc == OUTSIDE_EDGE) {
        return !STATUS_OK;
    }
    
    return gotoPosition(pMap, posX, posY);
}

void test_getPositionMetaInfo(Map *pMap) {
    UINT x, y;
    PositionMetaInfo info;
    
    //printf("P3\n");
    //printf("3000 3000\n");
    //printf("15\n");
    for(y = 0; y < 3000; y ++) {
        for(x = 0; x < 3000; x ++) {

            ////printf("(%d, %d) => ", x, y);            
            getPositionMetaInfo(pMap, x, y, &info);
                
            switch(info.loc) {
                case AT_NODE: //printf(" 15 0 0 \n"); break;                
                case ON_EDGE: 
                    //printf(" 0 0 15 \n"); 
                    break;
                case OUTSIDE_EDGE: //printf(" 0 0 0 \n"); break;
                default: ASSERT(0);
            }
            /*            
            //printf("(%d, %d) ", pMap->nodeList[info.nodeA].posX,
                    pMap->nodeList[info.nodeA].posY);
                    
            //printf("(%d, %d) \n", pMap->nodeList[info.nodeB].posX,
                    pMap->nodeList[info.nodeB].posY);
                    */
        }
    }
}

void test_getShortestPath(Map *pMap) {
    Path p;
    UINT i, src, dest;
    
    src = 4;
    dest = 4;    
    getShortestPath(pMap, src, dest, &p);
    
    //printf("Path: ");
    for(i = 0; p.nodes[i] != dest; i++){
        //printf("%d, ", p.nodes[i]);
    }
    //printf("%d\n", p.nodes[i]);
    //printf("Length: %u\n", p.length);
    //printf("Distance: %u\n", p.distance);
}


void test_analyzeShortestRoute(Map *pMap) {
    Path p;
    STATUS ret;
    UINT i;

    ret = analyzeShortestRoute(pMap, 0, 405, 609, 1, &p);
    ASSERT(ret == STATUS_OK);
    
    //printf("Path: ");
    for(i = 0; i < p.length; i++){
        //printf("%d, ", p.nodes[i]);
    }
    //printf("%d\n", p.nodes[i]);
    //printf("Length: %u\n", p.length);
    //printf("Distance: %u\n", p.distance);

}

void test_gotoPosition(Map *pMap) {
    PositionMetaInfo info1, info2;
    UINT idx, idx2, last, x1, y1;
    STATUS ret;
    UINT xCor[6000], yCor[6000];
    UINT ort;
    
    idx = 0;
    for(x1 = 0; x1 <= 3000; x1 ++) {
        for(y1 = 0; y1 <= 3000; y1 ++) {
            ret = getPositionMetaInfo(pMap, x1, y1, &info1);
            ASSERT(ret == STATUS_OK);
            
            if(info1.loc == AT_NODE || info1.loc == ON_EDGE) {
                xCor[idx] = x1;
                yCor[idx] = y1;
                idx ++;
            }
        }
    }
    
    last = idx;
    for(idx = 0; idx < last; idx ++) {
        for(idx2 = 0; idx2 < last; idx2 ++) {
            for(ort = 0; ort < 4; ort ++) {
                /* Otherwise simulate goto */
                thisBotLocation.posX = xCor[idx]; 
                thisBotLocation.posY = yCor[idx];
                thisBotOrientation = ort;
                ret = gotoPosition(pMap, xCor[idx2], yCor[idx2]);
                ASSERT(ret == STATUS_OK);
            }
        }
        fprintf(stderr, "idx:%d\n", idx);        
    }
}

void test_gotoPosition2(Map *pMap){
    struct testCase {
        UINT x1, y1, x2, y2;
    };
    struct testCase test[] = {
        {2040,     0, 2040,    0},  /* AN-AN, same node */
        {   0,   880, 2040,   880}, /* AN-AN, distinct, no intermediate node */
        {   0,   880, 2040,    0},  /* AN-AN, distinct, one intermediate node */
        {2040,     0, 2040,  300},  /* AN-BN, no intermediate nodes */
        {2040,     0,    0,  200},  /* AN-BN, one intermediate node */
        {1500,   880,    0,  880},  /* BN-AN, no intermediate nodes */
        {1200,     0, 2040,  880},  /* BN-AN, one intermediate nodes */
        {2040,   500, 2040,  300},  /* BN-BN, no intermediate nodes */        
        {  20,     0, 2040,   30}   /* BN-BN, one intermediate nodes */        
    };
    UINT idx;
    STATUS ret;
    
    for(idx = 0; idx < (sizeof(test)/sizeof(test[0])); idx ++) {
        thisBotLocation.posX = test[idx].x1; 
        thisBotLocation.posY = test[idx].y1;
        thisBotOrientation = EASTWARD;
        ret = gotoPosition(pMap, test[idx].x2, test[idx].y2);
        ASSERT(ret == STATUS_OK);
    }
}

#if 0
int main() {
    Map thisMap;
    STATUS ret;
    
    initBotGuidanceSystem(stdin, &thisMap);
/*        thisBotLocation.posX = 0; 
        thisBotLocation.posY = 1;
        thisBotOrientation = EASTWARD;
        gotoPosition(&thisMap, 1, 880);*/

    test_gotoPosition(&thisMap);
//    test_getPositionMetaInfo(&thisMap);
//    gotoForward(&thisMap, 20);
    return 0;
}
#endif
