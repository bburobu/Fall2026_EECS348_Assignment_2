//
//Name of program: 3228668_Warren_assignment2.c
//Discription: A program that takes in a list of commands holding emails, 
//adds them to a max heap by priority, and completes each list of commands.
//commands able to be used:
//EMAIL (category, subject, date)
//READ
//COUNT
//NEXT
//
//Inputs: .txt file where each line contains only one of any of ay of the above mentioned commands.
// Output: all commands completed (printed statements that commands were done)
// collaborators: none
//other sources: Google Gemini's code
//Author: Christopher Warren
//Creation date: 9/17/2026
// Revision date: 9/17/2026
// Revisions: added comments, altered heapify-up to be iterative

#include <stdio.h>  //input/output header file
#include <stdlib.h> //library file
#include <string.h> // header file to manipulate strings

#define MAX_LINE_LEN 512 //this is for keeping memory for the command file
#define MAX_STR_LEN 128 //this is for keeping memory for the emails

// Enum representing sender priority (higher values indicate higher priority)
//the boss is the most important, hile other people are the least,
// and unknown are even lower.
typedef enum {
    OTHER_PERSON = 0,
    IMPORTANT_PERSON = 1,
    PEER = 2,
    SUBORDINATE = 3,
    BOSS = 4,
    UNKNOWN = -1
} Priority; //type name Priority

// Structure to store Email information
typedef struct {
    Priority priority;
    char category[MAX_STR_LEN]; //e. g. "boss"
    char subject[MAX_STR_LEN]; //e. g. "Quarterly Report"
    char date[MAX_STR_LEN]; //e. g. "9/17/2026"
    unsigned long long id; // Tracks arrival order to prioritize the newest on ties
} Email; //type name Email

// Max Heap Structure using a dynamic array (list-based representation)
typedef struct {
    Email *arr; //declares a pointer for emails
    int size; 
    int capacity;
} MaxHeap; //type name MaxHep

// Global counter tracking insertion age (in case multiple emails have same priority)
unsigned long long insertion_counter = 0;

// Helper to map string category to Priority enum
// Inputs: category (the second part of the EMAIL command line)
// Output: the correct Priority value
//strcmp() checks if theyre identical or not (0 means it is)
Priority get_priority(const char *category) {
    if (strcmp(category, "Boss") == 0) return BOSS;
    if (strcmp(category, "Subordinate") == 0) return SUBORDINATE;
    if (strcmp(category, "Peer") == 0) return PEER;
    if (strcmp(category, "ImportantPerson") == 0) return IMPORTANT_PERSON;
    if (strcmp(category, "OtherPerson") == 0) return OTHER_PERSON;
    return UNKNOWN;
}

// Comparison function: 
// Input: two emails (email a and email b)
//Output: returns 1 if email 'a' has higher max-heap precedence than 'b'
int has_higher_priority(Email a, Email b) {
    if (a.priority > b.priority) return 1;
    if (a.priority < b.priority) return 0;
    // Tie-breaker: If priority is identical, the newest email (higher id) comes first
    return (a.id > b.id) ? 1 : 0;
}

// Swap two email objects in the heap
//inputs: two emails (email x and y)
//outputs: none (but swaps the values)
void swap(Email *x, Email *y) {
    Email temp = *x; //temporary value to keep the first email
    *x = *y;
    *y = temp;
}

// Create and initialize the heap
MaxHeap* create_heap(int capacity) {
    MaxHeap *heap = (MaxHeap*)malloc(sizeof(MaxHeap)); //malloc is for memory for the heap
    heap->capacity = capacity;// how much it can hold
    heap->size = 0; // where it starts
    heap->arr = (Email*)malloc(capacity * sizeof(Email)); //keeps memory fro the email capacity in our maxheap
    return heap;
}

// Shift up to maintain heap property
//input: the heap and the index out object to be heaped up is
//output: none, but shifts the objects in the heap iteratively to keep everything in order
void heapify_up(MaxHeap *heap, int index) {
    while (index > 0) { //while its not at the root
        int parent = (index - 1) / 2; //email's parent in maxheap
        if (has_higher_priority(heap->arr[index], heap->arr[parent])) { //if the child has more priority, switch
            swap(&heap->arr[index], &heap->arr[parent]);
            index = parent; //moves the index to the parent co check again
        } else {
            break; // done when the child isnt higher priority or reaches root
        }
    }
}

//iterative version of heapify down, similar to heapify up
//inputs: the maxheap and the index
//output: no output, but properly swaps the higher priority child if there is one

