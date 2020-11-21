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

// structure to represent an user node
struct user_node
{
    int dest;
    struct user_node *next;
};

// structure to represent the list of users
struct user_list
{
    struct user_node *head;
};

// graph is an array of adjacency lists.
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

// create a graph of V vertices
struct Graph *createGraph(int V)
{
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    graph->V = V;
    graph->array = (struct user_list *)malloc(V * sizeof(struct user_list));
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    return graph;
}

// add an edge to an undirected graph
void add_friend(struct Graph *graph, int src, int dest)
{
    // add edge from src to dest
    struct user_node *newNode = newuser_node(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
    // add edge from dest to src
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
/*
void remove_friend(struct Graph *graph, int a, int b)
{
    if(find_user(graph, src) && find_user(graph, dest))
    {
        struct user_node *aCrawl = graph->array[v].head;
        struct user_node *bCrawl = graph->array[v].head;
        while (aCrawl || bCrawl)
        {
            if(aCrawl->dest == b)
                free(graph->array);
            aCrawl = aCrawl->next;
            if(bCrawl->dest == a)
                free(graph->array);
            bCrawl = bCrawl->next;
        }
    }
}
*/
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
