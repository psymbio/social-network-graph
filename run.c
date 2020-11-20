#include <stdio.h>
#include "sql.h"
#include "graph.h"

// Driver program to test above functions
int main()
{
    // setup the database connection

    // structure defined in mysql.h used to handle connection
    MYSQL *conn;
    MYSQL_ROW row;
    MYSQL_RES *result;

    // initialize the connection
    conn = mysql_init(NULL);

    // connecting code to the server
    // error handling
    if (!(mysql_real_connect(conn, host, user, password, dbname, port, unix_socket, flag)))
    {
        fprintf(stderr, "\nError %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        exit(1);
    }

    int choice = 0;
    int login_status = 0;
    printf("1. Enter 1 to log in with existing account.\n");
    printf("2. Enter 2 to sign up.\n");
    printf("3. Enter 3 to exit.\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        login_status = login(conn, row);
        while (login_status != 1)
        {
            printf("\nThe password and username you've entered don't match.\nPress any enter to continue.\n");
            getchar();
            login_status = login(conn, row);
        }
        printf("Login successful!\n");
        getchar();
        getchar();
        break;
    case 2:
        sign_up(conn, row);
        break;
    case 3:
        exit(0);
        break;
    default:
        printf("Not a valid input... Exiting.");
        exit(0);
    }
    // creating the social network graph
    char uid[50];
    if (mysql_query(conn, "SELECT id FROM user_database ORDER BY ID DESC LIMIT 1") != 0)
    {
        printf("Query failed  with the following message:\n");
        printf("%s\n", mysql_error(conn));
        exit(1);
    }
    result = mysql_store_result(conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        strcpy(uid, row[0]);
    }
    int V = atoi(uid) + 1;
    int user_number = atoi(uid) + 1;
    struct Graph *graph = createGraph(V);
    for (int i = 1; i < user_number; i++)
    {
        int r = rand() % 200; // roughly the friends a user might have
        for (int j = 0; j < r; j++)
        {
            int x = rand() % user_number;
            add_friend(graph, i, x);
            // printf("\ndid it for %d and %d", i, x);
        }
    }

    // print the adjacency list representation of the above graph

    int menu_choice = 0;
    while (1)
    {
        printf("1. Enter 1 to view your friends.\n");
        printf("2. Enter 2 to view friend recommendations.\n");
        printf("3. Enter 3 to make a new friend.\n");
        printf("4. Enter 4 to view a user's information.\n");
        printf("5. Enter 5 to view a user's friends.\n");
        printf("6. Enter 6 to view common friends of two users.\n");
        printf("7. Enter 7 to delete your account.\n");
        printf("8. Enter 8 to exit.\n");
        printf("Enter your choice: ");
        scanf("%d", &menu_choice);
        switch (menu_choice)
        {
        case 1:
            printf("\nYour friends are: \n");
            print_userid_friends(graph, atoi(current_uid), conn, row);
            getchar();
            getchar();
            break;
        case 2:
            print_friend_recommendations(graph, atoi(current_uid), conn, row);
            getchar();
            getchar();
            break;
        case 3:
            make_new_friend(graph, atoi(current_uid), conn, row);
            getchar();
            getchar();
            break;
        case 4:
            view_user_info(conn, row);
            getchar();
            getchar();
            break;
        case 5:
            view_user_friends(graph, conn, row);
            getchar();
            getchar();
            break;
        case 6:
            print_common_friends(graph, conn, row);
            getchar();
            getchar();
            break;
        case 7:
            delete_your_account(graph, conn);
            getchar();
            getchar();
            exit(0);
            break;
        case 8:
            exit(0);
            break;
        case 9:
            print_friend_graph(graph);
            getchar();
            getchar();
        default:
            printf("Not a valid input... Exiting.");
            exit(0);
        }
    }
}
