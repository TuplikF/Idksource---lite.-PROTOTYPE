// =========================
//  Idksource-Lite Kernel
// =========================

#include <stdint.h>
#include <stddef.h>

// -------------------------
// 1) Simple allocator
// -------------------------
static uint8_t* heap_ptr = (uint8_t*)0x100000; // Start of RAM

void* kmalloc(size_t size) {
    void* p = heap_ptr;
    heap_ptr += size;
    return p;
}

// -------------------------
// 2) Task structure
// -------------------------
typedef struct {
    int pid;
    void (*entry)(void);
    int state;
    uint32_t* stack;
} task_t;

#define MAX_TASKS 8
task_t* tasks[MAX_TASKS];
int current_task = 0;
int task_count = 0;

// -------------------------
// 3) Create task
// -------------------------
int create_task(void (*entry)(void)) {
    task_t* t = kmalloc(sizeof(task_t));
    t->pid = task_count;
    t->entry = entry;
    t->state = 0;
    t->stack = kmalloc(1024); // simple stack

    tasks[task_count] = t;
    return task_count++;
}

// -------------------------
// 4) Scheduler
// -------------------------
void schedule() {
    while (1) {
        task_t* t = tasks[current_task];
        t->entry();  // just a function call for now 
        current_task = (current_task + 1) % task_count;
    }
}

// -------------------------
// 5) Kernel print (stub)
// -------------------------
void kprint(const char* s) {
    // Later: VGA text mode / UART
}

// -------------------------
// 6) Kernel entry point
// -------------------------
extern void cli_main(); // from CLI from

void kernel_main() {
    kprint("Kernel start\n");

    create_task(cli_main); // first process
    schedule();            // never returns
}