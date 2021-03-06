/*
   Copyright (C) 2014  Statoil ASA, Norway. 
    
   The file 'run_arg.c' is part of ERT - Ensemble based Reservoir Tool. 
    
   ERT is free software: you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by 
   the Free Software Foundation, either version 3 of the License, or 
   (at your option) any later version. 
    
   ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.   
    
   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
   for more details. 
*/


#include <ert/util/path_fmt.h>
#include <ert/util/subst_list.h>
#include <ert/util/type_macros.h>

#include <ert/enkf/enkf_types.h>
#include <ert/enkf/run_arg.h>
#include <ert/enkf/enkf_fs.h>


#define RUN_ARG_TYPE_ID 66143287



struct run_arg_struct {
  UTIL_TYPE_ID_DECLARATION;
  bool                    __ready;              /* An attempt to check the internal state - not really used. */
  int                     iens;
  bool                    active;               /* Is this state object active at all - used for instance in ensemble experiments where only some of the members are integrated. */
  int                     init_step_parameters; /* The report step we initialize parameters from - will often be equal to step1, but can be different. */
  state_enum              init_state_parameter; /* Whether we should init from a forecast or an analyzed state - parameters. */
  state_enum              init_state_dynamic;   /* Whether we should init from a forecast or an analyzed state - dynamic state variables. */
  int                     max_internal_submit;  /* How many times the enkf_state object should try to resubmit when the queueu has said everything is OK - but the load fails. */  
  int                     num_internal_submit;   
  int                     load_start;           /* When loading back results - start at this step. */
  int                     step1;                /* The forward model is integrated: step1 -> step2 */
  int                     step2;
  int                     iter;
  char                  * run_path;             /* The currently used  runpath - is realloced / freed for every step. */
  run_mode_type           run_mode;             /* What type of run this is */
  int                     queue_index;          /* The job will in general have a different index in the queue than the iens number. */
  
  enkf_fs_type          * init_fs;
  enkf_fs_type          * result_fs;
  enkf_fs_type          * update_target_fs;

  /******************************************************************/
  /* Return value - set by the called routine!!  */
  run_status_type         run_status;
};
  

UTIL_SAFE_CAST_FUNCTION( run_arg , RUN_ARG_TYPE_ID )
UTIL_IS_INSTANCE_FUNCTION( run_arg , RUN_ARG_TYPE_ID )


static run_arg_type * run_arg_alloc(enkf_fs_type * init_fs , 
                                    enkf_fs_type * result_fs , 
                                    enkf_fs_type * update_target_fs , 
                                    int iens , 
                                    run_mode_type run_mode          , 
                                    int init_step_parameters        ,      
                                    state_enum init_state_parameter ,
                                    state_enum init_state_dynamic   ,
                                    int step1                       , 
                                    int step2                       ,
                                    int iter                        ,
                                    const char * runpath) {
  
  run_arg_type * run_arg = util_malloc(sizeof * run_arg );
  UTIL_TYPE_ID_INIT(run_arg , RUN_ARG_TYPE_ID);
  
  run_arg->init_fs = init_fs;
  run_arg->result_fs = result_fs;
  run_arg->update_target_fs = update_target_fs;

  run_arg->iens = iens;
  run_arg->run_mode = run_mode;
  run_arg->init_step_parameters = init_step_parameters;
  run_arg->init_state_parameter = init_state_parameter;
  run_arg->init_state_dynamic = init_state_dynamic;
  run_arg->step1 = step1;
  run_arg->step2 = step2;
  run_arg->iter = iter;
  run_arg->run_path = util_alloc_abs_path( runpath );
  run_arg->num_internal_submit = 0;

  if (step1 == 0)
    run_arg->load_start = 1;
  else
    run_arg->load_start = step1;
  
  return run_arg;
}


run_arg_type * run_arg_alloc_ENKF_ASSIMILATION(enkf_fs_type * fs , 
                                               int iens , 
                                               state_enum init_state_parameter ,
                                               state_enum init_state_dynamic   ,
                                               int step1                       , 
                                               int step2                       ,
                                               const char * runpath) {
  
  return run_arg_alloc(fs,fs,fs,iens,ENKF_ASSIMILATION,step1 , init_state_parameter, init_state_dynamic , step1 , step2 , 0 , runpath);
}



run_arg_type * run_arg_alloc_ENSEMBLE_EXPERIMENT(enkf_fs_type * fs , int iens , int iter , const char * runpath) {
  return run_arg_alloc(fs , fs , NULL , iens , ENSEMBLE_EXPERIMENT , 0 , ANALYZED , ANALYZED , 0 , 0 , iter , runpath);
}


