#include <stdint.h>
#include <setjmp.h>


#define E_OK                        (0)    // 正常終了 (Success)

#define E_SYSTEM_ERROR              (-5)   // システムエラー (System Error)
#define E_COPROCESSOR_UNAVAILABLE   (-6)   // コプロセッサ使用不可 (Coprocessor Unavailable)
#define E_NOT_SUPPORTED             (-9)   // 未サポート機能 (Feature Not Supported)
#define E_RESERVED_FUNCTION_NUMBER  (-10)  // 予約機能コード番号 (Reserved Function Number)
#define E_INVALID_ATTRIBUTE         (-11)  // 不正属性 (Invalid Attribute)
#define E_INVALID_PARAMETER         (-17)  // パラメータ不正 (Invalid Parameter)
#define E_INVALID_ID                (-18)  // ID不正 (Invalid ID)
#define E_CONTEXT_ERROR             (-25)  // コンテキストエラー (Context Error)
#define E_MEMORY_ACCESS_VIOLATION   (-26)  // メモリアクセス違反 (Memory Access Violation)
#define E_OBJECT_ACCESS_VIOLATION   (-27)  // オブジェクトアクセス違反 (Object Access Violation)
#define E_ILLEGAL_SYSTEM_CALL       (-28)  // システムコール不正 (Illegal System Call)
#define E_OUT_OF_MEMORY             (-33)  // メモリ不足 (Out Of Memory)
#define E_LIMIT_ERROR               (-34)  // 上限値エラー (Limit Error)
#define E_OBJECT_STATE_ERROR        (-41)  // オブジェクト状態エラー (Object State Error)
#define E_OBJECT_DOES_NOT_EXIST     (-42)  // オブジェクト不正エラー (Object Does Not Exist)
#define E_QUEUE_OVERFLOW            (-43)  // 上限エラー (Queue Overflow)
#define E_WAIT_ABORTED              (-49)  // 待ち状態強制解除 (Wait Aborted)
#define E_TIMEOUT                   (-50)  // タイムアウトエラー (Timeout Error)
#define E_WAIT_OBJECT_DELETED       (-51)  // 待ちオブジェクト削除 (Wait Object Deleted)
#define E_WAIT_DISALLOWED           (-52)  // 待ち禁止による待ち状態解除 (Wait Disallowed)
#define E_IO_ERROR                  (-57)  // I/Oエラー (I/O Error)

#define MAX_TASK 10
#define TASK_STACK_SIZE 2048
#define MAX_PRIORITY 10
#define TIMER_PERIOD 10

#define BEGIN_CRITICAL_SECTION disable_signal();
#define END_CRITICAL_SECTION enable_signal();

typedef uint32_t    ID;
typedef void (*FP)(void);
typedef uint32_t PRIORITY;
typedef int32_t ERROR;
typedef int32_t RELATIVE_TIME;

/* ステータス */
typedef enum {
    TASK_STATUS_NONE,
    TASK_STATUS_DORMANT,
    TASK_STATUS_READY,
    TASK_STATUS_WAIT
} TASK_STATUS;

typedef enum {
    WAITFACT_NONE,
    WAITFACT_DELAY
} WAITFACT;

/* TCB */
typedef struct st_tcb {
    struct st_tcb *pre;
    struct st_tcb *next;

    ID taskid;
    PRIORITY task_priority;
    TASK_STATUS status;
    jmp_buf context;
    FP task;

    WAITFACT waitfact;
    RELATIVE_TIME waittime;
} TCB;

/* タスク生成用 */
typedef struct {
    FP task;
    PRIORITY task_priority;
} Type_Create_Task;


/* 宣言 */
extern ID cy_create_task(Type_Create_Task *pk_create_task);
extern ERROR cy_delay_task(RELATIVE_TIME delaytime);

extern void scheduler();
extern void dispatch(jmp_buf from, jmp_buf to);
extern void usermain(void);

extern void queue_add_entry(TCB **queue, TCB *tcb);
extern void queue_remove_top(TCB **queue);
extern void queue_remove_entry(TCB **queue, TCB *tcb);