// SPDX-License-Identifier: MIT
/**
 * @file task.h
 * @brief Header defining the data structures belonging to a single task and related functions.
 */
#ifndef __TASK_H__
#define __TASK_H__

#include "confparse.h"
#include "crinit-sdefs.h"
#include "envset.h"
#include "ioredir.h"

typedef unsigned long crinitTaskOpts_t;  ///< Type to store Task option bitmask.

#define CRINIT_TASK_OPT_RESPAWN (1 << 0)       ///< RESPAWN task option bitmask.
#define CRINIT_TASK_OPT_RESPAWN_DEFAULT false  ///< Default value for RESPAWN option

#define CRINIT_TASK_EVENT_RUNNING "spawn"  ///< Dependency event that fires when a task reaches the RUNNING state.
#define CRINIT_TASK_EVENT_DONE "wait"      ///< Dependency event that fires when a task reaches the DONE state.
#define CRINIT_TASK_EVENT_FAILED "fail"    ///< Dependency event that fires when a task reaches the FAILED state.
#define CRINIT_TASK_EVENT_NOTIFY_SUFFIX \
    "-notified"  ///< Dependency event suffix to indicate the event needs to be generated by sd-notify

typedef enum crinitTaskType_t { CRINIT_TASK_TYPE_STANDARD, CRINIT_TASK_TYPE_INCLUDE } crinitTaskType_t;

/**
 * Type to store a single command within a task.
 */
typedef struct crinitTaskCmd_t {
    int argc;     ///< Number of arguments within argv.
    char **argv;  ///< String array containing the program arguments, argv[0] contains absolute path to executable.
} crinitTaskCmd_t;

/**
 * Type to store a single dependency within a task.
 */
typedef struct crinitTaskDep_t {
    char *name;   ///< Dependency name.
    char *event;  ///< Dependency event.
} crinitTaskDep_t;

/**
 * Type to store a single provided feature within a task.
 */
typedef struct crinitTaskPrv_t {
    char *name;                  ///< Name of the provided feature.
    crinitTaskState_t stateReq;  ///< The task state required to be reached to provide the feature.
} crinitTaskPrv_t;

/**
 * Type to store a single task.
 */
typedef struct crinitTask_t {
    char *name;                  ///< Name of the task, corresponds to NAME in the config file.
    crinitTaskCmd_t *cmds;       ///< Dynamic array of commands, corresponds to COMMAND[N] in the config file.
    size_t cmdsSize;             ///< Number of commands in cmds array.
    crinitEnvSet_t taskEnv;      ///< Environment variables valid for each COMMAND in this task.
    crinitEnvSet_t elosFilters;  ///< Elos filter definitions valid for use in dependencies of this task.
    crinitTaskDep_t *deps;       ///< Dynamic array of dependencies, corresponds to DEPENDS in the config file.
    size_t depsSize;             ///< Number of dependencies in deps array.
    crinitTaskPrv_t *prv;        ///< Dynamic array of provided features, corresponds to PROVIDES in the config file.
    size_t prvSize;              ///< Number of provided features in prv array.
    crinitTaskOpts_t opts;       ///< Task options.
    crinitTaskState_t state;     ///< Task state.
    pid_t pid;                   ///< PID of currently running process subordinate to the task, if any.
    crinitIoRedir_t *redirs;     ///< IO redirection descriptions.
    size_t redirsSize;           ///< Number of IO redirections.
    int maxRetries;              ///< If crinitTask_t::opts includes #CRINIT_TASK_OPT_RESPAWN, this variable specifies a
                                 ///< maximum consecutive number of respawns after failure (default: -1 for infinite).
    int failCount;  ///< Counts consecutive respawns after failure (see crinitTaskOpts_t::maxRetries). Resets
                    ///< on a successful completion (i.e. all COMMANDs in the task have returned 0).
} crinitTask_t;

/**
 * Given a crinitTask_t, iterate over all dependencies of this task.
 *
 * @param task  Pointer to the task.
 * @param dep   Pointer to the current dependency.
 */
#define crinitTaskForEachDep(task, dep) for ((dep) = (task)->deps; (dep) != (task)->deps + (task)->depsSize; (dep)++)

/**
 * Given an crinitConfKvList_t created from a task config, build an equivalent crinitTask.
 *
 * The crinitTask returned via \a out is dynamically allocated and should be freed using crinitFreeTask if no longer
 * needed.
 *
 * @param out  The return pointer for the newly created task.
 * @param in   The crinitConfKvList_t from which to build the task.
 *
 * @return 0 on success, -1 on error
 */
int crinitTaskCreateFromConfKvList(crinitTask_t **out, const crinitConfKvList_t *in);

/**
 * Frees memory associated with an crinitTask created by crinitTaskCreateFromConfKvList() or crinitTaskDup().
 *
 * Uses crinitDestroyTask() internally and then frees the given pointer.
 *
 * @param t  Pointer to the crinitTask to free.
 */
void crinitFreeTask(crinitTask_t *t);

/**
 * Frees memory for internal members of an crinitTask_t.
 *
 * @param t  The task whose members shall be freed.
 */
void crinitDestroyTask(crinitTask_t *t);

/**
 *  Duplicates an crinitTask.
 *
 *  The copy returned via \a out is dynamically allocated and should be freed using crinitFreeTask() if no longer
 * needed.
 *
 *  @param out   Double pointer to return a dynamically allocated copy of \a orig.
 *  @param orig  The original task to copy.
 *
 *  @return 0 on success, -1 on error
 */
int crinitTaskDup(crinitTask_t **out, const crinitTask_t *orig);

/**
 *  Copies the conents from one task to another.
 *
 *  @param out   Pointer to copy of \a orig.
 *  @param orig  The original task to copy.
 *
 *  @return 0 on success, -1 on error
 */
int crinitTaskCopy(crinitTask_t *out, const crinitTask_t *orig);

/**
 * Merges the options set in a given include file into the target crinitTask_t.
 *
 * Uses the same parser handlers as regular task files but will fail if configuration options are encountered where
 * crinitConfigMapping_t::includeSafe is `false`. Optionally a comma-separated importList can be specified. If
 * given, only the configuration options in the list will be merged. If not given, everything in the include file
 * will be imported.
 *
 * @param tgt         The target crinitTask_t which will be modified.
 * @param src         Name of the include file to parse/merge (filename without leading path and ending).
 * @param importList  A comma-separated list which configs to merge from the include file. `NULL` means everything.
 *
 * @return  0 on success, -1 on failure
 */
int crinitTaskMergeInclude(crinitTask_t *tgt, const char *src, char *importList);

#endif /* __TASK_H__ */
