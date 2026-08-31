# R27 Test

## Task 0: Forking

I forked the given repository and made the required changes in my fork. The repository is public for evaluation.

---

## Task 1: Encoding and Decoding

### Understanding

The `en_dc.c` file contains the encoding and decoding functions used for communication. The existing implementation had incomplete logic, especially around pointers, buffer sizes and COBS encoding.

### Changes Made

I implemented/fixed:

- COBS encoding in `frame_encode()`
- COBS decoding in `frame_decode()`
- NULL pointer checks
- Output buffer overflow checks
- Invalid input checks
- Handling of zero bytes
- COBS blocks reaching `0xFF`

I also made sure that the decoded data matches the original input.

---

## Task 2: Multithreading and Synchronization

### Understanding

The project uses pthreads, mutexes, semaphores and a message queue for communication between the producer and consumer threads.

### Changes Made

In `queue.c` I fixed the message queue initialization, including the queue counters, mutex and semaphores.

In `mutex.c` I fixed the reader entry logic so that the first reader locks the shared resource while allowing multiple readers to enter at the same time.

In `main.c` I initialized the required mutex and condition variable properly and kept the existing producer/consumer structure.

---

## Task 3: Rover Control

### Understanding

The rover uses differential drive, so the left and right wheel velocities can be changed independently to control its direction.

### Changes Made

I completed `drive_to_target()` in `drive.c`.

The function now:

- Checks for invalid pointers and values
- Calculates the direction towards the target
- Calculates the heading error
- Handles heading wraparound
- Calculates suitable left and right wheel velocities
- Uses the existing rover control functions

I kept the existing structure of the project instead of changing the overall design.

---

## Task 4: Compile and Run

I built and tested the project using the provided build setup.

The generated test executable was:

```text
build/queue_test.exe


I ran it using:
./build/queue_test.exe

The provided four inputs were tested and all of them returned:
Success

I also ran:
git diff --check

and no whitespace errors were reported.





SOLUTION:
---------------------------------------------------------------------------------------------

Understanding-

The main goal of the test was to fix the incomplete parts of an existing C robotics project rather than write a new project from scratch.

I first went through the source files and the headers to understand how the different parts were connected. I then worked on the communication code, threading/synchronization and rover control separately.
---------------------------------------------------------------------------------------------

Thought Process-

For the encoder/decoder, I focused on understanding how COBS works and then checked the buffer boundaries and special cases.

For the threading part, I followed the existing producer-consumer design and fixed the synchronization instead of redesigning it.

For the rover part, I used the existing rover state and wheel-control functions and implemented the missing calculations in drive_to_target().

After making the changes, I ran the provided test executable and checked the Git diff for errors.
---------------------------------------------------------------------------------------------

Implementation

I implemented the changes directly in the existing source files while keeping the original project structure.

For the encoding and decoding task, I fixed the COBS logic and added checks for invalid inputs, NULL pointers and buffer limits. I also handled zero bytes and the 0xFF COBS block case.

For the multithreading task, I corrected the message queue initialization and synchronization logic in the existing producer-consumer structure. I also fixed the reader synchronization logic so that multiple readers can access the shared resource while maintaining proper locking.

For the rover control task, I completed drive_to_target() using the existing rover state and control functions. The function calculates the target direction and heading error, handles angle wraparound and generates the required wheel velocities.

Finally, I compiled and tested the project and used git diff --check to check for whitespace errors.
---------------------------------------------------------------------------------------------

AI / Online Help-

I used AI occasionally to understand parts of the existing code and to help debug build/test issues. The implementation was then tested locally using the provided project.