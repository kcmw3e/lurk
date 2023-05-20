// license
// ---------------------------------------------------------------------------------------------- //
// Copyright (c) 2023, Casey Walker
// All rights reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//
//
// result.h
// ---------------------------------------------------------------------------------------------- //
// This file defines the result enum which contains many common results (errors, successes, etc.),
// and some useful functions to categorize and log the results. It also contains helpful macros for
// outputting an error message while returning an error result.


#ifndef LURK_RESULT_H
#define LURK_RESULT_H

#include <stdbool.h>
#include <stdarg.h>


// These macros can be used to log a simple error to [stderr]. They use the function [return_error]
// defined below, and include the function name in the error message. They can be bypassed by
// defining [LURK_NO_CALL_RETURN_ERROR] if a programmer wishes to avoid *all* potential logging
// overhead (see below about configuration how logging can be dynamically controlled instead).
// ---------------------------------------------------------------------------------------------- //
#ifndef LURK_NO_CALL_RETURN_ERROR
#   define LURK_ERROR_CALL lurk_err
#else
#   define LURK_ERROR_CALL(result, ...) result
#endif

#define RETURN_TRACE_ERROR(result)                                                                 \
    LURK_ERROR_CALL(result, __func__, LURK_LINE_STRING, "Callback trace.")

#define RETURN_PASS_ERROR(result, pass)                                                            \
    LURK_ERROR_CALL(result, __func__, LURK_LINE_STRING, "Callback trace, passing: [%08x].", pass)

