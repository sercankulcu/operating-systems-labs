#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define TIMER_INTERVAL_SEC  1    // Timer interval in seconds
#define TIMER_INTERVAL_NSEC 0    // Timer interval in nanoseconds

// Callback function for timer expiration
void timer_callback(int sig, siginfo_t *si, void *uc) {
    printf("Timer expired!\n");
}

int main() {
    struct sigaction sa;
    struct sigevent sev;
    timer_t timerid;
    struct itimerspec its;

    // Set up the signal handler for the timer
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_callback;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Set up the timer
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    // Configure the timer to expire after TIMER_INTERVAL_SEC seconds
    its.it_value.tv_sec = TIMER_INTERVAL_SEC;
    its.it_value.tv_nsec = TIMER_INTERVAL_NSEC;
    its.it_interval.tv_sec = TIMER_INTERVAL_SEC;
    its.it_interval.tv_nsec = TIMER_INTERVAL_NSEC;
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    // Keep the program running to allow the timer to trigger
    printf("Timer started. Waiting for expiration...\n");
    while (1)
        sleep(1);

    return 0;
}

