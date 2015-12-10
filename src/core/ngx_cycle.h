
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CYCLE_H_INCLUDED_
#define _NGX_CYCLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef NGX_CYCLE_POOL_SIZE
#define NGX_CYCLE_POOL_SIZE     NGX_DEFAULT_POOL_SIZE
#endif


#define NGX_DEBUG_POINTS_STOP   1
#define NGX_DEBUG_POINTS_ABORT  2


typedef struct ngx_shm_zone_s  ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

struct ngx_shm_zone_s 
{
    void                     *data;
    ngx_shm_t                 shm;
    ngx_shm_zone_init_pt      init;
    void                     *tag;
    ngx_uint_t                noreuse;  /* unsigned  noreuse:1; */
};


struct ngx_cycle_s 
{
    void                  ****conf_ctx;   		//NGX_CORE_MODULE类型的模块的上下文
    ngx_pool_t               *pool;

    ngx_log_t                *log;
    ngx_log_t                 new_log;

    ngx_uint_t                log_use_stderr;  		/* unsigned  log_use_stderr:1; */

    ngx_connection_t        **files;				/*连接文件*/
    ngx_connection_t         *free_connections;		/*空闲连接链表表头*/	
    ngx_uint_t                free_connection_n;	/*空闲连接链表结点数量*/

    ngx_queue_t               reusable_connections_queue; 	/*复用连接队列*/

    ngx_array_t               listening;		/*ngx_listening_t类型的数组*/
    ngx_array_t               paths;			/*ngx_path_t*类型的数组*/
    ngx_array_t               config_dump;		/*ngx_conf_dump_t类型的数组*/
    ngx_list_t                open_files;		/*ngx_open_file_t 类型的链表*/
    ngx_list_t                shared_memory;    /*ngx_shm_zone_t 类型的链表*/

    ngx_uint_t                connection_n;		/*connections数组的大小, 每个工作进程的连接最大数*/	
    ngx_uint_t                files_n;

    ngx_connection_t         *connections;     	/*指向所分配的所有的connection*/
    ngx_event_t              *read_events;
    ngx_event_t              *write_events;

    ngx_cycle_t              *old_cycle;

    ngx_str_t                 conf_file;			/*nginx.conf配置文件路径*/
    ngx_str_t                 conf_param;			/*存储启动参数-g设置的值�??*/
    ngx_str_t                 conf_prefix;			/*配置路径前缀*/
    ngx_str_t                 prefix;				/*前缀路径*/
    ngx_str_t                 lock_file;
    ngx_str_t                 hostname;
};


typedef struct 
{
     ngx_flag_t               daemon;
     ngx_flag_t               master;

     ngx_msec_t               timer_resolution;

     ngx_int_t                worker_processes;		/*工作进程的个数*/
     ngx_int_t                debug_points;

     ngx_int_t                rlimit_nofile;  		/*最大描述符个数限制??*/
     off_t                    rlimit_core;			/*CoreDump文件大小*/

     int                      priority;

     ngx_uint_t               cpu_affinity_n;	/*cpu_affinity数组个数*/
     uint64_t                *cpu_affinity;  	/*uint64_t类型的数组，每个元素表示一个工作进程的CPU亲和性掩码*/

     char                    *username;  	/*用户名*/
     ngx_uid_t                user;			/*用户uid*/
     ngx_gid_t                group;		/*用户gid*/

     ngx_str_t                working_directory;  /*指定进程当前工作目录*/
     ngx_str_t                lock_file;

     ngx_str_t                pid;
     ngx_str_t                oldpid;

     ngx_array_t              env;    		/*ngx_str_t类型的数组, 存储环境变量*/
     char                   **environment;
} ngx_core_conf_t;


#define ngx_is_init_cycle(cycle)  (cycle->conf_ctx == NULL)


ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
uint64_t ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);


extern volatile ngx_cycle_t  *ngx_cycle;
extern ngx_array_t            ngx_old_cycles;
extern ngx_module_t           ngx_core_module;
extern ngx_uint_t             ngx_test_config;
extern ngx_uint_t             ngx_dump_config;
extern ngx_uint_t             ngx_quiet_mode;


#endif /* _NGX_CYCLE_H_INCLUDED_ */
