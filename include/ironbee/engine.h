/*****************************************************************************
 * Licensed to Qualys, Inc. (QUALYS) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * QUALYS licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef _IB_ENGINE_H_
#define _IB_ENGINE_H_

/**
 * @file
 * @brief IronBee
 *
 * @author Brian Rectanus <brectanus@qualys.com>
 */

#include <sys/socket.h>

#include <ironbee/build.h>
#include <ironbee/release.h>
#include <ironbee/util.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup IronBee IronBee
 * @{
 * @defgroup IronBeeEngine Engine
 *
 * This is the API for the IronBee engine.
 *
 * @{
 */

/** Engine Handle */
typedef struct ib_engine_t ib_engine_t;

/** Module Structure */
typedef struct ib_module_t ib_module_t;

/** Provider Definition */
typedef struct ib_provider_def_t ib_provider_def_t;
/** Provider */
typedef struct ib_provider_t ib_provider_t;
/** Provider Instance */
typedef struct ib_provider_inst_t ib_provider_inst_t;

/** Configuration Context. */
typedef struct ib_context_t ib_context_t;

/* Public type declarations */
typedef struct ib_conn_t ib_conn_t;
typedef struct ib_conndata_t ib_conndata_t;
typedef struct ib_txdata_t ib_txdata_t;
typedef struct ib_tx_t ib_tx_t;
typedef struct ib_tfn_t ib_tfn_t;
typedef struct ib_logevent_t ib_logevent_t;

typedef enum {
    IB_TXDATA_HTTP_LINE,
    IB_TXDATA_HTTP_HEADER,
    IB_TXDATA_HTTP_BODY,
    IB_TXDATA_HTTP_TRAILER
} ib_txdata_type_t;

/* Connection Flags */
/// @todo Do we need anymore???
#define IB_CONN_FNONE           (0)
#define IB_CONN_FERROR          (1 << 0) /**< Connection had an error */
#define IB_CONN_FSEENTX         (1 << 1) /**< Connection had transaction */
#define IB_CONN_FSEENDATAIN     (1 << 2) /**< Connection had data in */
#define IB_CONN_FSEENDATAOUT    (1 << 3) /**< Connection had data out */
#define IB_CONN_FOPENED         (1 << 4) /**< Connection opened */
#define IB_CONN_FCLOSED         (1 << 5) /**< Connection closed */

/* Transaction Flags */
/// @todo Do we need anymore???
#define IB_TX_FNONE             (0)
#define IB_TX_FERROR            (1 << 0) /**< Transaction had an error */
#define IB_TX_FPIPELINED        (1 << 1) /**< Transaction is pipelined */
#define IB_TX_FSEENDATAIN       (1 << 2) /**< Transaction had data in */
#define IB_TX_FSEENDATAOUT      (1 << 3) /**< Transaction had data out */
#define IB_TX_FREQ_STARTED      (1 << 4) /**< Request started */
#define IB_TX_FREQ_SEENHEADERS  (1 << 5) /**< Request headers seen */
#define IB_TX_FREQ_NOBODY       (1 << 6) /**< Request should not have body */
#define IB_TX_FREQ_SEENBODY     (1 << 7) /**< Request body seen */
#define IB_TX_FREQ_FINISHED     (1 << 8) /**< Request finished */
#define IB_TX_FRES_STARTED      (1 << 9) /**< Response started */
#define IB_TX_FRES_SEENHEADERS  (1 << 0) /**< Response headers seen */
#define IB_TX_FRES_SEENBODY     (1 <<11) /**< Response body seen */
#define IB_TX_FRES_FINISHED     (1 <<12) /**< Response finished  */

/** Configuration Context Type */
/// @todo Perhaps "context scope" is better (CSCOPE)???
typedef enum {
    IB_CTYPE_ENGINE,
//    IB_CTYPE_SESS,
    IB_CTYPE_CONN,
    IB_CTYPE_TX,
//    IB_CTYPE_CUSTOM,
} ib_ctype_t;

/**
 * Configuration Context Function.
 *
 * This function returns IB_OK if the context should be used.
 *
 * @param ctx Configuration context
 * @param type Connection structure
 * @param ctxdata Context data (type dependent: conn or tx)
 * @param cbdata Callback data (fn_ctx_data from context)
 *
 * @returns Status code
 */
typedef ib_status_t (*ib_context_fn_t)(ib_context_t *ctx,
                                       ib_ctype_t type,
                                       void *ctxdata,
                                       void *cbdata);

/** Connection Structure */
struct ib_conn_t {
    ib_engine_t        *ib;              /**< Engine handle */
    ib_mpool_t         *mp;              /**< Connection memory pool */
    ib_context_t       *ctx;             /**< Config context */
    void               *pctx;            /**< Plugin context */
    ib_provider_inst_t *dpi;             /**< Data provider instance */
    ib_hash_t          *data;            /**< Generic data store */

    const char         *remote_ipstr;    /**< Remote IP as string */
//    struct sockaddr_storage remote_addr; /**< Remote address */
    int                 remote_port;     /**< Remote port */

    const char         *local_ipstr;     /**< Local IP as string */
//    struct sockaddr_storage local_addr;  /**< Local address */
    int                 local_port;      /**< Local port */

    ib_tx_t            *tx;              /**< Pending transaction(s) */
    ib_tx_t            *tx_last;         /**< Last transaction in the list */

    ib_flags_t          flags;           /**< Connection flags */
};

/** Connection Data Structure */
struct ib_conndata_t {
    ib_engine_t        *ib;              /**< Engine handle */
    ib_mpool_t         *mp;              /**< Data memory pool */
    ib_conn_t          *conn;            /**< Connection */
    size_t              dalloc;          /**< Data buffer allocated */
    size_t              dlen;            /**< Data buffer length */
    uint8_t            *data;            /**< Data buffer */
};

/** Transaction Data Structure */
struct ib_txdata_t {
    ib_engine_t        *ib;              /**< Engine handle */
    ib_mpool_t         *mp;              /**< Data memory pool */
    ib_tx_t            *tx;              /**< Transaction */
    ib_txdata_type_t    dtype;           /**< Transaction data type */
    size_t              dalloc;          /**< Data buffer allocated */
    size_t              dlen;            /**< Data buffer length */
    uint8_t            *data;            /**< Data buffer */
};

/** Transaction Structure */
struct ib_tx_t {
    ib_engine_t        *ib;              /**< Engine handle */
    ib_mpool_t         *mp;              /**< Transaction memory pool */
    ib_conn_t          *conn;            /**< Connection */
    ib_context_t       *ctx;             /**< Config context */
    void               *pctx;            /**< Plugin context */
    ib_provider_inst_t *dpi;             /**< Data provider instance */
    ib_hash_t          *data;            /**< Generic data store */
    ib_tx_t            *next;            /**< Next transaction */
    const char         *hostname;        /**< Hostname used in the request */
    const char         *path;            /**< Path used in the request */

    ib_flags_t          flags;           /**< Transaction flags */
};

/// @todo Maybe a ib_create_ex() that takes a config?

/**
 * Create an engine handle.
 *
 * After creating the engine, the caller must configure defaults, such as
 * initial logging parameters, and then call @ref ib_init() to initialize
 * the engine configuration context.
 *
 * @param pib Address which new handle is written
 * @param plugin Information on the server plugin instantiating the engine
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_engine_create(ib_engine_t **pib, void *plugin);

/**
 * Initialize the engine configuration context.
 *
 * @param ib Engine handle
 * @param plugin Information on the server plugin instantiating the engine
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_engine_init(ib_engine_t *ib);

/**
 * Get a module by name.
 *
 * @param ib Engine handle
 * @param name Module name
 * @param pm Address which module will be written
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_engine_module_get(ib_engine_t *ib,
                                            const char * name,
                                            ib_module_t **pm);

/**
 * Get the main engine memory pool.
 *
 * @param ib Engine handle
 *
 * @returns Memory pool
 */
ib_mpool_t DLL_PUBLIC *ib_engine_pool_main_get(ib_engine_t *ib);

/**
 * Get the engine configuration memory pool.
 *
 * This pool should be used to configure the engine.
 *
 * @param ib Engine handle
 *
 * @returns Memory pool
 */
ib_mpool_t DLL_PUBLIC *ib_engine_pool_config_get(ib_engine_t *ib);

/**
 * Get the engine temporary memory pool.
 *
 * This pool should be destroyed by the plugin after the
 * configuration phase. Therefore is should not be used for
 * anything except temporary allocations which are required
 * for performing configuration.
 *
 * @param ib Engine handle
 *
 * @returns Memory pool
 */
ib_mpool_t DLL_PUBLIC *ib_engine_pool_temp_get(ib_engine_t *ib);

/**
 * Destroy the engine temporary memory pool.
 *
 * This should be called by the plugin after configuration is
 * completed. After this call, any allocations in the temporary
 * pool will be invalid and no future allocations can be made to
 * to this pool.
 *
 * @param ib Engine handle
 */
void DLL_PUBLIC ib_engine_pool_temp_destroy(ib_engine_t *ib);

/**
 * Destroy an engine.
 *
 * @param ib Engine handle
 *
 * @returns Status code
 */
void DLL_PUBLIC ib_engine_destroy(ib_engine_t *ib);

/**
 * Create a configuration context.
 *
 * @param pctx Address which new context is written
 * @param ib Engine handle
 * @param fn_ctx Context function
 * @param fn_ctx_data Context function data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_create(ib_context_t **pctx,
                                         ib_engine_t *ib,
                                         ib_context_fn_t fn_ctx,
                                         void *fn_ctx_data);

/**
 * Create the main (default) configuration context.
 *
 * @param pctx Address which new context is written
 * @param ib Engine handle
 *
 * @returns Status code
 */
ib_status_t ib_context_create_main(ib_context_t **pctx,
                                   ib_engine_t *ib);

/**
 * Initialize a configuration context.
 *
 * This causes ctx_init functions to be executed for each module
 * registered in a configuration context.  It should be called
 * after a configuration context is fully configured.
 *
 * @param ctx Config context
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_init(ib_context_t *ctx);

/**
 * Destroy a configuration context.
 *
 * @param ctx Configuration context
 */
void DLL_PUBLIC ib_context_destroy(ib_context_t *ctx);

/**
 * Get the engine (startup) configuration context.
 *
 * @param ib Engine handle
 *
 * @returns Status code
 */
ib_context_t *ib_context_engine(ib_engine_t *ib);

/**
 * Get the main (default) configuration context.
 *
 * @param ib Engine handle
 *
 * @returns Status code
 */
ib_context_t DLL_PUBLIC *ib_context_main(ib_engine_t *ib);

/**
 * @todo Docs
 */
ib_status_t DLL_PUBLIC ib_context_init_cfg(ib_context_t *ctx,
                                           const void *base,
                                           const ib_cfgmap_init_t *init);

/**
 * Fetch the named module configuration data from the configuration context.
 *
 * @param ctx Configuration context
 * @param name Module name
 * @param pcfg Address which module config data is written
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_module_config(ib_context_t *ctx,
                                                ib_module_t *m,
                                                void *pcfg);

/**
 * Set a value in the config context.
 *
 * @param ctx Configuration context
 * @param name Variable name
 * @param pval Pointer to value to set
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_set(ib_context_t *ctx,
                                      const char *name,
                                      void *pval);

/**
 * Set a address value in the config context.
 *
 * @param ctx Configuration context
 * @param name Variable name
 * @param val Numeric value
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_set_num(ib_context_t *ctx,
                                          const char *name,
                                          uint64_t val);

/**
 * Set a string value in the config context.
 *
 * @param ctx Configuration context
 * @param name Variable name
 * @param val String value
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_set_string(ib_context_t *ctx,
                                      const char *name,
                                      const char *val);

/**
 * Get a value and length from the config context.
 *
 * @param ctx Configuration context
 * @param name Variable name
 * @param pval Address to which the value is written
 * @param ptype Address to which the value type is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_get(ib_context_t *ctx,
                                      const char *name,
                                      void *pval, ib_ftype_t *ptype);

/**
 * Default Site/Location context chooser.
 *
 * @param ctx Configuration context
 * @param type Context data type
 * @param ctxdata Context data
 * @param cbdata Chooser callback data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_context_siteloc_chooser(ib_context_t *ctx,
                                                  ib_ctype_t type,
                                                  void *ctxdata,
                                                  void *cbdata);
                                                  
/**
 * Create a connection structure.
 *
 * @param ib Engine handle
 * @param pconn Address which new connection is written
 * @param pctx Plugin connection context
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_conn_create(ib_engine_t *ib,
                                      ib_conn_t **pconn,
                                      void *pctx);

/**
 * Set connection flags.
 *
 * @param conn Connection structure
 * @param flag Flags
 */
#define ib_conn_flags_set(conn, flag) do { (conn)->flags |= (flag); } while(0)

/**
 * Check if connection flags are all set.
 *
 * @param conn Connection structure
 * @param flag Flags
 *
 * @returns All current flags
 */
#define ib_conn_flags_isset(conn, flag) ((conn)->flags & (flag) ? 1 : 0)

/**
 * Create a connection data structure.
 *
 * @param conn Connection structure
 * @param pconndata Address which new connection data is written
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_conn_data_create(ib_conn_t *conn,
                                           ib_conndata_t **pconndata,
                                           size_t dalloc);

/**
 * Destroy a connection structure.
 *
 * @param conn Connection structure
 */
void DLL_PUBLIC ib_conn_destroy(ib_conn_t *conn);

/**
 * Create a transaction structure.
 *
 * @param ib Engine handle
 * @param ptx Address which new transaction is written
 * @param conn Connection structure
 * @param pctx Plugin transaction context
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_tx_create(ib_engine_t *ib,
                                    ib_tx_t **ptx,
                                    ib_conn_t *conn,
                                    void *pctx);

/**
 * Set transaction flags.
 *
 * @param tx Transaction structure
 * @param flag Flags
 *
 * @returns All current flags
 */
#define ib_tx_flags_set(tx, flag) do { (tx)->flags |= (flag); } while(0)

/**
 * Check if transaction flags are all set.
 *
 * @param tx Transaction structure
 * @param flag Flags
 *
 * @returns All current flags
 */
#define ib_tx_flags_isset(tx, flag) ((tx)->flags & (flag) ? 1 : 0)

/**
 * Mark transaction as not having a body.
 *
 * @param tx Transaction structure
 */
#define ib_tx_mark_nobody(tx) ib_tx_flags_set(tx, IB_TX_FREQ_NOBODY)

/**
 * Destroy a transaction structure.
 *
 * @param tx Transaction structure
 */
void DLL_PUBLIC ib_tx_destroy(ib_tx_t *tx);

/**
 * @defgroup IronBeeEngineState State
 *
 * This API allows for sending state and data information to the engine.
 *
 * The following diagrams the general state processing order of a typical
 * transaction that is allowed to be fully processed.
 *
 * @dot
 * digraph legend1 {
 *   plugin_state [label="Plugin\nstate",shape=octagon]
 *   plugin_async_state [label="Plugin\nasyncronous\nstate",shape=octagon,peripheries=2]
 *   parser_state [label="Parser\nstate",shape=ellipse]
 *   engine_state [label="Engine\nstate",shape=diamond]
 *   handler_state [label="Handler\nstate",shape=parallelogram]
 *   entity [label="Entity",shape=box]
 *   note [label="Note",shape=note]
 *   {rank=same; plugin_state plugin_async_state parser_state engine_state handler_state note}
 * }
 * @enddot
 * @dot
 * digraph legend2 {
 *   edge1a [style=invis]
 *   edge1b [style=invis]
 *   edge2a [style=invis]
 *   edge2b [style=invis]
 *   edge3a [style=invis]
 *   edge3b [style=invis]
 *   { rank=same; edge1a edge1b edge2a edge2b edge3a edge3b }
 *
 *   edge1a -> edge1b [label="Ordered Transition\nto State"]
 *   edge2a -> edge2b [label="Influences Transition\nto State",style=dotted,arrowhead=none]
 *   edge3a -> edge3b [label="Alternate Transition\nto State",style=dashed]
 * }
 * @enddot
 * @dot
 * digraph legend3 {
 *   conn_style [label="connection event",style=bold,shape=box]
 *   tx_style [label="transaction event",style=filled,fillcolor="#e6e6e6",shape=box]
 *   data_style [label="data event",style=solid,shape=box]
 *   { rank=same; conn_style tx_style data_style }
 * }
 * @enddot
 * @dot
 * digraph legend3 {
 *   note1 [label="*  Automatically triggered if\l    not done explicitly (some\l    parsers may not be capable).\l",style=bold,shape=plaintext]
 *   note2 [label="** Special handler states allowing\l    modules to do any setup/init\l    within their context.\l",style=bold,shape=plaintext]
 *   { rank=same; note1 note2 }
 * }
 * @enddot
 *
 * Plugin states are triggered by the plugin and parser states by the
 * parser. These states cause the engine to trigger both the engine and
 * handler states. The engine states are meant to be syncronization
 * points. The handler states are meant to be handled by modules to do
 * detection and take actions, while the plugin and parser states are
 * to be used to generate fields and anything else needed in the handler
 * states.
 *
 * - Connection event hook callbacks receive a @ref ib_conn_t parameter.
 * - Data event hook callbacks receive a @ref ib_conndata_t parameter.
 * - Transaction event hook callbacks receive a @ref ib_tx_t parameter.
 *
 * @note Config contexts and some fields are populated during the plugin
 *       events and thus the following handler event is what should be used
 *       to use these contexts and fields for detection.
 *
 * The following diagram shows a complete connection from start to finish.
 *
 * @dot
 * digraph states {
 *
 *   start [label="start",style=bold,shape=plaintext]
 *   finish [label="finish",style=bold,shape=plaintext]
 *
 *   incoming [label="Raw (unparsed)\nIncoming\nData",style=solid,shape=box]
 *   outgoing [label="Raw (unparsed)\nOutgoing\nData",style=solid,shape=box]
 *   request [label="HTTP (parsed)\nRequest\nData",style=solid,shape=box]
 *   response [label="HTTP (parsed)\nResponse\nData",style=solid,shape=box]
 *
 *   context_conn_selected [label="At this point the connection context is\nselected. Events that follow will use the context\nselected here, which may impose a different\nconfiguration than previous events. Anything\nused in the context selection process must\nbe generated in a previous event handler.",style=bold,shape=note,URL="\ref handle_context_conn_event"]
 *   context_tx_selected [label="At this point the transaction context is\nselected. Events that follow will use the context\nselected here, which may impose a different\nconfiguration than previous events. Anything\nused in the context selection process must\nbe generated in a previous event handler.",style=filled,fillcolor="#e6e6e6",shape=note,URL="\ref handle_context_tx_event"]
 *
 *   conn_started_event [label="conn_started",style=bold,shape=diamond,URL="\ref conn_started_event"]
 *   conn_opened_event [label="conn_opened",style=bold,shape=octagon,URL="\ref conn_opened_event"]
 *   conn_data_in_event [label="conn_data_in",style=solid,shape=octagon,peripheries=2,URL="\ref conn_data_in_event"]
 *   conn_data_out_event [label="conn_data_out",style=solid,shape=octagon,peripheries=2,URL="\ref conn_data_out_event"]
 *   conn_closed_event [label="conn_closed",style=bold,shape=octagon,URL="\ref conn_closed_event"]
 *   conn_finished_event [label="conn_finished",style=bold,shape=diamond,URL="\ref conn_finished_event"]
 *
 *   tx_started_event [label="tx_started",style=filled,fillcolor="#e6e6e6",shape=diamond,URL="\ref tx_started_event"]
 *   tx_data_in_event [label="tx_data_in",style=solid,shape=octagon,peripheries=2,URL="\ref tx_data_in_event"]
 *   tx_process_event [label="tx_process",style=filled,fillcolor="#e6e6e6",shape=diamond,URL="\ref tx_process_event"]
 *   tx_data_out_event [label="tx_data_out",style=solid,shape=octagon,peripheries=2,URL="\ref tx_data_out_event"]
 *   tx_finished_event [label="tx_finished",style=filled,fillcolor="#e6e6e6",shape=diamond,URL="\ref tx_finished_event"]
 *
 *   request_started_event [label="request_started *",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref request_started_event"]
 *   request_headers_event [label="request_headers",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref request_headers_event"]
 *   request_body_event [label="request_body",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref request_body_event"]
 *   request_finished_event [label="request_finished",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref request_finished_event"]
 *   response_started_event [label="response_started *",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref response_started_event"]
 *   response_headers_event [label="response_headers",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref response_headers_event"]
 *   response_body_event [label="response_body",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref response_body_event"]
 *   response_finished_event [label="response_finished",style=filled,fillcolor="#e6e6e6",shape=ellipse,URL="\ref response_finished_event"]
 *
 *   handle_context_conn_event [label="handle_context_conn **",style=bold,shape=parallelogram,URL="\ref handle_context_conn_event"]
 *   handle_connect_event [label="handle_connect",style=bold,shape=parallelogram,URL="\ref handle_connect_event"]
 *   handle_context_tx_event [label="handle_context_tx **",style=filled,fillcolor="#e6e6e6",shape=parallelogram,URL="\ref handle_context_tx_event"]
 *   handle_request_headers_event [label="handle_request_headers",style=filled,fillcolor="#e6e6e6",shape=parallelogram,URL="\ref handle_request_headers_event"]
 *   handle_request_event [label="handle_request",style=filled,fillcolor="#e6e6e6",shape=parallelogram,URL="\ref handle_request_event"]
 *   handle_response_headers_event [label="handle_response_headers",style=filled,fillcolor="#e6e6e6",shape=parallelogram,URL="\ref handle_response_headers_event"]
 *   handle_response_event [label="handle_response",style=filled,fillcolor="#e6e6e6",shape=parallelogram,URL="\ref handle_response_event"]
 *   handle_disconnect_event [label="handle_disconnect",style=bold,shape=parallelogram,URL="\ref handle_disconnect_event"]
 *   handle_postprocess_event [label="handle_postprocess",style=filled,fillcolor="#e6e6e6",shape=parallelogram,URL="\ref handle_postprocess_event"]
 *
 *   // These are just for organizational purposes
 *   conn_started_event -> incoming [style=invis,weight=5.0]
 *   conn_data_in_event -> request [style=invis,weight=100.0]
 *   conn_data_out_event -> response [style=invis,weight=100.0]
 *   tx_started_event -> request [style=invis,weight=5.0]
 *   tx_started_event -> tx_finished_event [style=invis,weight=5.0]
 *   tx_process_event -> outgoing [style=invis]
 *   response_started_event -> response [style=invis,weight=5.0]
 *
 *   start -> conn_started_event [weight=500.0]
 *   incoming -> conn_data_in_event [dir=none,weight=100.0]
 *   conn_data_in_event -> conn_data_in_event [weight=0.1]
 *   conn_data_in_event -> tx_data_in_event [weight=0.1]
 *   tx_data_in_event -> tx_data_in_event [weight=0.1]
 *
 *   conn_started_event -> conn_opened_event [weight=100.0]
 *   conn_opened_event -> context_conn_selected [weight=100.0]
 *   context_conn_selected -> handle_context_conn_event [weight=100.0]
 *   handle_context_conn_event -> handle_connect_event [weight=100.0]
 *   handle_connect_event -> tx_started_event [weight=100.0]
 *
 *   request -> tx_data_in_event [dir=none,weight=100.0]
 *   tx_data_in_event -> request_started_event [style=dotted,arrowhead=none,weight=1.5]
 *   tx_data_in_event -> request_headers_event [style=dotted,arrowhead=none,weight=1.5]
 *   tx_data_in_event -> request_body_event [style=dotted,arrowhead=none,weight=1.5]
 *
 *   tx_started_event -> request_started_event [weight=5.0]
 *   request_started_event -> request_headers_event [weight=1.0]
 *   request_headers_event -> context_tx_selected [weight=1.0]
 *   context_tx_selected -> handle_context_tx_event [weight=1.0]
 *   handle_context_tx_event -> handle_request_headers_event [weight=1.0]
 *   handle_request_headers_event -> request_started_event [label="HTTP\nPipeline\nRequest",style=dashed,weight=10.0]
 *   handle_request_headers_event -> request_body_event [weight=1.0]
 *   request_body_event -> handle_request_event [weight=1.0]
 *   handle_request_event -> request_finished_event [weight=1.0]
 *   request_finished_event -> tx_process_event [weight=1.0]
 *
 *   tx_process_event -> response_started_event [weight=1.0]
 *
 *   response -> tx_data_out_event [dir=none,weight=100.0]
 *   outgoing -> conn_data_out_event [dir=none,weight=100.0]
 *   conn_data_out_event -> conn_data_out_event [weight=0.1]
 *   conn_data_out_event -> tx_data_out_event [weight=0.1]
 *   tx_data_out_event -> tx_data_out_event [weight=0.1]
 *
 *   tx_data_out_event -> response_started_event [style=dotted,arrowhead=none,weight=1.5]
 *   tx_data_out_event -> response_headers_event [style=dotted,arrowhead=none,weight=1.5]
 *   tx_data_out_event -> response_body_event [style=dotted,arrowhead=none,weight=1.5]
 *
 *   response_started_event -> response_headers_event [weight=1.0]
 *   response_headers_event -> handle_response_headers_event [weight=1.0]
 *   handle_response_headers_event -> response_body_event [weight=1.0]
 *   response_body_event -> handle_response_event [weight=1.0]
 *   handle_response_event -> response_finished_event [weight=5.0]
 *   response_finished_event -> response_started_event [label="HTTP\nPipeline\nResponse",style=dashed,weight=10.0]
 *
 *   response_finished_event -> handle_postprocess_event [weight=5.0]
 *   handle_postprocess_event -> tx_finished_event [weight=5.0]
 *
 *   tx_finished_event -> tx_started_event [weight=5.0,constraint=false]
 *   tx_finished_event -> conn_closed_event [weight=5.0]
 *
 *   conn_closed_event -> handle_disconnect_event [weight=5.0]
 *   handle_disconnect_event -> conn_finished_event [weight=10.0]
 *
 *   conn_finished_event -> finish [weight=500.0]
 * }
 * @enddot
 *
 * @{
 */

/**
 * State Event Types
 *
 * @warning Remember to update ib_state_event_name_list[] as well.
 */
typedef enum {
    /* Engine States */
    conn_started_event,            /**< Connection started */
    conn_finished_event,           /**< Connection finished */
    tx_started_event,              /**< Transaction started */
    tx_process_event,              /**< Transaction is about to be processed */
    tx_finished_event,             /**< Transaction finished */

    /* Handler States */
    handle_context_conn_event,     /**< Handle connection context chosen */
    handle_connect_event,          /**< Handle a connect */
    handle_context_tx_event,       /**< Handle transaction context chosen */
    handle_request_headers_event,  /**< Handle the request headers */
    handle_request_event,          /**< Handle the full request */
    handle_response_headers_event, /**< Handle the response headers */
    handle_response_event,         /**< Handle the full response */
    handle_disconnect_event,       /**< Handle a disconnect */
    handle_postprocess_event,      /**< Handle transaction post processing */

    /* Plugin States */
    conn_opened_event,             /**< Plugin notified connection opened */
    conn_data_in_event,            /**< Plugin notified of incoming data */
    conn_data_out_event,           /**< Plugin notified of outgoing data */
    conn_closed_event,             /**< Plugin notified connection closed */

    /* Parser States */
    tx_data_in_event,              /**< Parser notified of request data */
    tx_data_out_event,             /**< Parser notified of response data */
    request_started_event,         /**< Parser notified request has started */
    request_headers_event,         /**< Parser notified of request headers */
    request_body_event,            /**< Parser notified of request body */
    request_finished_event,        /**< Parser notified request finished */
    response_started_event,        /**< Parser notified response started */
    response_headers_event,        /**< Parser notified of response headers */
    response_body_event,           /**< Parser notified of response body */
    response_finished_event,       /**< Parser notified response finished */

    /* Not an event, but keeps track of the number of events. */
    IB_STATE_EVENT_NUM,
} ib_state_event_type_t;

/**
 * State Hook Callback Function.
 *
 * @param ib Engine handle
 * @param param Call parameter
 * @param cbdata Callback data
 */
typedef ib_status_t (*ib_state_hook_fn_t)(ib_engine_t *ib,
                                          void *param,
                                          void *cbdata);

/**
 * Resolve an event name.
 *
 * @param event Event type
 *
 * @returns Statically allocated event name
 */
const char *ib_state_event_name(ib_state_event_type_t event);

/**
 * Notify the state machine that a connection started.
 *
 * @param ib Engine handle
 * @param conn Connection
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_opened(ib_engine_t *ib,
                                                   ib_conn_t *conn);

/**
 * Notify the state machine that connection data came in.
 *
 * @param ib Engine handle
 * @param conndata Connection data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_data_in(ib_engine_t *ib,
                                                    ib_conndata_t *conndata);

/**
 * Notify the state machine that connection data is headed out.
 *
 * @param ib Engine handle
 * @param conndata Connection data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_data_out(ib_engine_t *ib,
                                                     ib_conndata_t *conndata);

/**
 * Notify the state machine that a connection finished.
 *
 * @param ib Engine handle
 * @param conn Connection
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_closed(ib_engine_t *ib,
                                                   ib_conn_t *conn);

/**
 * Notify the state machine that transaction data came in.
 *
 * @param ib Engine handle
 * @param txdata Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_tx_data_in(ib_engine_t *ib,
                                                  ib_txdata_t *txdata);

/**
 * Notify the state machine that transaction data is headed out.
 *
 * @param ib Engine handle
 * @param txdata Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_tx_data_out(ib_engine_t *ib,
                                                   ib_txdata_t *txdata);

/**
 * Notify the state machine that the request started.
 *
 * @note This is an optional event. Unless the plugin can detect that
 *       a request has started prior to receiving the headers, then you
 *       should just call @ref ib_state_notify_request_headers() when the
 *       headers are received which will automatically notify that the
 *       request has started.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_started(ib_engine_t *ib,
                                                       ib_tx_t *tx);

/**
 * Notify the state machine that request headers are available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_headers(ib_engine_t *ib,
                                                       ib_tx_t *tx);

/**
 * Notify the state machine that the request body is available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_body(ib_engine_t *ib,
                                                    ib_tx_t *tx);

/**
 * Notify the state machine that a request finished.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_finished(ib_engine_t *ib,
                                                        ib_tx_t *tx);

/**
 * Notify the state machine that a response started.
 *
 * @note This is an optional event. Unless the plugin can detect that
 *       a response has started prior to receiving the headers, then you
 *       should just call @ref ib_state_notify_response_headers() when the
 *       headers are received which will automatically notify that the
 *       request has started.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_started(ib_engine_t *ib,
                                                        ib_tx_t *tx);

/**
 * Notify the state machine that the response headers are available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_headers(ib_engine_t *ib,
                                                        ib_tx_t *tx);

/**
 * Notify the state machine that the response body is available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_body(ib_engine_t *ib,
                                                     ib_tx_t *tx);

/**
 * Notify the state machine that a response finished.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_finished(ib_engine_t *ib,
                                                         ib_tx_t *tx);

/**
 * @} QEngineState
 */


/**
 * @defgroup IronBeeEngineHooks Hooks
 * @{
 */

/** Hook */
typedef struct ib_hook_t ib_hook_t;

/**
 * Register a callback for a given event
 *
 * @param ib Engine handle
 * @param event Event
 * @param cb The callback to register
 * @param cdata Data passed to the callback (or NULL)
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_hook_register(ib_engine_t *ib,
                                        ib_state_event_type_t event,
                                        ib_void_fn_t cb, void *cdata);

/**
 * Unregister a callback for a given event
 *
 * @param ib Engine handle
 * @param event Event
 * @param cb The callback to unregister
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_hook_unregister(ib_engine_t *ib,
                                          ib_state_event_type_t event,
                                          ib_void_fn_t cb);

/**
 * Register a callback with a config context for a given event
 *
 * @param ctx Config context
 * @param event Event
 * @param cb The callback to register
 * @param cdata Data passed to the callback (or NULL)
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_hook_register_context(ib_context_t *ctx,
                                                ib_state_event_type_t event,
                                                ib_void_fn_t cb, void *cdata);

/**
 * Unregister a callback with a config context for a given event
 *
 * @param ctx Config context
 * @param event Event
 * @param cb The callback to unregister
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_hook_unregister_context(ib_context_t *ctx,
                                                  ib_state_event_type_t event,
                                                  ib_void_fn_t cb);

/**
 * @} IronBeeEngineHooks
 */

/**
 * @defgroup IronBeeEngineData Data Field
 * @{
 */

/**
 * Add a data field.
 *
 * @param dpi Data provider instance
 * @param f Field
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add(ib_provider_inst_t *dpi,
                                   ib_field_t *f);

/**
 * Create and add a numeric data field (extended version).
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param nlen Name length
 * @param val Numeric value
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_num_ex(ib_provider_inst_t *dpi,
                                          const char *name,
                                          size_t nlen,
                                          uint64_t val,
                                          ib_field_t **pf);

/**
 * Create and add a nulstr data field (extended version).
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param nlen Name length
 * @param val String value
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_nulstr_ex(ib_provider_inst_t *dpi,
                                             const char *name,
                                             size_t nlen,
                                             char *val,
                                             ib_field_t **pf);

/**
 * Create and add a bytestr data field (extended version).
 *
 * @param dpi Data provider instance
 * @param name Name
 * @param nlen Name length
 * @param val Byte string value
 * @param vlen Value length
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_bytestr_ex(ib_provider_inst_t *dpi,
                                              const char *name,
                                              size_t nlen,
                                              uint8_t *val,
                                              size_t vlen,
                                              ib_field_t **pf);

/**
 * Create and add a list data field (extended version).
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param nlen Name length
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_list_ex(ib_provider_inst_t *dpi,
                                           const char *name,
                                           size_t nlen,
                                           ib_field_t **pf);

/**
 * Get a data field (extended version).
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param nlen Name length
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_get_ex(ib_provider_inst_t *dpi,
                                      const char *name,
                                      size_t nlen,
                                      ib_field_t **pf);

/**
 * Create and add a numeric data field.
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param val Numeric value
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_num(ib_provider_inst_t *dpi,
                                       const char *name,
                                       uint64_t val,
                                       ib_field_t **pf);

#define ib_data_add_num(dpi,name,val,pf) \
    ib_data_add_num_ex(dpi,name,strlen(name),val,pf)

/**
 * Create and add a nulstr data field.
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param val String value
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_nulstr(ib_provider_inst_t *dpi,
                                          const char *name,
                                          char *val,
                                          ib_field_t **pf);

#define ib_data_add_nulstr(dpi,name,val,pf) \
    ib_data_add_nulstr_ex(dpi,name,strlen(name),val,pf)

/**
 * Create and add a bytestr data field.
 *
 * @param dpi Data provider instance
 * @param name Name
 * @param val Byte string value
 * @param vlen Value length
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_bytestr(ib_provider_inst_t *dpi,
                                           const char *name,
                                           uint8_t *val,
                                           size_t vlen,
                                           ib_field_t **pf);

#define ib_data_add_bytestr(dpi,name,val,vlen,pf) \
    ib_data_add_bytestr_ex(dpi,name,strlen(name),val,vlen,pf)

/**
 * Create and add a list data field.
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_add_list(ib_provider_inst_t *dpi,
                                        const char *name,
                                        ib_field_t **pf);

#define ib_data_add_list(dpi,name,pf) \
    ib_data_add_list_ex(dpi,name,strlen(name),pf)

/**
 * Get a data field.
 *
 * @param dpi Data provider instance
 * @param name Name as byte string
 * @param pf Pointer where new field is written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_data_get(ib_provider_inst_t *dpi,
                                   const char *name,
                                   ib_field_t **pf);

#define ib_data_get(dpi,name,pf) \
    ib_data_get_ex(dpi,name,strlen(name),pf)

/**
 * @} IronBeeEngineData
 */

/**
 * @defgroup IronBeeEngineMatcher Matcher
 * @{
 */

typedef struct ib_matcher_t ib_matcher_t;
typedef void ib_matcher_result_t; /// @todo Not implemented yet

ib_status_t DLL_PUBLIC ib_matcher_create(ib_engine_t *ib,
                                         ib_mpool_t *pool,
                                         const char *key,
                                         ib_matcher_t **pm);

void DLL_PUBLIC *ib_matcher_compile(ib_matcher_t *m,
                                    const char *patt,
                                    const char **errptr,
                                    int *erroffset);

ib_status_t DLL_PUBLIC ib_matcher_match_buf(ib_matcher_t *m,
                                            void *cpatt,
                                            ib_flags_t flags,
                                            const uint8_t *data,
                                            size_t dlen);

ib_status_t DLL_PUBLIC ib_matcher_match_field(ib_matcher_t *m,
                                              void *cpatt,
                                              ib_flags_t flags,
                                              ib_field_t *f);

ib_status_t DLL_PUBLIC ib_matcher_add_pattern(ib_matcher_t *m,
                                              const char *patt);

ib_status_t DLL_PUBLIC ib_matcher_exec_buf(ib_matcher_t *m,
                                           ib_flags_t flags,
                                           const uint8_t *data,
                                           size_t dlen);

ib_status_t DLL_PUBLIC ib_matcher_exec_field(ib_matcher_t *m,
                                             ib_flags_t flags,
                                             ib_field_t *f);

/**
 * @} IronBeeEngineMatcher
 */


/**
 * @defgroup IronBeeEngineLog Logging
 * @{
 */

/**
 * Logger callback.
 *
 * @param cbdata Callback data
 * @param level Log level
 * @param prefix Optional prefix to the log
 * @param file Optional source filename (or NULL)
 * @param line Optional source line number (or 0)
 * @param fmt Formatting string
 */
typedef void (*ib_log_logger_fn_t)(void *cbdata,
                                   int level,
                                   const char *prefix,
                                   const char *file, int line,
                                   const char *fmt, va_list ap)
                                   VPRINTF_ATTRIBUTE(6);

/** Normal Logger. */
#define ib_log(ib,lvl,...) ib_clog_ex(ib_context_main(ib),(lvl),NULL,NULL,0,__VA_ARGS__)
/** Error Logger. */
#define ib_log_error(ib,lvl,...) ib_clog_ex(ib_context_main(ib),(lvl),"ERROR - ",NULL,0,__VA_ARGS__)
/** Abort Logger. */
#define ib_log_abort(ib,...) do { ib_clog_ex(ib_context_main(ib),0,"ABORT - ",__FILE__,__LINE__,__VA_ARGS__); abort(); } while(0)
/** Debug Logger. */
#define ib_log_debug(ib,lvl,...) ib_clog_ex(ib_context_main(ib),(lvl),NULL,__FILE__,__LINE__,__VA_ARGS__)

/** Normal Context Logger. */
#define ib_clog(ctx,lvl,...) ib_clog_ex((ctx),(lvl),NULL,NULL,0,__VA_ARGS__)
/** Error Logger. */
#define ib_clog_error(ctx,lvl,...) ib_clog_ex((ctx),(lvl),"ERROR - ",NULL,0,__VA_ARGS__)
/** Abort Logger. */
#define ib_clog_abort(ctx,...) do { ib_clog_ex((ctx),0,"ABORT - ",__FILE__,__LINE__,__VA_ARGS__); abort(); } while(0)
/** Debug Logger. */
#define ib_clog_debug(ctx,lvl,...) ib_clog_ex((ctx),(lvl),NULL,__FILE__,__LINE__,__VA_ARGS__)

/**
 * Initialize logging.
 *
 * @param ib Engine handle
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_log_init(ib_engine_t *ib);

/**
 * Generic Logger.
 *
 * @todo Get a real logging framework.
 *
 * @warning There is currently a 1024 byte formatter limit when prepending the
 *          log header data.
 *
 * @param ctx Config context
 * @param level Log level (0-9)
 * @param prefix String to prefix log header data (or NULL)
 * @param file Filename (or NULL)
 * @param line Line number (or 0)
 * @param fmt Printf-like format string
 */
void DLL_PUBLIC ib_clog_ex(ib_context_t *ctx, int level,
                           const char *prefix, const char *file, int line,
                           const char *fmt, ...);

/**
 * Generic Logger (va_list version).
 *
 * @todo Get a real logging framework.
 *
 * @warning There is currently a 1024 byte formatter limit when prepending the
 *          log header data.
 *
 * @param ctx Config context
 * @param level Log level (0-9)
 * @param prefix String to prefix log header data (or NULL)
 * @param file Filename (or NULL)
 * @param line Line number (or 0)
 * @param fmt Printf-like format string
 * @param ap Variable argument pointer
 */
void DLL_PUBLIC ib_vclog_ex(ib_context_t *ctx, int level,
                            const char *prefix, const char *file, int line,
                            const char *fmt, va_list ap);

/**
 * @} IronBeeEngineLog
 */

/**
 * @defgroup IronBeeEngineLogEvent Log Events
 * @{
 */

struct ib_logevent_t {
    uint64_t       id;           /**< Event ID */
    char          *publisher;    /**< Publisher name */
    char          *source;       /**< Source identifier */
    char          *source_ver;   /**< Source version string */
    char          *msg;          /**< Event message */
    uint8_t       *data;         /**< Event data that matched */
    size_t         data_len;     /**< Event data size */
    /// TODO: uint8_t types should be typedef
    uint8_t        type;         /**< Event type */
    uint8_t        activity;     /**< Event activity (recon, attache, etc.) */
    uint8_t        pri_cat;      /**< Primary category (ex: injection) */
    uint8_t        sec_cat;      /**< Secondary categoty (ex: SQL) */
    uint8_t        confidence;   /**< Event confidence (percent) */
    uint8_t        severity;     /**< Event severity (0-100?) */
    uint8_t        sys_env;      /**< System environment (pub or priv)  */
    uint8_t        rec_action;   /**< Recommended action */
    ib_list_t     *tags;         /**< List of tags */
    ib_list_t     *fields;       /**< List of fields */
    ib_mpool_t    *mp;           /**< Memory pool */
};

/**
 * @} IronBeeEngineLogEvent
 */


/**
 * @defgroup IronBeeEngineTfn Transformations
 * @{
 */

/** Set if transformation modified the value. */
#define IB_TFN_FMODIFIED          (1<<0)
/** Set if transformation performed an in-place operation. */
#define IB_TFN_FINPLACE           (1<<1)

/**
 * Check if FMODIFIED flag is set.
 *
 * @param f Transformation flags
 *
 * @returns True if FMODIFIED flag is set
 */
#define IB_TFN_CHECK_FMODIFIED(f) ((f) & IB_TFN_FMODIFIED)

/**
 * Check if FINPLACE flag is set.
 *
 * @param f Transformation flags
 *
 * @returns True if FINPLACE flag is set
 */
#define IB_TFN_CHECK_FINPLACE(f) ((f) & IB_TFN_FINPLACE)


/**
 * Transformation function.
 *
 * @param fndata Transformation function data (config)
 * @param data_in Input data
 * @param dlen_in Input data length
 * @param data_out Output data
 * @param dlen_out Output data length
 * @param flags Address of flags set by transformation
 *
 * @returns Status code
 */
typedef ib_status_t (*ib_tfn_fn_t)(void *fndata,
                                   uint8_t *data_in,
                                   size_t dlen_in,
                                   uint8_t **data_out,
                                   size_t *dlen_out,
                                   ib_flags_t *pflags);

/**
 * Create and register a new transformation.
 *
 * @param ib Engine handle
 * @param name Transformation name
 * @param transform Transformation function
 * @param fndata Transformation function data
 * @param ptfn Points to address where new tfn will be written if non-NULL
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_tfn_create(ib_engine_t *ib,
                                     const char *name,
                                     ib_tfn_fn_t transform,
                                     void *fndata,
                                     ib_tfn_t **ptfn);

/**
 * Lookup a transformation by name.
 *
 * @param ptfn Address where new tfn will be written
 * @param name Transformation name
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_tfn_lookup(ib_engine_t *ib,
                                     const char *name,
                                     ib_tfn_t **ptfn);

/**
 * Transform data.
 *
 * @note Some transformations may destroy/overwrite the original data.
 *
 * @param tfn Transformation
 * @param data_in Input data
 * @param dlen_in Input data length
 * @param data_out Address of output data
 * @param dlen_out Address of output data length
 * @param flags Address of flags set by transformation
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_tfn_transform(ib_tfn_t *tfn,
                                        uint8_t *data_in,
                                        size_t dlen_in,
                                        uint8_t **data_out,
                                        size_t *dlen_out,
                                        ib_flags_t *pflags);

/**
 * @} IronBeeEngineTfn
 */


/**
 * @} IronBeeEngine
 * @} IronBee
 */

#ifdef __cplusplus
}
#endif

#endif /* _IB_ENGINE_H_ */
