// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13  // size of hash table
#define HASH(x) ((x) % NBUCKET)

struct {
  struct spinlock locks[NBUCKET];
  struct buf buf[NBUF];
  struct buf heads[NBUCKET]; // hash table of buffers
} bcache;

void
binit(void)
{
  struct buf *b;

  // initialize locks
  char *name = "bcache0";
  for(int i=0; i<NBUCKET; i++){
    initlock(&bcache.locks[i], name);
    name[6]++;
  }

  // Create hash table of buffers
  for(int i=0; i<NBUCKET; i++){
    bcache.heads[i].prev = &bcache.heads[i];
    bcache.heads[i].next = &bcache.heads[i];
  }
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    int i = HASH(b->blockno);
    b->next = bcache.heads[i].next;
    b->prev = bcache.heads + i;
    initsleeplock(&b->lock, "buffer");
    bcache.heads[i].next->prev = b;
    bcache.heads[i].next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  int i = HASH(blockno);
  acquire(&bcache.locks[i]);

  // Is the block already cached?
  for(b = bcache.heads[i].next; b != &bcache.heads[i]; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.locks[i]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.locks[i]);

  // Not cached.
  for(int j = 0; j<NBUCKET; j++){
    acquire(&bcache.locks[j]);
    for(b = bcache.heads[j].prev; b != &bcache.heads[j]; b = b->prev){
      if(b->refcnt == 0) {
        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;
        if (i == j) {
          release(&bcache.locks[j]);
        } else {
          b->next->prev = b->prev;
          b->prev->next = b->next;
          release(&bcache.locks[j]);
          acquire(&bcache.locks[i]);
          b->next = bcache.heads[i].next;
          b->prev = &bcache.heads[i];
          bcache.heads[i].next->prev = b;
          bcache.heads[i].next = b;
          release(&bcache.locks[i]);
        }
        acquiresleep(&b->lock);
        return b;
      }
    }
    release(&bcache.locks[j]);
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int i = HASH(b->blockno);
  acquire(&bcache.locks[i]);
  b->refcnt--;
  // if (b->refcnt == 0) {
  //   // no one is waiting for it.
  //   b->next->prev = b->prev;
  //   b->prev->next = b->next;
  //   b->next = bcache.heads[i].next;
  //   b->prev = &bcache.heads[i];
  //   bcache.heads[i].next->prev = b;
  //   bcache.heads[i].next = b;
  // }
  release(&bcache.locks[i]);
}

void
bpin(struct buf *b) {
  int i = HASH(b->blockno);
  acquire(&bcache.locks[i]);
  b->refcnt++;
  release(&bcache.locks[i]);
}

void
bunpin(struct buf *b) {
  int i = HASH(b->blockno);
  acquire(&bcache.locks[i]);
  b->refcnt--;
  release(&bcache.locks[i]);
}


