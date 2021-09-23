# MapReduce-Style-Wordcount-Application
My project of CMPSC 473. 
\
\
This project is designed to give you experience in writing multi-threaded programs by implementing a simplified MapReduce-style wordcount application. By working on this project:\
• You will learn to write multi-threaded code that correctly deals with race conditions.\
• You will carry out a simple performance evaluation to examine the performance impact of (i) the degree of parallelism in the mapper stage and (ii) the size of the shared buffer which the two stages of your application will use to communicate.
\
\
\
[Hanfei He] , [hjh5321@psu.edu]\
\[name2] , [psu2Email]\
[name3] , [psu3Email]\
// myself
\
\
\
@Test1: [1], [1000], [0:06.14]\
@Test2: [2], [1000], [0:08.71]\
@Test3: [4], [1000], [0:16.81]\
@Test4: [8], [1000], [0:26.85]\
@Test5: [16], [1000], [0:29.83]\
@Test6: [32], [1000], [0:30.32]\
@Test7: [8], [100], [0:26.54]\
@Test8: [8], [1000], [0:26.45]\
@Test9: [8], [10000], [0:26.76]\
\
\
\
****SUCCESS: channel_test_initialization****\
****SUCCESS: sanitize_test_initialization****\
****SUCCESS: valgrind_test_initialization****\
****SUCCESS: channel_test_Free****\
****SUCCESS: sanitize_test_Free****\
****SUCCESS: valgrind_test_Free****\
****SUCCESS: channel_test_send_correctness****\
****SUCCESS: sanitize_test_send_correctness****\
****SUCCESS: valgrind_test_send_correctness****\
****SUCCESS: channel_test_receive_correctness****\
****SUCCESS: sanitize_test_receive_correctness****\
****SUCCESS: valgrind_test_receive_correctness****\
****SUCCESS: channel_test_overall_send_receive****\
****SUCCESS: sanitize_test_overall_send_receive****\
****SUCCESS: valgrind_test_overall_send_receive****\
****SUCCESS: channel_test_cpu_utilization_send****\
****SUCCESS: sanitize_test_cpu_utilization_send****\
****SUCCESS: valgrind_test_cpu_utilization_send****\
****SUCCESS: channel_test_cpu_utilization_receive****\
****SUCCESS: sanitize_test_cpu_utilization_receive****\
****SUCCESS: valgrind_test_cpu_utilization_receive****\
****SUCCESS: channel_test_channel_close_with_send****\
****SUCCESS: sanitize_test_channel_close_with_send****\
****SUCCESS: valgrind_test_channel_close_with_send****\
****SUCCESS: channel_test_channel_close_with_receive****\
****SUCCESS: sanitize_test_channel_close_with_receive****\
****SUCCESS: valgrind_test_channel_close_with_receive****\
****SUCCESS: channel_serialize****\
****SUCCESS: sanitize_serialize****\
****SUCCESS: valgrind_serialize****\
****SUCCESS: channel_test_correctness****\
****SUCCESS: sanitize_test_correctness****\
****SUCCESS: valgrind_test_correctness****\
Final score: 100\
