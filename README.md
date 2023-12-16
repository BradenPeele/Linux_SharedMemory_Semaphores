prod_cons_Exec.png shows the exuction of the project

The producer will input a file of passwords and a number of passwords to transfer to the consumer through shared memory. It will also encrypt the passwords. Once done it will use a semaphore to alert the consumer.
The consumer will get the passwords from shared memory, decrypt them, and store them in a file. 
