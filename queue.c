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
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

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
