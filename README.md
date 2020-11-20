# Social Network Graph

A social network website with thousands of users. With the use of a directed graph it stores the information of how people are connected to each other.

This project allows users to:

+ Create an account.

+ Login with a previously created account.

+ View your friends.

+ Get friend recommendations on the basis of your friends.

+ Make new friends.

+ View another user's profile and friends.

// not done

+ View common friends of two users.

+ Delete your account.

This is a linux friendly project:)

To run it:

```
sudo apt update
sudo apt install build-essential
which gcc
sudo apt-get install mysql-server
sudo apt-get install libmysqlclient-dev
```

Then simply clone the repository on your machine, and `cd` to its directory.

```
gcc -o run $(mysql_config --cflags) run.c $(mysql_config --libs)
./run
```