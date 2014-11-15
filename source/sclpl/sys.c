/**
  @file sys.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <stdlib.h>
#include "sys.h"
#include "opts.h"
#include "log.h"

str_t* sys_bin_dir(void) {
    str_t* bindir = NULL;
    str_t* slash = str_new("/");
    str_t* progname = str_new(opts_prog_name());
    size_t index = str_rfind(progname, slash);
    str_t* path = (index == SIZE_MAX) ? NULL : str_substr(progname, 0, index+1);
    str_t* prog = (index == SIZE_MAX) ? str_new(str_cstr(progname)) : str_substr(progname, index+1, str_size(progname));
    if (NULL != path) {
        bindir = mem_retain(path);
    } else {
        log_error("Could not locate the bin directory");
        exit(1);
    //    str_t* pathvar = str_new(getenv("PATH"));
    //    str_t* sep = str_new(":");
    //    vec_t* paths = str_split(pathvar, sep);
    //    for (size_t idx = 0u; idx < vec_size(paths); idx++) {
    //        str_t* currpath = (str_t*)vec_at(paths, idx);
    //        str_t* binpath = str_concat(str_concat(currpath, slash), prog);
    //        if (file_exists(str_cstr(binpath))) {
    //            bindir = binpath;
    //            mem_release(currpath);
    //            break;
    //        }
    //        mem_release(currpath);
    //        mem_release(binpath);
    //    }
    //    mem_release(sep);
    //    mem_release(pathvar);
    //    mem_release(paths);
    }
    mem_release(slash);
    mem_release(progname);
    mem_release(path);
    mem_release(prog);
    return bindir;
}

str_t* sys_inc_dir(void) {
    str_t* bindir  = sys_bin_dir();
    str_t* pathmod = str_new("../include/");
    str_t* incdir  = str_concat(bindir, pathmod);
    mem_release(bindir);
    mem_release(pathmod);
    return incdir;
}

str_t* sys_extension(file_type_t ftype) {
    str_t* ext = NULL;
    switch (ftype) {
        case TOKFILE:   ext = str_new(".tok"); break;
        case ASTFILE:   ext = str_new(".ast"); break;
        case CSOURCE:   ext = str_new(".c");   break;
        case OBJECT:    ext = str_new(".o");   break;
        case PROGRAM:   ext = str_new("");     break;
        case STATICLIB: ext = str_new(".a");   break;
        case SHAREDLIB: ext = str_new(".lib"); break;
        default:        ext = str_new("");     break;
    }
    return ext;
}

str_t* sys_filename(file_type_t ftype, str_t* infile) {
    str_t* ext_ind = str_new(".");
    size_t index   = str_rfind(infile, ext_ind);
    str_t* rawname = str_substr(infile, 0, index);
    str_t* ext = sys_extension(ftype);
    str_t* fname = str_concat(rawname, ext);
    mem_release(ext_ind);
    mem_release(rawname);
    mem_release(ext);
    return fname;
}

