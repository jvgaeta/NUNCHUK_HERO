/* queue.c --- 
 * 
 * Filename: queue.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Feb  7 19:49:26 2013 (-0500)
 * Version: 
 * Last-Updated: 04/03/2015 
 *           By: Jordan Gaeta and Sebastian Kagemann
 *     Update #: 0
 * URL: 
 * Doc URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
#include "queue.h"

// Initialize the queue
void init_queue(queue_t *buf) {
	buf->head = 0;
	buf->tail = 0;
	int i;
	for(i = 0; i < QUEUE_SIZE; i++) {
		buf->buffer[i] = 0;
	}
}

// Adds an int to the buffer. This function needs to return 1 when the character was successfully added and 0 when the buffer did not have room to add the character.
// Move the tail
int enqueue (queue_t *buf, int data) {
	if(((buf->tail % QUEUE_SIZE) != (buf->head % QUEUE_SIZE)) || queue_empty(buf)) {
		buf->buffer[buf->tail++ % QUEUE_SIZE] = data;
		return 1;
	} else {
		return 0;
	}
}

// Remove a character from the buffer. If the buffer is empty, 0 should be returned.
// Move the head
int dequeue (queue_t *buf) {
	if(!queue_empty(buf)) {
		int tmp = buf->buffer[buf->head % QUEUE_SIZE];
		buf->buffer[buf->head++ % QUEUE_SIZE] = 0;
		return tmp;
	} else {
		return 0;
	}
}

// Returns 1 if the buffer is empty and 0 otherwise. 
int queue_empty(queue_t *buf) {
	return ((buf->tail % QUEUE_SIZE) == (buf->head % QUEUE_SIZE));
}


/* queue.c ends here */
