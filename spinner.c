#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>

int update;
int term;

void handle_signals(int signum) {
  switch(signum) {
    case SIGALRM:
      update = 1;
      break;
    case SIGINT:
      term = 1;
      fprintf(stderr, " Terminating from signal %i!\n", signum);
      break;
    default:
      fprintf(stderr, " Did not handle signal %i!\n", signum);
  }
}

int main() {
  unsigned i;
  float rads = 0.f;
  size_t numthings = 16;
  int things[numthings];
  size_t bufsize = 64;
  char graphicbuf[bufsize];

  // Register signal handlers
  int signals[] = {SIGALRM, SIGUSR1, SIGUSR2, SIGINT};
  size_t numsignals = sizeof(signals)/sizeof(int);
  for(i = 0; i < numsignals; i++)
    (void)signal(signals[i], handle_signals);

  // Set up a timer to interrupt the program every 5ms
  struct timeval tt = {0, 5000};
  struct itimerval t = {tt,tt};
  setitimer(ITIMER_REAL, &t, NULL);

  // Configure a new signal mask where SIGALRM is ignored, so we can
  //  sigsuspend on it later. (Also make room to save a temporary mask.)
  sigset_t sigmask, sigoldmask;
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGALRM);

  term = 0;
  while(!term) {
    // Generate the new positions of all the things.
    //  This is essentially a simple sin(2*pi*1x), sin(2*pi*2x), ...
    //  except the value is also being scaled to make sense in the
    //  ASCII-graphical buffer in the same step.
    for(i = 0; i < numthings; i++) things[i] =
      (int)((sinf((2*M_PI*rads)*(1+i)/numthings)+1)*(bufsize-1)/2);

    // Fills string buf of size bufsize with characters based on the values of
    //  the numthings in things, essentially creating an ASCII-graphical
    //  representation of those numbers as if they were on a number line.
    for(i = 0; i < bufsize-1; i++)
      graphicbuf[i] = ' ';
    for(i = 0; i < numthings; i++)
      if (things[i] >= 0 && things[i] < (int)(bufsize-1)) {
        if(graphicbuf[things[i]] == ' ')
          graphicbuf[things[i]] = '+';
        else
          graphicbuf[things[i]] = '#';
      }
    graphicbuf[bufsize-1] = '\0';

    // Update the screen with the new ASCII graphics
    printf("  [%s]\r", graphicbuf);
    fflush(stdout);

    rads += 0.0005f;
    if (rads >= (float)numthings) rads = 0.f;

    // Stop executing the program until a signal interrupts that we care about.
    // (Also handle saving and restoring the signal mask correctly.)
    update = 0;
    sigprocmask(SIG_BLOCK, &sigmask, &sigoldmask);
    while(!update) sigsuspend(&sigoldmask);
    sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
  } 

  return 0;
}