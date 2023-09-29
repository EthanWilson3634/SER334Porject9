#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_MAX 2147483647

// Estimate the next t value by using the following formula
// tau_n+1 = at_n + (1 - a)tau_n
int estimate_next_tau(float alpha, float t_n, float tau_n){
    float tau = (alpha*t_n + (1-alpha)*tau_n);
    return (int) tau;
}

// Helper for calculating absolute value
int abs(int num){
    if (num > 0){
        return num * -1;
    } else {
        return num;
    }
}

int main(int argc, char* argv[]) {
    // Open input file
    FILE* file = fopen(argv[1], "r");
    if(file == NULL){
        perror("Could not open file.\n");
        return 1;
    }
    // Declare main variables
    int tick_count;
    int process_count;
    fscanf(file, "%d%*c", &tick_count);
    fscanf(file, "%d%*c", &process_count);

    float a_values[process_count];
    int tau_values[process_count][tick_count];
    int t_values[process_count][tick_count];

    // Dynamically read tau, alpha, and t values and discard newline characters
    for(int i = 0; i < process_count; i++){
        fscanf(file, "%*d%*c%d%*c", &tau_values[i][0]);
        fscanf(file, "%f%*c", &a_values[i]);

        for(int j = 0; j < tick_count; j++){
            fscanf(file, "%d%*c", &t_values[i][j]);
        }
    }

    int t_values_live[process_count][tick_count];
    memcpy(t_values_live, t_values, sizeof(t_values));
    ///////////////////////////////////////////////////////////////////////////////
    printf("==Shortest-Job-First==\n");
    int current_time = 0;
    int turnaround_time = 0;
    int waiting_time = 0;

    int shortest = 0;
    for(int i = 0; i < tick_count; i++){
        printf("simulating %dth tick of processes @ time %d:\n", i, current_time);
        int sum_completed_processes = 0; // For calculating turnaround time, resets after each tick
        int sum_tick_waiting = 0; // For calculating waiting time per process per tick
        for(int j = 0; j < process_count; j++){
            for(int k = 0; k < process_count; k++){
                // Find the shortest value
                if(t_values[shortest][i] > t_values[k][i]){
                    shortest = k;
                }
            }
            // Add to the cumulative turnaround time
            sum_completed_processes += t_values[shortest][i];
            turnaround_time += sum_completed_processes;
            // Add to the waiting time if not printing the first process
            if(j != 0){
                waiting_time += sum_tick_waiting;
            }

            printf("  Process %d took %d\n", shortest, t_values[shortest][i]);

            current_time += t_values[shortest][i];
            sum_tick_waiting += t_values[shortest][i];
            t_values[shortest][i] = INT_MAX; // Mark it as read
        }
    }
    printf("Turnaround time: %d\n", turnaround_time);
    printf("Waiting time: %d\n\n", waiting_time);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    printf("==Shortest-Job-First-Live==\n");
    int estimation_error = 0;
    current_time = 0;
    turnaround_time = 0;
    waiting_time = 0;

    for(int i = 0; i < tick_count; i++){
        printf("simulating %dth tick of processes @ time %d:\n", i, current_time);
        int sum_completed_processes = 0; // For calculating turnaround time, resets after each tick
        int sum_tick_waiting = 0; // For calculating waiting time per process per tick

        // Initialize tau values for the next tick
        for(int k = 0; k < process_count; k++){
            tau_values[k][i+1] = estimate_next_tau(a_values[k], (float) t_values_live[k][i], (float) tau_values[k][i]);
        }

        for(int j = 0; j < process_count; j++){

            // Find the shortest value
            for(int l = 0; l < process_count; l++){
                if(tau_values[shortest][i] > tau_values[l][i]){
                    shortest = l;
                }
            }

            // Add to the cumulative turnaround time
            sum_completed_processes += t_values_live[shortest][i];
            turnaround_time += sum_completed_processes;
            // Add to the waiting time if not printing the first process
            if(j != 0){
                waiting_time += sum_tick_waiting;
            }

            printf("  Process %d was estimated for %d took %d\n", shortest, tau_values[shortest][i], t_values_live[shortest][i]);

            estimation_error += abs(tau_values[shortest][i] - t_values_live[shortest][i]);
            current_time += t_values_live[shortest][i];
            sum_tick_waiting += t_values_live[shortest][i];
            tau_values[shortest][i] = INT_MAX; // Mark it as read
        }
    }
    printf("Turnaround time: %d\n", turnaround_time);
    printf("Waiting time: %d\n", waiting_time);
    printf("Estimation Error: %d\n\n", estimation_error);
    //////////////////////////////////////////////////////////////////////////////////////////

    fclose(file);
    return 0;
}