#include "BPatch.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_flowGraph.h"

#include <string>
#include <omp.h>



static BPatch_addressSpace *app;
static BPatch bpatch;
typedef enum {
	create_process,
	attach_process,
	open_binary
}
accessType_t;



BPatch_addressSpace*
startInstrumenting(accessType_t accessType, const char *name, int pid, const char *argv[])
{
	printf("%s : %d \n",__FUNCTION__,__LINE__);
	BPatch_addressSpace *handle = NULL;
	bpatch.setTrampRecursive(true);
	bpatch.setDebugParsing(false);
	bpatch.setDelayedParsing(true);
	bpatch.setTypeChecking(false);
	bpatch.setLivenessAnalysis(false);
	switch (accessType) {
		case create_process:
			printf("%s : %d \n",__FUNCTION__,__LINE__);
			handle = bpatch.processCreate(name, argv);
			printf("%s : %d \n",__FUNCTION__,__LINE__);
			break;
		case attach_process:
			handle = bpatch.processAttach(name, pid);
			break;
		case open_binary:
			printf("%s : %d \n",__FUNCTION__,__LINE__);
			handle = bpatch.openBinary(name);
			printf("%s : %d \n",__FUNCTION__,__LINE__);
			break;
	}
	return handle;
}



std::vector<BPatch_point *>*
findEntryPoint(BPatch_addressSpace *app, const char* funcName)
{
	printf("%s : %d\n",__FUNCTION__,__LINE__);
	std::vector<BPatch_function *> functions;
	std::vector<BPatch_point *> *points=NULL;
	BPatch_image *appImage = app->getImage();

	vector<BPatch_module *> * modules = appImage->getModules();
	auto rv = modules->at(0)->findFunction(funcName, functions);
	if (!rv || functions.empty()) {
	 printf("error: unable to find %s\n", funcName);
	 exit(EXIT_FAILURE);
	}
	points = functions[0]->findPoint(BPatch_entry);
	return points;
}



void
createAndInsertSnippet(BPatch_addressSpace *app, std::vector<BPatch_point *> *points)
{
	app->loadLibrary("./libhooks.so");
	BPatch_image *appImage = app->getImage();

	std::vector<BPatch_function *> printfFuncs;
	appImage->findFunction("preHook", printfFuncs);
	if ( printfFuncs.empty() ) {
		printf("unable to find function for printf\n");
		exit(EXIT_FAILURE);
	}
	vector< BPatch_snippet * > printfArgs;
	BPatch_constExpr arg0 ("Hook was called\n" );
	printfArgs.push_back( & arg0 );

	BPatch_funcCallExpr callPrintf( *printfFuncs[0], printfArgs );

	printf("%s : %d \n",__FUNCTION__,__LINE__);
	app->insertSnippet(callPrintf, *points);
	printf("%s : %d \n",__FUNCTION__,__LINE__);
}



void
finishInstrumenting(BPatch_addressSpace *app, const char *newName)
{
	BPatch_process *appProc = dynamic_cast<BPatch_process *>(app);
	BPatch_binaryEdit *appBin = dynamic_cast<BPatch_binaryEdit *>(app);
	if (appProc) {
		appProc->continueExecution();
		while (!appProc->isTerminated()) {
			bpatch.waitForStatusChange();
		}
	}
	if (appBin) {
		appBin->writeFile(newName);
	}
}



int
main(int argc, const char* argv[])
{
	if (argc < 3) {
		fprintf(stderr, "usage:\n\tmutator -- mutatee arg0 arg1 ...\n");
		exit(EXIT_FAILURE);
	}

	int progPID = 42;
	double start,end;
	const char* fileName = argv[2];
	const char** procArgv = argv+2;
	printf("%s : %d \n",__FUNCTION__,__LINE__);

	start = omp_get_wtime();
	app = startInstrumenting(
			create_process,
			fileName,
			progPID,
			procArgv);
	if (!app) {
		printf("error: unable to init app\n");
		exit(EXIT_FAILURE);
	}
	end = omp_get_wtime();
	printf("startInstrumenting %f\n", end-start);

	start = omp_get_wtime();
	auto entryPoint = findEntryPoint(app, "gpu::gles2::GLES2Implementation::Viewport");
	if (!entryPoint) {
		printf("error: unable to find entry point\n");
		exit(EXIT_FAILURE);
	}
	end = omp_get_wtime();
	printf("findEntryPoint %f\n", end-start);

	start = omp_get_wtime();
	createAndInsertSnippet(app, entryPoint);
	end = omp_get_wtime();
	printf("createAndInsertSnippet %f\n", end-start);

	start = omp_get_wtime();
	finishInstrumenting(app, "/tmp/chrome.hooked");
	end = omp_get_wtime();
	printf("finishInstrumenting %f\n", end-start);

	return 0;
}
