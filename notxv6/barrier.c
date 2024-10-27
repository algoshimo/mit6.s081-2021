#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static int nthread = 1;
static int round = 0;

struct barrier {
  pthread_mutex_t barrier_mutex;
  pthread_cond_t barrier_cond;
  int nthread;      // Number of threads that have reached this round of the barrier
  int round;     // Barrier round
} bstate;

static void
barrier_init(void)
{
  assert(pthread_mutex_init(&bstate.barrier_mutex, NULL) == 0);
  assert(pthread_cond_init(&bstate.barrier_cond, NULL) == 0);
  bstate.nthread = 0;
}

static void 
barrier()
{
  pthread_mutex_lock(&bstate.barrier_mutex);  // 锁定互斥锁
  bstate.nthread++;  // 增加已到达屏障的线程数量

  if (bstate.nthread == nthread) {  // 检查是否所有线程都到达屏障
    bstate.nthread = 0;  // 重置计数器，为下一轮屏障做好准备
    bstate.round++;      // 增加屏障轮数
    pthread_cond_broadcast(&bstate.barrier_cond); // 唤醒所有等待线程
  } else {
    int current_round = bstate.round;  // 记录当前轮次
    while (current_round == bstate.round) {  // 等待轮次更新
      pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex);
    }
  }
  pthread_mutex_unlock(&bstate.barrier_mutex);  // 解锁互斥锁
  
}

static void *
thread(void *xa)
{
  long n = (long) xa;
  long delay;
  int i;

  for (i = 0; i < 20000; i++) {
    int t = bstate.round;
    assert (i == t);
    barrier();
    usleep(random() % 100);
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  long i;
  double t1, t0;

  if (argc < 2) {
    fprintf(stderr, "%s: %s nthread\n", argv[0], argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);

  barrier_init();

  for(i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void *) i) == 0);
  }
  for(i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0);
  }
  printf("OK; passed\n");
}
