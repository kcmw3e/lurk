#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "result.h"

result_t log_default(result_t result, const char* restrict fmt, va_list args);
result_t err_default(result_t result, const char* caller, const char* loc, const char* restrict fmt, va_list args);

static const result_config_t result_config_default = {
    .projname = "lurk",
    .do_log = true,
    .do_err = true,
    .log_fn = &log_default,
    .err_fn = &err_default,
};

static const result_config_t* result_config = NULL;


struct tm get_time() {
    time_t traw = time(NULL);
    struct tm t = {0};
    struct tm* tp = gmtime(&traw);
    if (tp != NULL) t = *tp;
    return t;
}

const char* get_config_projname() {
    if (result_config == NULL) return result_config_default.projname;

    const char* projname = result_config->projname;
    if (projname == NULL) return result_config_default.projname;

    return projname;
}

bool get_config_do_log() {
    if (result_config == NULL) return result_config_default.do_log;
    return result_config->do_log;
}

bool get_config_do_err() {
    if (result_config == NULL) return result_config_default.do_err;
    return result_config->do_err;
}

result_log_fn* get_config_log_fn() {
    if (result_config == NULL) return result_config_default.log_fn;

    result_log_fn* log_fn = result_config->log_fn;
    if (log_fn == NULL) return result_config_default.log_fn;

    return log_fn;
}

result_err_fn* get_config_err_fn() {
    if (result_config == NULL) return result_config_default.err_fn;

    result_err_fn* err_fn = result_config->err_fn;
    if (err_fn == NULL) return result_config_default.err_fn;

    return err_fn;
}

bool is_err(result_t result) {
    return result < 0;
}

bool is_lurk_err(result_t result) {
    switch (result) {
        case (RESULT_INVALID_OBJECT):
        case (RESULT_INTERNAL_ERROR):
        case (RESULT_BAD_PARAM):
            return true;
        default:
            break;
    }
    return false;
}

result_t lurk_set_result_config(result_config_t* config) {
    result_config = config;
    return RESULT_SUCCESS;
}

result_t lurk_log(result_t result, const char* fmt, ...) {
    if (fmt == NULL) return RESULT_BAD_PARAM;

    if (!get_config_do_log()) return RESULT_SUCCESS;

    result_log_fn* log_fn = get_config_log_fn();

    va_list args;

    va_start(args, fmt);

    result = (*log_fn)(result, fmt, args);

    va_end(args);

    return RESULT_SUCCESS;
}

result_t lurk_err(result_t result, const char* caller, const char* loc, const char* fmt, ...) {
    if (fmt == NULL) return RESULT_BAD_PARAM;

    if (!get_config_do_err()) return RESULT_SUCCESS;

    result_err_fn* err_fn = get_config_err_fn();

    va_list args;

    va_start(args, fmt);

    result = (*err_fn)(result, caller, loc, fmt, args);

    va_end(args);

    return result;
}

result_t log_default(result_t result, const char* restrict fmt, va_list args) {
    if (fmt == NULL) return RESULT_BAD_PARAM;

    if (!get_config_do_log()) return result;

    const char* projname = get_config_projname();

    struct tm t = get_time();

    int n;

    n = fprintf(stdout, "%02d:%02d:%02d  %08x  [%s]  ",
                    t.tm_hour, t.tm_min, t.tm_sec, result, projname);

    // this *shouldn't* ever happen, but can't be too careful (if it does, the calling program
    // should be able to intercept the abort signal if desired)
    if (n < 0) abort();

    n = vfprintf(stdout, fmt, args);
    if (n < 0) abort();

    n = fprintf(stdout, "\n");
    if (n < 0) abort();

    return result;
}

result_t err_default(result_t result, const char* caller, const char* loc, const char* restrict fmt, va_list args) {
    if (fmt == NULL) return RESULT_BAD_PARAM;

    if (!get_config_do_err()) return result;

    if (caller == NULL) caller = "(unknown)";
    if (loc == NULL) loc = "???";

    struct tm t = get_time();

    const char* projname = get_config_projname();

    int n = fprintf(stderr, "%02d:%02d:%02d  %08x  [%s:%s.%s]  ",
                    t.tm_hour, t.tm_min, t.tm_sec, result, projname, caller, loc);
    if (n < 0) abort(); // this *shouldn't* ever happen, but just in case

    n = vfprintf(stderr, fmt, args);
    if (n < 0) abort();

    n = fprintf(stderr, "\n");
    if (n < 0) abort();

    return result;
}
