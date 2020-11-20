#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include "string_handling.h"

char current_uid[50];
static struct termios old, current;

// A structure to represent an user node
struct user_node
{
    int dest;
    struct user_node *next;
};

// A structure to represent the list of users
struct user_list
{
    struct user_node *head; // pointer to head node of list
};

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct user_list *array;
};

// A utility function to create a new adjacency list node
struct user_node *newuser_node(int dest)
{
    struct user_node *newNode = (struct user_node *)malloc(sizeof(struct user_node));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

// A utility function that creates a graph of V vertices
struct Graph *createGraph(int V)
{
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    graph->V = V;

    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct user_list *)malloc(V * sizeof(struct user_list));

    // Initialize each adjacency list as empty by making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

// Adds an edge to an undirected graph
void add_friend(struct Graph *graph, int src, int dest)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    struct user_node *newNode = newuser_node(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Since graph is undirected, add an edge from dest to src also
    newNode = newuser_node(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

int find_user(struct Graph *graph, int src)
{
    for (int v = 0; v < graph->V; ++v)
    {
        struct user_node *pCrawl = graph->array[v].head;
        while (pCrawl)
        {
            if (pCrawl->dest == src)
                return 1;
            pCrawl = pCrawl->next;
        }
    }
}

void delete_user(struct Graph *graph, int a)
{
    if (find_user(graph, a))
    {
        for (int v = 0; v < graph->V; ++v)
        {
            struct user_node *aCrawl = graph->array[v].head;
            if (v == a)
            {
                free(graph->array);
            }
        }
    }
}
void delete_your_account(struct Graph *graph, MYSQL *conn)
{
    delete_user(graph, atoi(current_uid));
    sprintf(sql_statement, "DELETE FROM user_database where id=%d", atoi(current_uid));
    if (mysql_query(conn, sql_statement))
    {
        finish_with_error(conn);
    }
    printf("\nGood luck with your dead social life bud.\n");
}
// A utility function to print the adjacency list representation of graph
void print_friend_graph(struct Graph *graph)
{
    int v;
    for (v = 0; v < graph->V; ++v)
    {
        struct user_node *pCrawl = graph->array[v].head;
        printf("\nFriends of %d are:\n", v);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}
void print_userid_friends(struct Graph *graph, int v, MYSQL *conn, MYSQL_ROW row)
{
    int status = 0;
    struct user_node *pCrawl = graph->array[v].head;
    while (pCrawl)
    {
        sprintf(sql_statement, "SELECT username, first_name, last_name FROM user_database WHERE id=%d", pCrawl->dest);
        if (mysql_query(conn, sql_statement))
        {
            finish_with_error(conn);
        }
        MYSQL_RES *result = mysql_store_result(conn);
        while ((row = mysql_fetch_row(result)) != NULL)
        {
            printf("Username: %s\t", row[0]);
            printf("Name: %s %s\n", row[1], row[2]);
        }
        pCrawl = pCrawl->next;
    }
}
void find_common_friends(struct Graph *graph, int a, int b, MYSQL *conn, MYSQL_ROW row)
{
    struct user_node *aCrawl = graph->array[a].head;
    struct user_node *bCrawl = graph->array[b].head;
    while (aCrawl || bCrawl)
    {
        if (aCrawl->dest == bCrawl->dest)
        {
            sprintf(sql_statement, "SELECT username, first_name, last_name FROM user_database WHERE id=%d", aCrawl->dest);
            if (mysql_query(conn, sql_statement))
            {
                finish_with_error(conn);
            }
            MYSQL_RES *result = mysql_store_result(conn);
            while ((row = mysql_fetch_row(result)) != NULL)
            {
                printf("Username: %s\t", row[0]);
                printf("Name: %s %s\n", row[1], row[2]);
            }
        }
        aCrawl = aCrawl->next;
        bCrawl = bCrawl->next;
    }
}
void print_common_friends(struct Graph *graph, MYSQL *conn, MYSQL_ROW row)
{
    char user_a[50];
    char user_b[50];
    char user_a_uid[50];
    char user_b_uid[50];
    printf("Enter the first user: ");
    scanf("%s", user_a);
    printf("Enter the second user: ");
    scanf("%s", user_b);
    sprintf(sql_statement, "SELECT id FROM user_database WHERE username='%s'", user_a);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(user_a_uid, row[0]);
    }
    // mysql_free_result(result);
    sprintf(sql_statement, "SELECT id FROM user_database WHERE username='%s'", user_b);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(user_b_uid, row[0]);
    }
    // mysql_free_result(result);
    printf("The common friends of %s and %s are:\n", user_a, user_b);
    find_common_friends(graph, atoi(user_a_uid), atoi(user_b_uid), conn, row);
}
void print_friend_recommendations(struct Graph *graph, int v, MYSQL *conn, MYSQL_ROW row)
{
    struct user_node *pCrawl = graph->array[v].head;
    while (pCrawl)
    {
        if (pCrawl->dest != v)
        {
            sprintf(sql_statement, "SELECT username, first_name, last_name FROM user_database WHERE id=%d", pCrawl->dest);
            if (mysql_query(conn, sql_statement))
            {
                finish_with_error(conn);
            }
            MYSQL_RES *result = mysql_store_result(conn);
            while ((row = mysql_fetch_row(result)) != NULL)
            {
                printf("Recommendation from username: %s\t", row[0]);
                printf("Name: %s %s\n", row[1], row[2]);
                print_userid_friends(graph, pCrawl->dest, conn, row);
                printf("\n\n\n");
            }
        }
        pCrawl = pCrawl->next;
    }
}

void view_user_friends(struct Graph *graph, MYSQL *conn, MYSQL_ROW row)
{
    char user[50];
    char user_uid[50];
    printf("\nEnter a username to find friends of: ");
    scanf("%s", user);
    sprintf(sql_statement, "SELECT id FROM user_database WHERE username='%s'", user);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(user_uid, row[0]);
    }
    printf("\n%s's friends are: \n", user);
    print_userid_friends(graph, atoi(user_uid), conn, row);
}
void make_new_friend(struct Graph *graph, int v, MYSQL *conn, MYSQL_ROW row)
{
    char new_friend[50];
    char friend_uid[50];
    printf("Enter a username to be friends with: ");
    scanf("%s", new_friend);
    sprintf(sql_statement, "SELECT id FROM user_database WHERE username='%s'", new_friend);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(friend_uid, row[0]);
    }
    add_friend(graph, atoi(current_uid), atoi(friend_uid));
    printf("\nYou've made a new friend!\n");
}

