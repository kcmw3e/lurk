#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lurk.h"
#include "result.h"

void log_default(result_t result, const char* restrict fmt, va_list args);
void err_default(result_t result, const char* caller, const char* loc, const char* restrict fmt, va_list args);

static const result_config_t result_config_default = {
    .projname = "lurk",
    .prefix = "",
    .postfix = "\n",
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

const char* get_config_prefix() {
    if (result_config == NULL) return result_config_default.prefix;

    const char* prefix = result_config->prefix;
    if (prefix == NULL) return result_config_default.prefix;

    return prefix;
}

const char* get_config_postfix() {
    if (result_config == NULL) return result_config_default.postfix;

    const char* postfix = result_config->postfix;
    if (postfix == NULL) return result_config_default.postfix;

    return postfix;
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

bool is_success(result_t result) {
    if (result == RESULT_SUCCESS) return true;
    return false;
}

bool is_failure(result_t result) {
    if (result == RESULT_FAILURE) return true;
    return false;
}

bool is_valid_object(result_t result) {
    if (result == RESULT_VALID_OBJECT) return true;
    return false;
}

bool is_error(result_t result) {
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

bool is_true(result_t result) {
    if (result == RESULT_TRUE) return true;
    return false;
}

bool is_false(result_t result) {
    if (result == RESULT_FALSE) return true;
    return false;
}

result_t lurk_set_result_config(result_config_t* config) {
    result_config = config;
    return RESULT_SUCCESS;
}

result_t lurk_get_defaults(result_config_t* config) {
    if (config == NULL) return RETURN_BAD_PARAM_NULL(config);
    *config = result_config_default;

    return RESULT_SUCCESS;
}

result_t lurk_log(result_t result, const char* fmt, ...) {
    if (fmt == NULL) return result;

    if (!get_config_do_log()) return result;

    result_log_fn* log_fn = get_config_log_fn();

    va_list args;

    va_start(args, fmt);

    (*log_fn)(result, fmt, args);

    va_end(args);

    return result;
}

result_t lurk_err(result_t result, const char* caller, const char* loc, const char* fmt, ...) {
    if (fmt == NULL) return result;

    if (!get_config_do_err()) return result;

    result_err_fn* err_fn = get_config_err_fn();

    va_list args;

    va_start(args, fmt);

    (*err_fn)(result, caller, loc, fmt, args);

    va_end(args);

    return result;
}

void log_default(result_t result, const char* restrict fmt, va_list args) {
    if (fmt == NULL) return;

    if (!get_config_do_log()) return;

    const char* projname = get_config_projname();
    const char* prefix = get_config_prefix();
    const char* postfix = get_config_postfix();

    struct tm t = get_time();

    int n;

    n = fprintf(stdout, "%02d:%02d:%02d  %08x  [%s]  %s",
                    t.tm_hour, t.tm_min, t.tm_sec, result, projname, prefix);

    // this *shouldn't* ever happen, but can't be too careful (if it does, the calling program
    // should be able to intercept the abort signal if desired)
    if (n < 0) abort();

    n = vfprintf(stdout, fmt, args);
    if (n < 0) abort();

    n = fprintf(stdout, "%s", postfix);
    if (n < 0) abort();
}

void err_default(result_t result, const char* caller, const char* loc, const char* restrict fmt, va_list args) {
    if (fmt == NULL) return;

    if (!get_config_do_err()) return;

    if (caller == NULL) caller = "(unknown)";
    if (loc == NULL) loc = "???";

    struct tm t = get_time();

    const char* projname = get_config_projname();
    const char* prefix = get_config_prefix();
    const char* postfix = get_config_postfix();

    int n = fprintf(stderr, "%02d:%02d:%02d  %08x  [%s:%s.%s]  %s",
                    t.tm_hour, t.tm_min, t.tm_sec, result, projname, caller, loc, prefix);

    if (n < 0) abort(); // this *shouldn't* ever happen, but just in case

    n = vfprintf(stderr, fmt, args);
    if (n < 0) abort();

    n = fprintf(stderr, "%s", postfix);
    if (n < 0) abort();
}