#define RETURN_BAD_PARAM(param)                                                                    \
    LURK_ERROR_CALL(RESULT_BAD_PARAM, __func__, LURK_LINE_STRING, "Bad parameter ["#param"].")

#define RETURN_BAD_PARAM_NULL(param)                                                               \
    LURK_ERROR_CALL(RESULT_BAD_PARAM, __func__, LURK_LINE_STRING,                                  \
             "Bad parameter ["#param"]. Must not be [NULL]")

#define RETURN_INVALID_OBJECT(obj)                                                                 \
    LURK_ERROR_CALL(RESULT_INVALID_OBJECT, __func__, LURK_LINE_STRING,                             \
            "Invalid object ["#obj"].")

#define RETURN_INVALID_OBJECT_MEMBER(obj, member)                                                  \
    LURK_ERROR_CALL(RESULT_INVALID_OBJECT, __func__, LURK_LINE_STRING,                             \
             "Invalid object member ["#obj"."#member"].")

#define RETURN_INVALID_OBJECT_MEMBERS(obj, ...)                                                    \
    LURK_ERROR_CALL(RESULT_INVALID_OBJECT, __func__, LURK_LINE_STRING,                             \
             "Invalid object member ["#obj".("#__VA_ARGS__")].")

#define RETURN_INTERNAL_ERROR()                                                                    \
    LURK_ERROR_CALL(RESULT_INTERNAL_ERROR, __func__, LURK_LINE_STRING, "Internal error.")

#define RETURN_ERROR(result, err)                                                                  \
    LURK_ERROR_CALL(result, __func__, LURK_LINE_STRING, err)

#define RETURN_ERROR_FMT(result, err, ...)                                                         \
    LURK_ERROR_CALL(result, __func__, LURK_LINE_STRING, err, __VA_ARGS__)


// These macros can be used to replicate repetitive guards that may be omitted for performance in
// some cases but desired for debugging during development.
// ---------------------------------------------------------------------------------------------- //
#ifndef LURK_NO_CALL_VALIDATE_OBJECT
#   define VALIDATE_OBJECT(fn, obj)                                                                \
        if (!is_valid_object(fn(obj))) return RETURN_INVALID_OBJECT(obj)

#   define VALIDATE_OBJECT_MEMBER(fn, obj, memb)                                                   \
        if (!is_valid_object(fn(memb))) return RETURN_INVALID_OBJECT_MEMBER(obj, memb)
#else
#   define VALIDATE_OBJECT(fn, obj)
#   define VALIDATE_OBJECT_MEMBER(fn, obj, memb)
#endif // LURK_NO_CALL_VALIDATE_OBJECT

#ifndef LURK_NO_CHECK_NULL_GUARD
#   define NULL_GUARD(ptr) if (ptr == NULL) return RETURN_BAD_PARAM_NULL(ptr);
#else
#   define NULL_GUARD(ptr)
#endif // LURK_NO_CHECK_NULL_GUARD


// The result enum defines many common results. They are split into to four categories: error,
// success, boolean, and status. Errors are always negative, statuses are always positive, and
// success is always zero. Note that a *failure* is not the same as an error. Errors are strictly
// for indicating when some unexpected behavior occurs or an invalid value arises.
// ---------------------------------------------------------------------------------------------- //
// [enum result]
//  [RESULT_INVALID_OBJECT]
//      * this error is useful for when a program encounters an object (typically a struct) that is
//        invalid and wishes to notify a client or calling program
//  [RESULT_INTERNAL_ERROR]
//      * this error is useful when programs encounter bugs or unexpected behavior internally and
//        either wish to signal to itself or a calling program that some unknown error has happened
//  [RESULT_BAD_PARAM]
//      * this error is useful to notify a calling program that a parameter it has passed to a
//        function is invalid in some way
//  [RESULT_SUCCESS]
//      * generally this indicates the successful execution of a function, notifying the caller
//        that it can move on with confidence
//  [RESULT_FAILURE]
//      * this status typically reports a failure, but not an error
//          * for example, if a library implements a queue and a calling program attempts to dequeue
//            from an empty queue, this might be the returned result
//  [RESULT_DONE]
//      * this status is useful for iterators or ongoing processes and can be used to indicate that
//        the job is done
//  [RESULT_VALID_OBJECT]
//      * this status result is equivalent to [RESULT_SUCCESS] and is typically used alongside
//        alongside [RESULT_INVALID_OBJECT] internally for verifying the validity of objects
//  [RESULT_TRUE]
//      * this boolean result is merely an alias for [true] defined in [stdbool.h] to provide
//        flexibility in boolean functions, allowing them to also return errors
//      * great care should be taken when using either [RESULT_TRUE] or [RESULT_FALSE], as it can be
//        easy to associate [RESULT_TRUE] as being equivalent to [RESULT_SUCCESS] and
//        [RESULT_FALSE] with [RESULT_FAILURE] when they are actually different
//          * any function using these should *always* specify its possible returns in documentation
//            somewhere to avoid this confusion
//          * boolean results should never be combined with other status or success results
//      * note that a special type definition, [boolresult_t], is provided as an alias to [result_t]
//        for programmers to self-document the function's return as a boolean result
//  [RESULT_FALSE]
//      * this boolean result is merely an alias for [false] defined in [stdbool.h] to provide
//        flexibilty in boolean functions, allowing them to also return errors
//      * see the documentation for [RESULT_TRUE] for an extra warning and best practices regarding
//        the usage of boolean results
enum result {
    RESULT_INVALID_OBJECT = -2,
    RESULT_INTERNAL_ERROR = -3,
    RESULT_BAD_PARAM      = -1,

    RESULT_SUCCESS = 0,
    RESULT_FAILURE = 1,
    RESULT_DONE = 2,

    RESULT_VALID_OBJECT = RESULT_SUCCESS,

    RESULT_TRUE  = true,
    RESULT_FALSE = false,
};


typedef int result_t;
typedef int boolresult_t;


// There is a small set of configuration available to specify how and when results should be logged.
// Programmers can also specify their own logging functions if desired.
// ---------------------------------------------------------------------------------------------- //
// [result_log_fn]
//  * defines a type of function for general logging of results, especially useful for diagnostics
//    and update information
//  * see source for default, which logs to [stdout]
//  == Parameters ==
//      [result]
//          * the result that is being logged
//      [fmt]
//          * the format string, formatted like printf-style strings
//          * lurk functions calling a [result_log_fn] function will never pass [NULL]
//      [args]
//          * a variable list of args to fit with [fmt] for printf-style printing
// [result_err_fn]
//  * defines a type of function for logging errors, especially useful for debugging and logging
//    errors before a crash
//  * see source for the default, which logs to [stderr]
//  == Parameters ==
//      [result]
//          * the result that is being logged
//      [caller]
//          * a string to specify the calling function
//          * may be [NULL] if the the caller of [lurk_log_err] passes [NULL]
//      [loc]
//          * a string to specify the location in the calling function where the error is found
//          * may be [NULL] if the caller of [lurk_log_err] passes [NULL]
//      [fmt]
//          * the format string, formatted like printf-style strings
//          * lurk functions calling a [result_err_fn] function will never pass [NULL]
//      [args]
//          * a variable list of args to fit with [fmt] for printf-style printing
// Notes
//  * if using [lurk_log] and [lurk_err], it is up to these functions to output a new line after the
//    logging (unless [fmt] contains the new line, where it is up to the programmer in that case)
//      * the default log and err functions automatically add a new line if [result_config.postfix]
//        is left default
typedef void result_log_fn(result_t result, const char* fmt, va_list args);
typedef void result_err_fn(result_t result,
                               const char* caller, const char* loc,
                               const char* fmt, va_list args);

// [struct result_config]
//  [.projname]
//      * specifies the project name to be logged; the default is ["lurk"], so any program wishing
//        to tag its logs with a certain tag or project name should add this to the config
//      * if a program is defining its config but doesn't wish to specify a project name, it may
//        leave this field as [NULL] or, if it wishes there to be no tag at all, simply [""]
//  [.prefix]
//      * specifies the prefix string to log before printing the rest of the error message
//          * this does not prefix the time, result printout, or calling location
//  [.postfix]
//      * specifies the postfix string to log after all printing is done
//          * by default this is ["\n"] so that log calls print newlines
//  [.do_log]
//      * determines whether calling [lurk_log] actually writes to [stdout] or not
//      * it may be changed at any time and is useful when only certain logs are desired
//  [.do_err]
//      * determines whether calling [lurk_err] actually writes to [stderr] or not
//      * it may be changed at any time and is useful when only certain errors are desired
//  [.log_fn]
//      * a pointer to a [result_log_fn] function
//      * if this field is not [NULL], calling [lurk_log] will in turn call the function pointed to
//        and bypass calling the default logging function
//  [.err_fn]
//      * a pointer to a [result_err_fn] function
//      * if this field is not [NULL], calling [lurk_err] will in turn call the function pointed to
//        and bypass calling the default error logging function
struct result_config {
    const char* projname;
    const char* prefix;
    const char* postfix;
    bool do_log;
    bool do_err;
    result_log_fn* log_fn;
    result_err_fn* err_fn;
};

typedef struct result_config result_config_t;


// The interface for using the lurk library is below.
// ---------------------------------------------------------------------------------------------- //
// [is_success]
//  * determines if a result is [RESULT_SUCCESS] or not
//  == Parameters ==
//      [result]
//          * the result to check
//          * may be any integer that fits in the underlying type of [result_t]
//  ==   Return   ==
//      [true]
//          * if [result] is exactly [RESULT_SUCCESS]
//          * note that if [RESULT_FALSE] or any value equivalent to [RESULT_SUCCESS] is passed in,
//            there is no way to distinguish between them
//              * as such, a programmer should take great care in deciding what results from
//                functions to pass as [result]
//              * similarly, any library implementing its own results should take care in choosing
//                values for its results, keeping in mind this behavior
//      [false]
//          * otherwise
// [is_valid_object]
//  * determines if a result is [RESULT_VALID_OBJECT] or not
//  == Parameters ==
//      [result]
//          * the result to check
//          * may be any integer that fits in the underlying type of [result_t]
//  ==   Return   ==
//      [true]
//          * if [result] is exactly [RESULT_VALID_OBJECT]
//          * note similar behavior to [is_success]
//      [false]
//          * otherwise
// [is_err]
//  * determines whether a result is an error or not; alternatively, a programmer may also use the
//    definition of a result as being always negative, or strictly less than zero
//  == Parameters ==
//      [result]
//          * any integer that fits in the underlying type of [result_t]
//  ==   Return   ==
//      [true]
//          * when [result] is an error (strictly less than zero)
//      [false]
//          * otherwise
// [is_lurk_err]
//  * determines whether a result is a defined lurk error  in [enum result] or not
//  == Parameters ==
//      [result]
//          * any integer that fits in the underlying type of [result_t]
//  ==   Return   ==
//      [true]
//          * only if [result] is one of the errors defined in [enum result]
//      [false]
//          * otherwise, including if [result] is negative but not one of the defined errors
// [is_true]
//  * determines whether a result is [RESULT_TRUE]
//  == Parameters ==
//      [result]
//          * any integer that fits in the underlying type of [result_t]
//  ==   Return   ==
//      [true]
//          * only if [result] is exactly [RESULT_TRUE]
//      [false]
//          * otherwise
// [is_false]
//  * determines whether a result is [RESULT_FALSE]
//  == Parameters ==
//      [result]
//          * any integer that fits in the underlying type of [result_t]
//  ==   Return   ==
//      [true]
//          * only if [result] is exactly [RESULT_FALSE]
//      [false]
//          * otherwise
bool is_success(result_t result);
bool is_valid_object(result_t result);
bool is_error(result_t result);
bool is_lurk_err(result_t result);
bool is_true(result_t result);
bool is_false(result_t result);

// [lurk_set_result_config]
//  * set the config struct to something non-default; see the documentation above for how fields can
//    be set to pull from the default, or alternatively first use [lurk_get_defaults] to populate
//    the config, then fill in only the fields that need to be non-default
//  == Parameters ==
//      [config]
//          * a pointer to the config struct to pull config options from
//  ==   Return   ==
//      [RESULT_SUCCESS]
//          * always
// [lurk_get_defalts]
//  * populate a result config struct with the lurk defaults
//  == Parameters ==
//      [config]
//          * a pointer to the config struct to populate; must not be [NULL]
//  ==   Return   ==
//      [RESULT_SUCCESS]
//          * if [config] was not [NULL] and could be successfully populated
//      [RESULT_BAD_PARAM]
//          * if [config] was [NULL]
// [lurk_log]
//  * logs a result with the active [result_log_fn] unless the current config has
//    [result_config.do_log] set to [false]
//  * note that the default log function automatically outputs a new line after printing [fmt]
//  == Parameters ==
//      [result]
//          * the result that is being logged
//      [fmt]
//          * the format string for printf-like printing; must not be [NULL]
//      [...]
//          * the rest of the arguments for printing in the format string [fmt]; must match the
//            specifiers in [fmt] like in printf-like functions
//  ==   Return   ==
//      [result]
//          * will always return the result passed to it
// [lurk_err]
//  * logs an error result with the active [result_err_fn] unless the current config has
//    [result_config.do_err] set to [false]
//  * note that the default error function automatically outputs a new line after printing [fmt]
//  == Parameters ==
//      [result]
//          * the result that is being logged
//      [caller]
//          * a string specifying the calling function
//              * note that this is automatically populated when using the macros above
//          * may be [NULL]
//              * see source for how this is handled in the default error logging function
//      [loc]
//          * a string specifying the location within the calling function where the error is found
//              * note that this is automatically poulated as the line number using the macros above
//          * may be [NULL]
//              * see source for how this is handled in the default error logging function
//      [fmt]
//          * the format string for printf-like printing; must not be [NULL]
//      [...]
//          * the rest of the arguments for printing in the format string [fmt]; must match the
//            specifiers in [fmt] like in printf-like functions
//  ==   Return   ==
//      [result]
//          * will always return the result passed to it
result_t lurk_set_result_config(result_config_t* config);
result_t lurk_get_defaults(result_config_t* config);
result_t lurk_log(result_t result, const char* fmt, ...);
result_t lurk_err(result_t result, const char* caller, const char* loc, const char* fmt, ...);

#endif // LURK_RESULT_H