void heapify_down(MaxHeap *heap, int index) { 
    int size = heap->size;
    while (1) {
        int highest = index; //the index we are on i. e. parent
        int left = 2 * index + 1; //child (left)
        int right = 2 * index + 2; //child (right)
            //we check left then right child
            // checking the size helps not go off the array
        if (left < size && has_higher_priority(heap->arr[left], heap->arr[highest])) {
            highest = left;
        }
        if (right < size && has_higher_priority(heap->arr[right], heap->arr[highest])) {
            highest = right;
        }
        if (highest != index) { //if child is higher, we swap it
            swap(&heap->arr[index], &heap->arr[highest]);
            index = highest; // Step downward smoothly
        } else {
            break;
        }
    }
}

// Insert a new email item
void insert(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) { //if the array's full, increase the size
        heap->capacity *= 2;
        heap->arr = (Email*)realloc(heap->arr, heap->capacity * sizeof(Email));
    }
    heap->arr[heap->size] = email; //add the email to the heap
    heapify_up(heap, heap->size); //we make sure the email is in the right position in the heap
    heap->size++; //increase heap size
}

// Remove and return the maximum priority item
//input: the maxheap
//output: the removed email
Email extract_max(MaxHeap *heap) {
    Email max_email = heap->arr[0]; 
    heap->arr[0] = heap->arr[heap->size - 1]; 
    heap->size--; //reduce the size of the heap
    if (heap->size > 0) { //if the list has items, heapify down
        heapify_down(heap, 0);
    }
    return max_email;
}

// Deallocate heap arrays (garbage collection)
void free_heap(MaxHeap *heap) {
    free(heap->arr); //free email array
    free(heap); //free heap pointer
}

// Safely clean newline and carriage return characters from end of lines
//input: line
//no output
void trim_newline(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

int main() { // the main processing part!
    char filename[MAX_STR_LEN]; //our variable for the file inputted
    printf("Enter input file name: ");
    if (scanf("%127s", filename) != 1) return 1; //gets finename, also ends program in case of crash

    FILE *file = fopen(filename, "r"); //opens file (reads only, we dont modify)
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    MaxHeap *heap = create_heap(10);  //create our starting heap
    char line[MAX_LINE_LEN]; 

    while (fgets(line, sizeof(line), file)) { //look through the lines of the file line by line
        trim_newline(line); //gets rid of the extra stuff on the ends of the line
        if (strlen(line) == 0) continue; //skips blank lines

        // Separate command text from arguments by targeting the first space delimiter
        char line_copy[MAX_LINE_LEN];
        strcpy(line_copy, line); //creates a copy of the line in the list to use
        
        char *cmd = strtok(line_copy, " ");
        if (cmd == NULL) continue; //skips if empty

        if (strcmp(cmd, "EMAIL") == 0) { //if the command is email,
            // Find the start of arguments past the command name text and its space
            char *args_start = line + 6; 

            char args_copy[MAX_LINE_LEN];
            strcpy(args_copy, args_start);

            // Tokenize strict comma boundaries (spaces inside subject fields are left intact)
            char *cat = strtok(args_copy, ","); // get category
            char *subj = strtok(NULL, ","); // get subject
            char *dt = strtok(NULL, ","); // get date

            if (cat && subj && dt) {  //make new email variable if the above strings exist
                Email new_email;
                new_email.id = ++insertion_counter;  //newness counter

                strcpy(new_email.category, cat); //we need to make copies of whats in the pointers above
                strcpy(new_email.subject, subj);
                strcpy(new_email.date, dt);
                new_email.priority = get_priority(cat); // and the priority

                insert(heap, new_email); //puts the email in the heap
            }
        } 
        else if (strcmp(cmd, "NEXT") == 0) { //if the command is NEXT
            if (heap->size > 0) { //if theres stuff in the maxheap
                Email top = heap->arr[0]; //get the top email
                printf("[%s] Subject: %s | Date: %s\n", top.category, top.subject, top.date); //print the info
            } else { //else tell user theres no emails
                printf("No emails in queue.\n");
            }
        } 
        else if (strcmp(cmd, "READ") == 0) {
            if (heap->size > 0) { //if theres emails in the heap, remove the top one
                Email read_email = extract_max(heap);
            } else {
                printf("No emails to read.\n");
            }
        } 
        else if (strcmp(cmd, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap->size);
        }
    }

    fclose(file); //closes the file
    free_heap(heap); //garbage collects
    return 0; //end of code.
}
