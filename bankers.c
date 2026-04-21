#include <stdio.h>

#define MAX 10

int n, m;

// ---------- NEED CALCULATION ----------
void calculateNeed(int max[][MAX], int alloc[][MAX], int need[][MAX]) {
    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            need[i][j] = max[i][j] - alloc[i][j];
}

// ---------- PRINT MATRIX ----------
void printMatrix(int mat[][MAX], char *name) {
    printf("\n%s:\n", name);
    for(int i=0;i<n;i++) {
        printf("P%d: ", i);
        for(int j=0;j<m;j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
}

// ---------- PRINT AVAILABLE ----------
void printAvailable(int avail[]) {
    printf("Available: ");
    for(int i=0;i<m;i++)
        printf("%d ", avail[i]);
    printf("\n");
}

// ---------- SAFETY ALGORITHM ----------
int isSafe(int alloc[][MAX], int need[][MAX], int avail[]) {
    int work[MAX], finish[MAX] = {0}, safeSeq[MAX];

    for(int i=0;i<m;i++) work[i] = avail[i];

    int count = 0;

    while(count < n) {
        int found = 0;

        for(int i=0;i<n;i++) {
            if(!finish[i]) {
                int canRun = 1;

                for(int j=0;j<m;j++) {
                    if(need[i][j] > work[j]) {
                        canRun = 0;
                        break;
                    }
                }

                if(canRun) {
                    for(int j=0;j<m;j++)
                        work[j] += alloc[i][j];

                    safeSeq[count++] = i;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        if(!found) {
            printf("\n[RESULT] System is UNSAFE (Deadlock possible)\n");
            return 0;
        }
    }

    printf("\n[RESULT] System is SAFE\nSafe Sequence: ");
    for(int i=0;i<n;i++)
        printf("P%d ", safeSeq[i]);
    printf("\n");

    return 1;
}

// ---------- RESOURCE REQUEST ----------
void requestResources(int alloc[][MAX], int max[][MAX], int avail[]) {
    int p, request[MAX], need[MAX][MAX];

    calculateNeed(max, alloc, need);

    printf("\nEnter process number: ");
    scanf("%d", &p);

    if(p < 0 || p >= n) {
        printf("Invalid process number\n");
        return;
    }

    printf("Enter request vector: ");
    for(int i=0;i<m;i++)
        scanf("%d", &request[i]);

    // Check request <= need
    for(int i=0;i<m;i++) {
        if(request[i] > need[p][i]) {
            printf("[ERROR] Request exceeds need\n");
            return;
        }
    }

    // Check request <= available
    for(int i=0;i<m;i++) {
        if(request[i] > avail[i]) {
            printf("[WAIT] Resources not available\n");
            return;
        }
    }

    // Backup for rollback
    int tempAvail[MAX], tempAlloc[MAX][MAX];

    for(int i=0;i<m;i++)
        tempAvail[i] = avail[i];

    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            tempAlloc[i][j] = alloc[i][j];

    // Tentative allocation
    for(int i=0;i<m;i++) {
        avail[i] -= request[i];
        alloc[p][i] += request[i];
    }

    // 🔥 FIX: Recalculate NEED after allocation
    calculateNeed(max, alloc, need);

    if(isSafe(alloc, need, avail)) {
        printf("[RESULT] Request GRANTED\n");
    } else {
        printf("[ROLLBACK] Unsafe state detected. Reverting...\n");

        // Restore from backup
        for(int i=0;i<m;i++)
            avail[i] = tempAvail[i];

        for(int i=0;i<n;i++)
            for(int j=0;j<m;j++)
                alloc[i][j] = tempAlloc[i][j];

        printf("[RESULT] Request DENIED\n");
    }
}

// ---------- DEADLOCK DETECTION ----------
void detectDeadlock(int alloc[][MAX], int max[][MAX], int avail[]) {
    int work[MAX], finish[MAX] = {0}, need[MAX][MAX];

    calculateNeed(max, alloc, need);

    for(int i=0;i<m;i++) work[i] = avail[i];

    int changed = 1;

    while(changed) {
        changed = 0;

        for(int i=0;i<n;i++) {
            if(!finish[i]) {
                int canFinish = 1;

                // 🔥 FIX: use NEED instead of ALLOCATION
                for(int j=0;j<m;j++) {
                    if(need[i][j] > work[j]) {
                        canFinish = 0;
                        break;
                    }
                }

                if(canFinish) {
                    for(int j=0;j<m;j++)
                        work[j] += alloc[i][j];

                    finish[i] = 1;
                    changed = 1;
                }
            }
        }
    }

    int deadlock = 0;
    printf("\nDeadlocked Processes: ");

    for(int i=0;i<n;i++) {
        if(!finish[i]) {
            printf("P%d ", i);
            deadlock = 1;
        }
    }

    if(!deadlock)
        printf("None");

    printf("\n");
}

// ---------- MAIN ----------
int main() {
    int alloc[MAX][MAX], max[MAX][MAX], need[MAX][MAX], avail[MAX];
    int choice;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter number of resources: ");
    scanf("%d", &m);

    if(n > MAX || m > MAX) {
        printf("Exceeds maximum limit\n");
        return 0;
    }

    printf("\nEnter Allocation Matrix:\n");
    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            scanf("%d",&alloc[i][j]);

    printf("\nEnter Maximum Matrix:\n");
    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            scanf("%d",&max[i][j]);

    // VALIDATION
    for(int i=0;i<n;i++) {
        for(int j=0;j<m;j++) {
            if(alloc[i][j] > max[i][j]) {
                printf("Error: Allocation > Maximum\n");
                return 0;
            }
        }
    }

    printf("\nEnter Available Resources:\n");
    for(int i=0;i<m;i++)
        scanf("%d",&avail[i]);

    while(1) {
        calculateNeed(max, alloc, need);

        printf("\n--- MENU ---\n");
        printf("1. Show Matrices\n");
        printf("2. Check Safe State\n");
        printf("3. Request Resources\n");
        printf("4. Detect Deadlock\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printMatrix(alloc, "Allocation");
                printMatrix(max, "Maximum");
                printMatrix(need, "Need");
                printAvailable(avail);
                break;

            case 2:
                isSafe(alloc, need, avail);
                break;

            case 3:
                requestResources(alloc, max, avail);
                break;

            case 4:
                detectDeadlock(alloc, max, avail);
                break;

            case 5:
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }
}