void view_user_info(MYSQL *conn, MYSQL_ROW row)
{
    char view_info_user[50];
    printf("\nEnter a username to view information of: ");
    scanf("%s", view_info_user);
    sprintf(sql_statement, "SELECT username, first_name, last_name, dob, since, lives_in, college, email, phone_number FROM user_database WHERE username='%s'", view_info_user);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        printf("Username: %s\t", row[0]);
        printf("Name: %s %s\n", row[1], row[2]);
        printf("Born in: %s\tUser since:%s\n", row[3], row[4]);
        printf("Lives in: %s\tCollege:%s\tEmail: %s\tPhone number: %s\n\n", row[5], row[6], row[7], row[8]);
    }
}
int login(MYSQL *conn, MYSQL_ROW row)
{
    char username[50];
    char password[50];
    printf("Enter your username : ");
    scanf("%s", username);
    // remember the size of the getpass password string needs to be same or lesser than the pwd string
    strncpy(password, getpass("Enter the password: "), 50);
    sprintf(sql_statement, "SELECT id FROM user_database WHERE username='%s'", username);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(current_uid, row[0]);
    }
    sprintf(sql_statement, "SELECT password FROM user_database WHERE username='%s'", username);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        if (strcmp(row[0], password) == 0)
            return 1;
    }
}

int sign_up(MYSQL *conn, MYSQL_ROW row)
{
    char uid[50];
    if (mysql_query(conn, "SELECT id FROM user_database ORDER BY ID DESC LIMIT 1") != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(uid, row[0]);
    }
    char *new_uid = string_test(uid);
    char username[50], password[50], first_name[50], last_name[50], gender[50];
    char dob[50], phone_number[50], email[50], lives_in[50], college[50];
    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your password: ");
    scanf("%s", password);
    printf("Enter your first name: ");
    scanf("%s", first_name);
    printf("Enter your last name: ");
    scanf("%s", last_name);
    printf("Enter your gender(Male/Female): ");
    scanf("%s", gender);
    printf("Enter your date of birth(YYYY-MM-DD): ");
    scanf("%s", dob);
    printf("Enter your mobile number(XXX-XXX-XXXX): ");
    scanf("%s", phone_number);
    printf("Enter your email: ");
    scanf("%s", email);
    printf("Enter your address: ");
    scanf("%s", lives_in);
    printf("Enter your college: ");
    scanf("%s", college);
    sprintf(sql_statement, "INSERT INTO user_database(id, username, password, first_name, last_name, gender, dob, since, phone_number, email, lives_in, college) VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '2020-11-29', '%s','%s', '%s', '%s')", new_uid, username, password, first_name, last_name, gender, dob, phone_number, email, lives_in, college);
    if (mysql_query(conn, sql_statement) != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
    }
    else
    {
        printf("Great! you've signed up. The program shall exit now, next time log in with your new credentials.\n");
        exit(0);
    }
}
