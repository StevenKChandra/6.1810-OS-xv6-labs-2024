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

#define NUMERATOR 3
#define DENUMERATOR 13

static char *lock_name[] = {
    "bcache 0",
    "bcache 1",
    "bcache 2",
    "bcache 3",
    "bcache 4",
    "bcache 5",
    "bcache 6",
    "bcache 7",
    "bcache 8",
    "bcache 9",
    "bcache 10",
    "bcache 11",
    "bcache 12",
};

int hash(int n) {
    return (NUMERATOR * n) % DENUMERATOR;
}

struct {
  struct spinlock lock;
  struct buf buf[NBUF];
  struct buf *free_buffer;

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf hash_table[DENUMERATOR];
  struct spinlock bucket_lock[DENUMERATOR];
} bcache;

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  for (int i = 0; i < DENUMERATOR; i++) {
    initlock(&bcache.bucket_lock[i], lock_name[i]);
    bcache.hash_table[i].next = &bcache.hash_table[i];
    bcache.hash_table[i].prev = &bcache.hash_table[i];

  }
  bcache.free_buffer = 0;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->next = bcache.free_buffer;
    bcache.free_buffer = b;
    initsleeplock(&b->lock, "buffer");
  }
}

// Allocates a new buffer from the free_buffer stack
struct buf *
bufferalloc() {
  struct buf *b;
  acquire(&bcache.lock);
  b = bcache.free_buffer;
  if (b == 0) {
    panic("bget: no buffers");
  }
  bcache.free_buffer = bcache.free_buffer->next;
  release(&bcache.lock);
  return b;
}

// Frees the buffer and returns it to the free_buffer stack
void
bufferfree(struct buf * b){
  acquire(&bcache.lock);
  b->next = bcache.free_buffer;
  bcache.free_buffer = b;
  release(&bcache.lock);
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int bucket;

  bucket = hash(blockno);
  acquire(&bcache.bucket_lock[bucket]);

  // Is the block already cached?
  for (b = bcache.hash_table[bucket].next; b != &bcache.hash_table[bucket]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bucket_lock[bucket]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.bucket_lock[bucket]);

  // Not cached.
  b = bufferalloc();
  acquire(&bcache.bucket_lock[bucket]);
  b->next = bcache.hash_table[bucket].next;
  b->next->prev = b;
  bcache.hash_table[bucket].next = b;
  b->prev = &bcache.hash_table[bucket];
  b->dev = dev;
  b->blockno = blockno;
  b->valid = 0;
  b->refcnt = 1;
  release(&bcache.bucket_lock[bucket]);
  acquiresleep(&b->lock);
  return b;
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
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  int bucket;
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);
  
  bucket = hash(b->blockno);
  
  acquire(&bcache.bucket_lock[bucket]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    bufferfree(b);
  }
  release(&bcache.bucket_lock[bucket]);
}

void
bpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}


