#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TASK_NAME_LENGTH 20
#define SCHED_YES 1
#define SCHED_NO 2
#define SCHED_UNKNOWN 3

typedef struct task {
    char name[TASK_NAME_LENGTH];
    int period;
    int WCET;
    int priority;
    int deadline;
    struct task* next;
} task;

task* first_task = NULL;
task* last_task = NULL;

/* Allocate a task and add it to the end of the linked list */
void add_task(char* name, int period, int WCET, int priority, int deadline)
{
    task* new_task = (task*)malloc(sizeof(task));
    if (strlen(name) < TASK_NAME_LENGTH) {
        strcpy(new_task->name, name);
    }
    else {
        printf("The task name is too long.\n");
        exit(-1); // Exit with an error code
    }

    new_task->period = period;
    new_task->WCET = WCET;
    new_task->priority = priority;
    new_task->deadline = deadline;

    new_task->next = NULL;
    if (first_task == NULL && last_task == NULL) { // If this is the first task to be added
        first_task = new_task;
        last_task = new_task;
        return;
    }
    last_task->next = new_task;
    last_task = new_task;
}

/* Remove first task in the list */
void remove_first_task()
{
    task* temp = first_task;

    if (first_task == NULL) {
        printf("The task list is empty\n");
        return;
    }
    if (first_task == last_task) { // Exactly one task in the list
        first_task = NULL;
        last_task = NULL;
    }
    else { // Set first_task to be the second task in the list
        first_task = first_task->next;
    }
    free(temp); // Deallocate the memory for the first task
}

/* Remove all tasks by iteratively removing the first task until the list is
   empty
*/
void remove_all_tasks()
{
    while (first_task != NULL) {
        remove_first_task();
    }
}

/* This function should return the number of tasks added to the system */
int nbr_of_tasks()
{
    // Todo: Implement this function. You should not change anything outside this
    // function.
	task* curr_task = first_task;
    int count_task = 0;
    while (curr_task != NULL) {

        count_task++;
        curr_task = curr_task->next;
    }
    return count_task;
}

/* Print information about all tasks in the list
   Notice, how to iterate through all tasks in the list
*/
void print_tasks()
{
    task* curr_task = first_task;

    while (curr_task != NULL) {
        printf("Name: %s, period: %d, WCET: %d, priority %d, deadline: %d\n",
            curr_task->name, curr_task->period, curr_task->WCET,
            curr_task->priority, curr_task->deadline);
        curr_task = curr_task->next;
    }
}

/*
Test if the tasks is schedulable according to the critera by Liu and Layland.
Reference: C. L. Liu and J. Layland. Scheduling algorithms for multiprogramming
in a hard real-time environment, Journal of the ACM, 20(1), 1973, pp. 46-61.
Return:
   1: Yes, the tasks are schedulable (according to the criteria)
   2: No, the tasks are not schedulable
   3: The criteria cannot determine if the tasks are schedulable or not

Assumptions: Priorities are unique, the list of tasks contains at least one
task.
*/
int schedulable_Liu_Layland()
{
    // Todo: Implement this function

    // REMOVE
    task* curr_task = first_task;
    int n = 0;
    double utilization = 0.0;

    while (curr_task != NULL) {
        if (curr_task->period > curr_task->deadline || curr_task->WCET > curr_task->deadline) {
            return SCHED_UNKNOWN;
        }
        utilization += (double)curr_task->WCET / curr_task->period;
        n++;
        curr_task = curr_task->next;
    }
    // printf("n = %d\n", n);
    // printf("utilization = %f\n",utilization);
    if (n == 0) return SCHED_UNKNOWN;

    double bound = n * (pow(2.0, 1.0 / n) - 1.0);
    // printf("bound = %f\n",bound);
    if (utilization <= bound)
        return SCHED_YES;
    else
        return SCHED_UNKNOWN;
}

