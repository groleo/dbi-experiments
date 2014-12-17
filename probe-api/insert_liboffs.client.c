/* **********************************************************
 * Copyright (c) 2008 VMware, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of VMware, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/* This client registers a probe with library offset. */

/* To prevent cl from complaining about calling standard c library routine. */
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <string.h>

#include "dr_api.h"
#include "dr_probe.h"
#include "dr_tools.h"
#include "drmgr.h"
#include "drsyms.h"
#include "drwrap.h"


#define NUM_PROBES 1
dr_probe_desc_t probes[NUM_PROBES];

/*----------------------------------------------------------------------------*/
#include "dr_defines.h"

/* This probe increments the argument to insert_liboffs.c:doubler() */
void doubler_probe(dr_mcontext_t *cxt)
{
    //volatile reg_t *arg_p = (reg_t *)(cxt->xsp + sizeof(reg_t));
    //*arg_p = (*arg_p) + 1;
    dr_printf("cococo\n");
}


/*----------------------------------------------------------------------------*/
/* TODO: Figure out a way to share this code across all probe-api tests.  Can't
 * put it inside tools.c as it links against external libraries.  Ok for now as
 * this is the only one test.
 */
extern unsigned long strtoul(const char *nptr, char **endptr, int base);
/* Library offset has to be computed before the probe library is loaded
 * into memory.  Reading it from the map file is one of the easiest ways to
 * do it.
 */

/*----------------------------------------------------------------------------*/
static void probe_def_init(void)
{
    probes[0].name = "chrome probe";
    probes[0].insert_loc.type = DR_PROBE_ADDR_LIB_OFFS;
    probes[0].insert_loc.lib_offs.library = "../mutatee/chrome";
    drsym_init(0);

    {
     size_t exe_export_offs;
     drsym_error_t r = drsym_lookup_symbol("../mutatee/chrome", "doubler",&exe_export_offs, DRSYM_DEMANGLE);
     if (r!=DRSYM_SUCCESS) {
      dr_fprintf(STDERR, "<FAILED to find gpu::gles2::GLES2Implementation::Viewport\n");
     } else {
      dr_printf("<Found Original symbol>\n");
      probes[0].insert_loc.lib_offs.offset = exe_export_offs;
     }
    }

    //probes[0].insert_loc.lib_offs.offset = 0x50530;



    probes[0].callback_func.type = DR_PROBE_ADDR_LIB_OFFS;
    probes[0].callback_func.lib_offs.library = "libhooks.so";
    {
     size_t exe_export_offs;
     drsym_error_t r = drsym_lookup_symbol("libhooks.so", "preHook",&exe_export_offs, DRSYM_DEMANGLE);
     if (r!=DRSYM_SUCCESS) {
      dr_fprintf(STDERR, "<FAILED to find gpu::gles2::GLES2Implementation::Viewport>\n");
     } else {
      dr_printf("<Found Hook symbol>\n");
      probes[0].callback_func.lib_offs.offset = exe_export_offs;
     }
    }
    drsym_exit();
/*
    probes[0].callback_func.lib_offs.offset = 0xe30;
*/
}

DR_EXPORT
void dr_init(client_id_t client_id)
{
    dr_probe_status_t stat;
    probe_def_init();
    if ( ! dr_register_probes(&probes[0], NUM_PROBES) ) {
      dr_printf("register failed\n");
    }
    if (probes[0].status==DR_PROBE_STATUS_UNSUPPORTED) {
     dr_printf("UN_SU_POR_TED\n");
    }

    int rv = dr_get_probe_status(probes[0].id, &stat);
    if (!rv && stat==DR_PROBE_STATUS_INVALID_ID) {
       dr_printf("Failed to register probe: %d\n", probes[0].id);
    }
}
