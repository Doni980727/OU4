/**
 * Datastructure and Algorithms.
 * Program:     is_connected.c
 * Authors:     Lovisa Nordström, Anna Olsson, Dino Felarca
 * CS Username: id23lnm, id23aon, ens21dfa
 * Description: The program reads the graph from a map file,
 * 				interacts with the user and does a breadth-first search in the graph.
 * Date:        2023-03-14.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include "graph.h"
#include "array_1d.h"
#include "dlist.h"
#include "queue.h"

/**
 * find_path() - Find a path between two nodes in a graph using breadth-first search.
 * @g: Graph to search.
 * @src: Source node.
 * @dest: Destination node.
 *
 * This function searches for a path between @src and @dest nodes in the graph @g
 * using breadth-first search algorithm.
 *
 * Returns: True if a path exists, false otherwise.
 */
bool find_path(graph *g, node *src, node *dest)
{
	// Initialize a queue for breadth-first search
	queue *q = queue_empty(NULL);
	bool dest_reached;

	// Mark the source node as seen and enqueue it
	g = graph_node_set_seen(g, src, true);
	q = queue_enqueue(q, src);

	// Breadth-first search loop
	while (!queue_is_empty(q))
	{
		node *current_node = queue_front(q);
		q = queue_dequeue(q);

		// Get neighbors of the current node
		dlist *get_neighbours = graph_neighbours(g, current_node);
		dlist_pos pos = dlist_first(get_neighbours);

		while (!dlist_is_end(get_neighbours, pos))
		{
			node *next_node = dlist_inspect(get_neighbours, pos);

			// If the neighbor hasn't been seen, mark it as seen and enqueue it
			if (!graph_node_is_seen(g, next_node))
			{
				g = graph_node_set_seen(g, next_node, true);
				q = queue_enqueue(q, next_node);
			}

			// Check if destination node has been reached
			dest_reached = nodes_are_equal(next_node, dest);

			if (dest_reached == true)
			{
				// Cleanup and return if destination is reached
				dlist_kill(get_neighbours);
				g = graph_reset_seen(g);
				queue_kill(q);
				return true;
			}

			pos = dlist_next(get_neighbours, pos);
		}

		// Clean up the neighbor list for the current node
		dlist_kill(get_neighbours);
	}

	// Cleanup and reset seen status if destination is not reached
	queue_kill(q);
	g = graph_reset_seen(g);
	return false;
}

/**
 * search_path() - Interactively search for a path between nodes in the graph.
 * @g: Graph to search.
 *
 * This function interacts with the user to find a path between nodes in the graph @g.
 * It prompts the user to input origin and destination nodes and displays the result.
 *
 * Returns: None.
 */
void search_path(graph *g)
{
	char s1[40];
	char s2[40];
	char search[100];

	while (1)
	{
		printf("Enter origin and destination (quit to exit): ");
		fgets(search, sizeof(search), stdin);
		sscanf(search, "%s %s", s1, s2);

		if (strcmp(s1, "quit") == 0 || strcmp(s1, "exit") == 0)
		{
			printf("Normal exit.\n\n");
			graph_kill(g);
			exit(EXIT_SUCCESS);
		}
		else if (sscanf(search, "%s %s", s1, s2) != 2)
		{
			printf("Two nodes need to be entered. Try again.\n\n");
		}
		else if (graph_find_node(g, s1) == NULL)
		{
			printf("Node %s does not exist in the graph.\n\n", s1);
		}
		else if (graph_find_node(g, s2) == NULL)
		{
			printf("Node %s does not exist in the graph.\n\n", s2);
		}
		else
		{
			if (strcmp(s1, s2) == 0)
			{
				printf("There is a path from %s to %s.\n\n", s1, s2);
			}
			else
			{
				node *source = graph_find_node(g, s1);
				node *destination = graph_find_node(g, s2);

				if (find_path(g, source, destination) == true)
				{
					printf("There is a path from %s to %s.\n\n", s1, s2);
				}
				else
				{
					printf("There is no path from %s to %s.\n\n", s1, s2);
				}
			}
		}
	}
}

/**
 * main() - Entry point of the program.
 * @argc: Number of command-line arguments.
 * @argv: Array of command-line argument strings.
 *
 * This function reads a graph from a file specified as a command-line argument,
 * interacts with the user to search for paths between nodes in the graph,
 * and performs necessary cleanup before exiting.
 *
 * Returns: 0 on successful execution, non-zero on failure.
 */
int main(int argc, char *argv[])
{
	FILE *file;
	char buff[255];
	int edges;

	const char *filename = argv[1];
	file = fopen(filename, "r");

	if (file == NULL)
	{
		fprintf(stderr, "Failed to open %s\n", filename);
		exit(EXIT_FAILURE);
	}

	// Read the number of edges from the file
	while (fgets(buff, sizeof(buff), file) != NULL)
	{
		if (buff[0] == '#' || isspace(buff[0]))
		{
			continue;
		}
		else
		{
			sscanf(buff, "%d", &edges);
			break;
		}
	}

	if (edges == 0)
	{
		fprintf(stderr, "Missing number of edges, map doesn't follow the specification.\n");
		exit(EXIT_FAILURE);
	}

	// Create an empty graph
	struct graph *g = graph_empty(edges * edges);

	char *node_name1 = NULL;
	char *node_name2 = NULL;

	// Loop to extract node names and build the graph
	while (fgets(buff, sizeof(buff), file) != NULL)
	{
		if (buff[0] == '#' || isdigit(buff[0]) || isspace(buff[0]))
		{
			continue;
		}

		// Extract node names from the line
		node_name1 = strtok(buff, " \t\n");
		node_name2 = strtok(NULL, " \t\n");

		if (node_name2 == NULL || node_name2[0] == '#')
		{
			fprintf(stderr, "Missing a second node, map doesn't follow the specification.\n");
			exit(EXIT_FAILURE);
		}

		// Insert nodes into the graph if not already present
		graph_insert_node(g, node_name1);
		graph_insert_node(g, node_name2);

		// Retrieve nodes
		node *node1 = graph_find_node(g, node_name1);
		node *node2 = graph_find_node(g, node_name2);

		// Insert an edge between the nodes
		graph_insert_edge(g, node1, node2);
	}

	// Close the file
	if (fclose(file))
	{
		fprintf(stderr, "Failed to close %s: %s\n", filename, strerror(errno));
		return -1;
	}

	// Perform interactive search for paths between nodes
	search_path(g);

	// Clean up and free memory
	graph_kill(g);
	free(node_name1);
	free(node_name2);

	return 0;
}