/*
Test if the tasks is schedulable according to the exact response time analysisnd
Layland.
Return:
   1: Yes, the tasks are schedulable (according to the criteria)
   2: No, the tasks are not schedulable
   3: The criteria cannot determine if the tasks are schedulable or not

Assumptions: Priorities are unique, the list of tasks contains at least one
task.
*/
int schedulable_response_time_analysis()
{
    // Todo: Implement this function
    // The C library provides a ceiling function that returns the smallest integer
    // value greater than or equal to x: double ceil(double x)
    task* curr_task = first_task;

    while (curr_task != NULL) {
        int Ri = curr_task->WCET;
        int prev_Ri = -1; //set a unlegal time to prevent the first "while" loop must be excuted

        
        while (Ri != prev_Ri) {
            prev_Ri = Ri;
            int interference = 0;

            task* other_task = first_task;
            while (other_task != NULL) {
                if (other_task->priority > curr_task->priority) {
                    // printf("other_task is %d\n", other_task->priority);
                    // printf("curr_task is %d\n", curr_task->priority);
                    interference += (int)ceil((double)prev_Ri / other_task->period) * other_task->WCET;
                    // printf("aaainterference is %d\n", interference);
                }
                other_task = other_task->next;
            }

            Ri = curr_task->WCET + interference;
            // printf("wcet is %d\n", curr_task-> WCET);
            // printf("interference is %d\n", interference);
            // printf("current task is %s\n", curr_task->name);
            // printf("RI is %d\n", Ri);
            if (Ri > curr_task->deadline) {
                return SCHED_NO;
            }

        }

        curr_task = curr_task->next;
    }

    return SCHED_YES;
}


int check_schedulable(int correct_response_Liu_Layland,
    int correct_response_response_time_analysis)
{
    int nbr_of_failed_tests = 0;
    if (schedulable_Liu_Layland() != correct_response_Liu_Layland) {
        nbr_of_failed_tests++;
        printf("\n=== Schedulable test failed: Liu-Layland\n");
        print_tasks();
        printf("===\n");
    }

    if (schedulable_response_time_analysis() != correct_response_response_time_analysis) {
        nbr_of_failed_tests++;
        printf("\n=== Schedulable test failed: Response-Time Analysis\n");
        print_tasks();
        printf("===\n");
    }
    return nbr_of_failed_tests;
}

/* Do not change or remove any code in this function.
   If you like you can add additional test cases.
   TODO
*/
int check_tests()
{
    int nbr_of_failed_tests = 0;

    remove_all_tasks();

    // Add test
    // add_task(name, period, WCET, priority, deadline)
    add_task("T1", 25, 10, 5, 25);
    add_task("T2", 25, 8, 4, 25);
    add_task("T3", 50, 5, 3, 50);
    add_task("T4", 50, 4, 2, 50);
    add_task("T5", 100, 2, 1, 100);

    nbr_of_failed_tests += check_schedulable(SCHED_UNKNOWN, SCHED_YES);

    // remove_all_tasks();
    // // Add test
    add_task("T1", 50, 12, 1, 50);
    add_task("T2", 40, 10, 2, 40);
    add_task("T3", 30, 10, 3, 30);

    nbr_of_failed_tests += check_schedulable(SCHED_UNKNOWN, SCHED_NO);

    remove_all_tasks();
    // Add test
    add_task("T1", 60, 20, 1, 60);
    add_task("T2", 40, 10, 2, 40);
    add_task("T3", 20, 10, 3, 20);

    nbr_of_failed_tests += check_schedulable(SCHED_UNKNOWN, SCHED_NO);

    remove_all_tasks();
    // Add test
    add_task("T1", 7, 2, 3, 7);
    add_task("T2", 12, 3, 2, 12);
    add_task("T3", 20, 4, 1, 20);

    nbr_of_failed_tests += check_schedulable(SCHED_YES, SCHED_YES);

    remove_all_tasks();
    // Add test
    add_task("T1", 20, 4, 1, 20);
    add_task("T2", 12, 3, 2, 6);
    add_task("T3", 7, 2, 3, 3);

    nbr_of_failed_tests += check_schedulable(SCHED_UNKNOWN, SCHED_YES);

    return nbr_of_failed_tests;
}

int main()
{
    int nbr_of_failed_tests = check_tests();
    printf("\n=== Total number of failed tests: %d\n", nbr_of_failed_tests);
 //   fflush(stdout); // Flush print buffer before terminating
    return 0;
}
