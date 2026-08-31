#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "read.h"
#include "en_dc.h"
#include "read_file.h"
#include "drive.h"

#define NUM_PRODUCERS 1
#define NUM_CONSUMERS 3

Message_Queue queue;
ReadWrite_Lock lock;

pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_available = PTHREAD_COND_INITIALIZER;

int producer_finished = 0;


/*
 * Producer
 *
 * Reads coordinates from the input file and places them
 * into the message queue.
 */
void *producer(void *arg)
{
    InputFile input;
    FileArgs *args = (FileArgs *)arg;

    if (input_file_open(&input, args->filename) != 0) {
        return NULL;
    }

    float x_coord;
    float y_coord;

    while (input_file_read(&input, &x_coord, &y_coord)) {

        Message msg = {0};

        /*
         * Store the two coordinates in the message.
         */
        memcpy(msg.data, &x_coord, sizeof(float));
        memcpy(msg.data + sizeof(float), &y_coord, sizeof(float));
        msg.length = 2 * sizeof(float);

        /*
         * Put the message into the queue.
         */
        if (message_queue_push(&queue, &msg) != 0) {
            continue;
        }

        pthread_mutex_lock(&message_mutex);
        pthread_cond_broadcast(&message_available);
        pthread_mutex_unlock(&message_mutex);
    }

    input_file_close(&input);

    /*
     * Send one empty message to each consumer.
     * This tells consumers that production has finished.
     */
    Message end_msg = {0};
    end_msg.length = 0;

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        message_queue_push(&queue, &end_msg);
    }

    pthread_mutex_lock(&message_mutex);
    producer_finished = 1;
    pthread_cond_broadcast(&message_available);
    pthread_mutex_unlock(&message_mutex);

    return NULL;
}


/*
 * Consumer
 *
 * Takes messages from the queue and decodes the coordinates.
 */
void *consumer(void *arg)
{
    int id = *(int *)arg;

    while (1) {

        Message msg;

        if (message_queue_pop(&queue, &msg) != 0) {
            break;
        }

        /*
         * Empty message means the producer has finished.
         */
        if (msg.length == 0) {
            break;
        }

        float x_coord;
        float y_coord;

        /*
         * Decode the coordinates from the message.
         */
        if (msg.length >= 2 * sizeof(float)) {
            memcpy(&x_coord, msg.data, sizeof(float));
            memcpy(&y_coord,
                   msg.data + sizeof(float),
                   sizeof(float));

            printf("Consumer %d received: %.2f %.2f\n",
                   id, x_coord, y_coord);
        }
    }

    return NULL;
}


/*
 * Drive writer
 *
 * Reads each target coordinate from the input file,
 * drives the rover to the target and writes the result.
 */
void *drive_write(void *arg)
{
    FileArgs *args = (FileArgs *)arg;
    InputFile input;

    if (input_file_open(&input, args->filename) != 0) {
        printf("Failed to open %s\n", args->filename);
        return NULL;
    }

    struct rover_state rover = {
        .position = {
            .latitude = 0.0f,
            .longitude = 0.0f,
            .altitude = 0.0f
        },
        .heading_rad = 0.0f
    };

    float x_coord;
    float y_coord;

    int final_status = DRIVE_REACHED_TARGET;

    while (input_file_read(&input, &x_coord, &y_coord)) {

        struct coordinate target;

        target.latitude = x_coord;
        target.longitude = y_coord;
        target.altitude = 0.0f;

        int status = drive_to_target(&rover, &target);

        float dx = target.latitude - rover.position.latitude;
        float dy = target.longitude - rover.position.longitude;
        float error = hypotf(dx, dy);

        final_status = status;

        input_file_write(
            &input,
            &rover.position.latitude,
            &rover.position.longitude,
            &error,
            &status
        );
    }

    input_file_close(&input);

    if (final_status == DRIVE_REACHED_TARGET) {
        printf("Success\n");
    } else {
        printf("Failed: drive status %d\n", final_status);
    }

    return NULL;
}


int main()
{
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    pthread_t drive_writers[NUM_PRODUCERS];

    int consumer_id[NUM_CONSUMERS] = {1, 2, 3};

    const char *testcases[] = {
        "input/testcase1.txt",
        "input/testcase2.txt",
        "input/testcase3.txt",
        "input/testcase4.txt"
    };

    const char *result_tc[] = {
        "result/result1.txt",
        "result/result2.txt",
        "result/result3.txt",
        "result/result4.txt"
    };

    /*
     * Initialize reader-writer lock.
     */
    if (rwlock_init(&lock) != 0) {
        printf("Reader writer synchronization failed\n");
        return 1;
    }

    /*
     * Initialize message queue.
     */
    if (message_queue_init(&queue) != 0) {
        printf("Queue initialization failed\n");
        rwlock_destroy(&lock);
        return 1;
    }

    /*
     * Run all four test cases.
     */
    for (int testcase = 0; testcase < 4; testcase++) {

        printf("Input : %d\n\n", testcase + 1);

        FileArgs file_args = {
            .id = testcase + 1,
            .filename = testcases[testcase],
            .result_filename = result_tc[testcase]
        };

        producer_finished = 0;

        /*
         * Start producer.
         */
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            pthread_create(
                &producers[i],
                NULL,
                producer,
                &file_args
            );
        }

        /*
         * Start consumers.
         */
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            pthread_create(
                &consumers[i],
                NULL,
                consumer,
                &consumer_id[i]
            );
        }

        /*
         * Start drive writer.
         */
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            pthread_create(
                &drive_writers[i],
                NULL,
                drive_write,
                &file_args
            );
        }

        /*
         * Wait for producer.
         */
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            pthread_join(producers[i], NULL);
        }

        /*
         * Wait for consumers.
         */
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            pthread_join(consumers[i], NULL);
        }

        /*
         * Wait for drive writer.
         */
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            pthread_join(drive_writers[i], NULL);
        }
    }

    /*
     * Clean up.
     */
    message_destroy(&queue);
    rwlock_destroy(&lock);

    pthread_mutex_destroy(&message_mutex);
    pthread_cond_destroy(&message_available);

    return 0;
}