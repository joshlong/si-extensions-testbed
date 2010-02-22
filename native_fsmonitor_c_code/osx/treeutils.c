/*
 * Copyright 2010 the original author or authors
 *
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

 

 /*
     Contributed by:

     Mario Gray (mario.gray@gmail.com)
 */



#include "treeutils.h"
/**
	
	TreeUtils will work by giving differences in Directory structure from the last scan. 
	(state change inference).
	The primary object is to determine that a file has been added or remove.
	with the following additions in mind for the future:
	- determine whether a file is 'HOT', that is actively being written.
	- we may/not need to worry about renaming files (E.x. temp file rename after download)
**/

static const int	TEST_SLEEP_MS = 500;

/*
	What if a or b were NULL ?
*/
static int cmpTnode(const treeNode *a, const treeNode *b) 
{		
	printf("Compare a=%s , b=%s\n", a->d_name, b->d_name);
	if( a->d_ino > b->d_ino )  
		return 1;
	if( a->d_ino < b->d_ino )  
		return -1;				
		return 0;

}


// Create snapshot buffer ( of dirents + metadata) of FILES for a single level directory
struct directory getDirSnapshot(void *path) {
	
	struct directory myDirStruct;
	int dirSize = 0;
							// Allocate memory for the buffer of 'dirent's
							// by 1st determining the size of the directory
							
	DIR *mydir = opendir(path);
	struct dirent *file;
	while( (file = readdir(mydir)) !=NULL ) {
		if(qualifyFile(path,file->d_name) )
			dirSize++;
	}
	rewinddir(mydir);
	myDirStruct.elements = dirSize;
	
	void *_fBuffer = calloc(1+dirSize, sizeof(treeNode));
	myDirStruct.treeNodes= _fBuffer;
	treeNode *treeNodes= _fBuffer;				// make dereference to buffer of 'dirent's		
	

	// Fill treeNodes with dirent entries
	int i= 0;
	file = NULL;
	while( (file = readdir(mydir)) != NULL) {		
			if(qualifyFile(path, file->d_name) ) {
				memcpy(treeNodes,file, sizeof(struct dirent));			
				treeNodes->nodeID = (i++)*2;
				//printf("Treenode= %s\n", treeNodes->d_name);
				treeNodes++;
			}
		}
	closedir(mydir);
	
	return myDirStruct;
}

// Builds the binary tree for a given directory (having a snapshot buffer)
void buildDirectoryBinTree(struct directory *_inDirPtr) {
	
	struct directory _inDir = *_inDirPtr;
	void *binTree = *(void **)&_inDir.binTree;			// Make dereference to the binaryTree;
	treeNode *treeNodes = _inDir.treeNodes;	
	int i= 0;
	for(i=0;i<_inDir.elements; i++) {		
			tsearch(treeNodes, binTree, (int (*)(const void*,const void*))cmpTnode);
			treeNodes++;
	}
}

// Destroys everything within a directory ( buffer + tree )
void destroyDirectory(struct directory *_inDirPtr) {

	struct directory _inDir = *_inDirPtr;
	void *binTree = *(void **)&_inDir.binTree;			// Make dereference to the binaryTree;
	treeNode *treeNodes = _inDir.treeNodes;
		
	int i=0;
	for(i=0;i<_inDir.elements; i++) {
		tdelete(treeNodes, binTree, (int (*)(const void*,const void*))cmpTnode);
		treeNodes++;
	}

	_inDir.binTree = NULL;
	free(_inDir.treeNodes);
}



/* walks the directory, looking for files that are missing from _fTree,
	calls the 'notice' callback function when a new file is detected.
 */
int checkFileAdded(char *testPath,  struct directory *_fDirPtr, void (*notice)(const void *))
{
// Scan by (not adding nodes) and notify when something's not found.  
// re-scan (build tree) afterwards.
	struct directory _fDir		= *_fDirPtr;
	void *binTree				= *(void **)&_fDir.binTree;			// Make dereference to the binaryTree;
printf("POINTER TO %p\n", cmpTnode);	
	struct directory _newDir	= getDirSnapshot(testPath);
	void		*newBinTree		= *(void **)&_newDir.binTree;
	treeNode	*newTreeNodes	= _newDir.treeNodes;
	
			
			
	int i =0;
	// Searches the old tree by using newtree keys (Will notify of keys that are in new tree but not in oldtree )
	for( i =0;i<_newDir.elements; i++) {		
		// Automagically add it to the new tree
				
		tsearch(newTreeNodes, newBinTree, (int (*)(const void*,const void*))cmpTnode);
		  	
			// Case 1:  All files must be at least 5 seconds old!
			
		// Search for a file in the old tree
		// Not found in old tree == New File!	
		void *fnd = tfind(newTreeNodes, binTree, (int (*)(const void*,const void*))cmpTnode);
		if(fnd==NULL) {
			 if(notice!=NULL)
					notice( newTreeNodes);
		}
		
		newTreeNodes++;
	}

	treeNode *oldTreeNodes = _fDir.treeNodes;
	// Now look for removed files
	for(i =0 ; i<_fDir.elements; i++) {
	//printf("Scandel: %s\n", oldTreeNodes->d_name);
		void *fnd = tfind(oldTreeNodes, newBinTree, (int (*)(const void*,const void*))cmpTnode);
		if(fnd == NULL) {
			printf("A file was deemed removed: %s\n", oldTreeNodes->d_name);
		}
		oldTreeNodes++;
	}
	
	destroyDirectory(_fDirPtr);
	_fDirPtr->elements = _newDir.elements;
	_fDirPtr->treeNodes = _newDir.treeNodes; 
	_fDirPtr->binTree = newBinTree;//buildDirectoryBinTree( _fDirPtr );

	return 0;
}


/**
	Test time between modificaitons to determine that a files has finished being written.
	( fisrt == second ) : COMPLETE
	( first != second ) : INCOMPLETE
**/
bool testWriteComplete(const char *path) {
	bool isWriteComplete = false;
	
	struct stat frstStat;
	struct stat secondStat;
	int statRet = stat( path, &frstStat);

	if(! statRet ) {
		
		usleep(1000*TEST_SLEEP_MS);
		
		if( ! stat ( path, &secondStat ) ) {
			
			if(frstStat.st_mtime == secondStat.st_mtime )
				isWriteComplete == true;
		}
	}
	
	// HERMM? ! ( just a test ) will block!
	if(!isWriteComplete) isWriteComplete = testWriteComplete( path );
	
	
	return isWriteComplete;
}


static void getContents(const void *node, VISIT v, int k)
{
	const void *myvoid = *(void **)node;
	const treeNode  *myNode = ( treeNode *)myvoid;
	
	if(v != postorder && v != leaf) return;

		printf("WALK: %s\n", myNode->d_name);
}

void printMyfiles(void *_fTree) {
	twalk(*(void **)_fTree, &getContents);
}



/** Qualifies a file as being in some case 'an actual file or directory with specific 'stat' values **/
 bool qualifyFile(char *fp, char *name) {
		bool isQual = false;
		struct stat fileStat;
		
		char fullPath[175] ;//= calloc(1,strlen(fp)+strlen(name)+1 );

		sprintf(fullPath, "%s/%s",fp, name);

		if( ! stat ( fullPath, &fileStat ) ) {
			
				if( S_ISREG(fileStat.st_mode) )
					isQual=true;
		}
		
		
		/*free(fullPath);
*/		
		return isQual;
}