run_arg_type * run_arg_alloc_INIT_ONLY(enkf_fs_type * init_fs , int iens , int iter , const char * runpath) {
  return run_arg_alloc(init_fs , NULL , NULL , iens , INIT_ONLY , 0 , ANALYZED , ANALYZED , 0 , 0 , iter , runpath);
}


run_arg_type * run_arg_alloc_SMOOTHER_RUN(enkf_fs_type * simulate_fs , enkf_fs_type * update_target_fs , int iens , int iter , const char * runpath) {
  return run_arg_alloc(simulate_fs , simulate_fs , update_target_fs , iens , ENSEMBLE_EXPERIMENT , 0 , ANALYZED , ANALYZED , 0 , 0 , iter , runpath);
}



void run_arg_free(run_arg_type * run_arg) {
  util_safe_free(run_arg->run_path);
  free(run_arg);
}


void run_arg_free__(void * arg) {
  run_arg_type * run_arg = run_arg_safe_cast( arg );
  run_arg_free( run_arg );
}


void run_arg_complete_run(run_arg_type * run_arg) {
  if (run_arg->run_status == JOB_RUN_OK) {
    util_safe_free(run_arg->run_path);
    run_arg->run_path = NULL;
  }
}



void run_arg_increase_submit_count( run_arg_type * run_arg ) {
  run_arg->num_internal_submit++;
}


void run_arg_set_queue_index( run_arg_type * run_arg , int queue_index) {
  run_arg->queue_index = queue_index;
}



const char * run_arg_get_runpath( const run_arg_type * run_arg) {
  return run_arg->run_path;
}




int run_arg_get_iter( const run_arg_type * run_arg ) {
  return run_arg->iter;
}


int run_arg_get_iens( const run_arg_type * run_arg ) {
  return run_arg->iens;
}


int run_arg_get_load_start( const run_arg_type * run_arg ) {
  return run_arg->load_start;
}


bool run_arg_is_ready( const run_arg_type * run_arg) {
  return run_arg->__ready;
}


void run_arg_set_ready( run_arg_type * run_arg , bool ready) {
  run_arg->__ready = ready;
}


int run_arg_get_step2( const run_arg_type * run_arg ) {
  return run_arg->step2;
}

bool run_arg_can_retry( const run_arg_type * run_arg ) {
  if (run_arg->num_internal_submit < run_arg->max_internal_submit)
    return true;
  else
    return false;
}


int run_arg_get_step1( const run_arg_type * run_arg ) {
  return run_arg->step1;
}


run_mode_type run_arg_get_run_mode( const run_arg_type * run_arg ) {
  return run_arg->run_mode;
}

state_enum run_arg_get_dynamic_init_state( const run_arg_type * run_arg ) {
  return run_arg->init_state_dynamic;
}


state_enum run_arg_get_parameter_init_state( const run_arg_type * run_arg ) {
  return run_arg->init_state_parameter;
}



int run_arg_get_parameter_init_step( const run_arg_type * run_arg ) {
  return run_arg->init_step_parameters;
}



void run_arg_set_inactive( run_arg_type * run_arg ) {
  run_arg->active = false;
}


int run_arg_get_queue_index( const run_arg_type * run_arg ) {
  return run_arg->queue_index;
}


run_status_type run_arg_get_run_status( const run_arg_type * run_arg) {
  return run_arg->run_status;
}


void run_arg_set_run_status( run_arg_type * run_arg , run_status_type run_status) {
  run_arg->run_status = run_status;
}



enkf_fs_type * run_arg_get_init_fs(const run_arg_type * run_arg) {
  if (run_arg->init_fs)
    return run_arg->init_fs;
  else {
    util_abort("%s: internal error - tried to access run_arg->init_fs when init_fs == NULL\n",__func__);
    return NULL;
  }
}


enkf_fs_type * run_arg_get_result_fs(const run_arg_type * run_arg) {
  if (run_arg->result_fs)
    return run_arg->result_fs;
  else {
    util_abort("%s: internal error - tried to access run_arg->result_fs when result_fs == NULL\n",__func__);
    return NULL;
  }
}


enkf_fs_type * run_arg_get_update_target_fs(const run_arg_type * run_arg) {
  if (run_arg->update_target_fs)
    return run_arg->update_target_fs;
  else {
    util_abort("%s: internal error - tried to access run_arg->update_target_fs when update_target_fs == NULL\n",__func__);
    return NULL;
  }
}
