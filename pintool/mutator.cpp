/*BEGIN_LEGAL
Intel Open Source License

Copyright (c) 2002-2014 Intel Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */

#include "pin.H"
#include <iostream>
#include <strings.h>

using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

static void (*pf_orig)(int);

/* ===================================================================== */
/* Replacement Functions */
/* ===================================================================== */


void SleepProbe( int b )
{
    cerr << "preHook called" << endl;
    if (pf_orig)
    {
        (pf_orig)( b );
    }
}



/* ===================================================================== */

// Returns TRUE if baseName matches tail of imageName. Comparison is case-insensitive.
// Parameters:
//   imageName  image file name in either form with extension
//   baseName   image base name with extension (e.g. kernel32.dll)
BOOL MatchedImageName(const string & imageName, const string & baseName)
{
    if (imageName.size() >= baseName.size())
    {
        return strcasecmp(imageName.c_str() + imageName.size() - baseName.size(), baseName.c_str()) == 0;
    }
    return FALSE;
}


/* ===================================================================== */

// Called every time a new image is loaded
// Look for routines that we want to probe

VOID ImageLoad(IMG img, VOID *v)
{

    RTN sleepRtn = RTN_Invalid();
    // Look for Sleep only in kernel32.dll
    if (MatchedImageName(IMG_Name(img), "chrome"))
    {
        cerr << "chrome" <<endl;
        sleepRtn = RTN_FindByName(img, "_ZN3gpu5gles219GLES2Implementation8ViewportEiiii");
    }

    if (RTN_Valid(sleepRtn))
    {
        if (  RTN_IsSafeForProbedReplacement( sleepRtn ) )
        {
            pf_orig = (void (*)(int)) RTN_ReplaceProbed(sleepRtn, AFUNPTR(SleepProbe));

            cerr << "<inserted probe for _ZN3gpu5gles219GLES2Implementation8ViewportEv> in " << IMG_Name(img) << endl;
        }
    }
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    PIN_Init(argc,argv);

    IMG_AddInstrumentFunction(ImageLoad, 0);

    PIN_StartProgramProbed();

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
