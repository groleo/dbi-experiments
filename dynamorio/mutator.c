/* **********************************************************
 * Copyright (c) 2011-2014 Google, Inc.  All rights reserved.
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
 * * Neither the name of Google, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL GOOGLE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/* Illustrates using the drwrap extension.
 *
 * Wraps malloc on Linux, HeapAlloc on Windows.  Finds the maximum
 * malloc size requested, and randomly changes a malloc to return
 * failure to test an application's handling of out-of-memory
 * conditions.
 */
#include <string.h>

#include "dr_api.h"
#include <drmgr.h>
#include "drsyms.h"

#include "drwrap.h"

#ifdef WINDOWS
# define IF_WINDOWS_ELSE(x,y) x
#else
# define IF_WINDOWS_ELSE(x,y) y
#endif

//static void event_exit(void);
static void wrap_pre(void *wrapcxt, OUT void **user_data);
static void wrap_post(void *wrapcxt, void *user_data);

static void *max_lock; /* to synch writes to max_malloc */

static
void so_load_event(void *drcontext, const module_data_t *mod, bool loaded)
{
    app_pc towrap = (app_pc) dr_get_proc_address(mod->handle, "gpu::gles2::GLES2Implementation::Viewport");

    if (towrap != NULL) {
        bool ok = drwrap_wrap(towrap, wrap_pre, wrap_post);
        if (ok)
            dr_fprintf(STDERR, "<wrapped gpu::gles2::GLES2Implementation::Viewport @"PFX"\n", towrap);
        else {
            /* We expect this w/ forwarded exports (e.g., on win7 both
             * kernel32!HeapAlloc and kernelbase!HeapAlloc forward to
             * the same routine in ntdll.dll)
             */
            dr_fprintf(STDERR, "<FAILED to wrap gpu::gles2::GLES2Implementation::Viewport @"PFX": already wrapped?\n", towrap);
        }
    } else {
        dr_fprintf(STDERR,"symbol not found in %s\n", mod->full_path);
    }
}

static
void module_load_event(void *drcontext, const module_data_t *mod, bool loaded)
{
    //dr_printf("load module %s addr %p endaddr %p %d\n",mod->full_path,mod->start,mod->end, loaded);
    if (strcmp(dr_module_preferred_name(mod),"chrome"))
        return;

    size_t exe_export_offs;
    drsym_init(0);
    drsym_error_t r = drsym_lookup_symbol(mod->full_path, "gpu::gles2::GLES2Implementation::Viewport",&exe_export_offs, DRSYM_DEMANGLE);
    if (r!=DRSYM_SUCCESS) {
        dr_fprintf(STDERR, "<FAILED to find gpu::gles2::GLES2Implementation::Viewport\n");
    } else {
        app_pc towrap = exe_export_offs + mod->start;
        bool ok = drwrap_wrap(towrap, wrap_pre, NULL);
        if (ok) {
            dr_fprintf(STDERR, "<wrapped gpu::gles2::GLES2Implementation::Viewport @"PFX"\n", towrap);
        }
        else {
            dr_fprintf(STDERR, "<FAILED to wrap gpu::gles2::GLES2Implementation::Viewport @"PFX": already wrapped?\n", towrap);
        }
    }
    drsym_exit();
}


void
fork_init_event(void *drcontext){
    dr_fprintf(STDERR,"fork\n");
}

DR_EXPORT void
dr_init(client_id_t id)
{
    dr_set_client_name("DynamoRIO Sample Client 'wrap'", "http://dynamorio.org/issues");
    /* make it easy to tell, by looking at log file, which client executed */
    dr_log(NULL, LOG_ALL, 1, "Client 'wrap' initializing\n");
    /* also give notification to STDERR */
    if (dr_is_notify_on()) {
# ifdef WINDOWS
        /* ask for best-effort printing to cmd window.  must be called in dr_init(). */
        dr_enable_console_printing();
# endif
        dr_fprintf(STDERR, "Client wrap is running\n");
    }
    drwrap_init();
    //dr_register_exit_event(event_exit);
    //dr_register_module_load_event(module_load_event);
    drmgr_register_module_load_event(module_load_event);
    //max_lock = dr_mutex_create();
}
#if 0
static void
event_exit(void)
{
    dr_mutex_destroy(max_lock);
    drwrap_exit();
}
#endif

static void
wrap_pre(void *wrapcxt, OUT void **user_data)
{
    //dr_fprintf(STDERR,"preHook called\n");
    return;
}

static void
wrap_post(void *wrapcxt, void *user_data)
{
}

