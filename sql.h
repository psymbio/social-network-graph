#include <mysql/mysql.h>
#include <stdio.h>

char sql_statement[2048];

// database MYSQL

// login information of the local sql database
static char *host = "localhost";
static char *user = "vanillaskies";
static char *password = "switch";
static char *dbname = "project";

// find port number: netstat -tlnp
unsigned int port = 3306;

// connection type
static char *unix_socket = NULL;

// specifies db odbc
unsigned int flag = 0;

void finish_with_error(MYSQL *conn)
{
    fprintf(stderr, "%s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}