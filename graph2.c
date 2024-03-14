/**
 * Datastructure and Algorithms.
 * Program:     graph2.c
 * Authors:    	Lovisa Nordström, Anna Olsson, Dino Felarca
 * CS Username: id23lnm, id23aon, ens21dfa
 * Description: Implementation of a graph as a array of directed lists.
 * Date:        2023-03-14.
 * gcc -g -Wall is_connected.c -o is_connected -I ~/edu/doa/code_base/datastructures-v1.0.13.0/include/ ~/edu/doa/code_base/datastructures-v1.0.13.0/src/dlist/dlist.c ~/edu/doa/code_base/datastructures-v1.0.13.0/src/queue/queue.c ~/edu/doa/code_base/datastructures-v1.0.13.0/src/list/list.c ~/edu/doa/code_base/datastructures-v1.0.13.0/src/array_1d/array_1d.c  graph.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "graph.h"
#include "array_1d.h"
#include "dlist.h"
#include "queue.h"

// Structure representing a node in the graph
struct node
{
	dlist *neighbours;	// List of neighboring nodes
	bool seen;			// Flag to track whether the node has been seen
	char *name;			// Name of the node
};

// Structure representing a graph
struct graph
{
	struct array_1d *nodes; 	// Array of nodes
	int max_nodes;				// Maximum number of nodes allowed in the graph
};

/**
 * nodes_are_equal() - Check if two nodes are equal.
 * @n1: First node.
 * @n2: Second node.
 *
 * Returns: true if the nodes are equal, false otherwise.
 */
bool nodes_are_equal(const node *n1, const node *n2)
{
	return n1 == n2;
}

/**
 * graph_empty() - Create an empty graph.
 * @max_nodes: Maximum number of nodes in the graph.
 *
 * Returns: A pointer to the created graph.
 */
graph *graph_empty(int max_nodes)
{
	graph *g = calloc(1, sizeof(*g));
	array_1d *a = array_1d_create(0, (max_nodes - 1), NULL);
	g->max_nodes = max_nodes;
	g->nodes = a;
	return g;
}

/**
 * graph_is_empty() - Check if the graph is empty.
 * @g: Graph to inspect.
 *
 * Returns: true if the graph is empty, false otherwise.
 */
bool graph_is_empty(const graph *g)
{
	return !array_1d_has_value(g->nodes, array_1d_low(g->nodes));
}

/**
 * graph_insert_node() - Insert a node into the graph.
 * @g: Graph to insert the node into.
 * @s: Name of the node to insert.
 *
 * Returns: A pointer to the modified graph.
 */
graph *graph_insert_node(graph *g, const char *s)
{
	node *n = malloc(sizeof(*n));
	n->name = malloc(sizeof(n->name));
	n->neighbours = dlist_empty(NULL);
	n->seen = false;
	strcpy(n->name, s);

	// Find an empty slot in the nodes array and insert the node
	for (int i = 0; i < (g->max_nodes); i++)
	{
		// Check if the node already exist in the array to avoid duplicates
		if (array_1d_has_value(g->nodes, i))
		{
			struct node *current_node = array_1d_inspect_value(g->nodes, i);

			if (strncmp(s, current_node->name, 40) == 0)
			{
				// Node already exists, free memory and return
				free(n->name);
				dlist_kill(n->neighbours);
				free(n);
				break;
			}
		}

		if (!array_1d_has_value(g->nodes, i))
		{
			array_1d_set_value(g->nodes, n, i);
			break;
		}
	}
	return g;
}

/**
 * graph_find_node() - Find a node by its name in the graph.
 * @g: Graph to search in.
 * @s: Name of the node to find.
 *
 * Returns: A pointer to the found node, or NULL if not found.
 */
node *graph_find_node(const graph *g, const char *s)
{
	for (int i = 0; array_1d_has_value(g->nodes, i); i++)
	{
		struct node *current_node = array_1d_inspect_value(g->nodes, i);
		if (strncmp(s, current_node->name, 40) == 0)
		{
			return current_node;
		}
	}
	return NULL;
}

/**
 * graph_node_is_seen() - Check if a node has been seen.
 * @g: Graph to inspect.
 * @n: Node to check.
 *
 * Returns: true if the node has been seen, false otherwise.
 */
bool graph_node_is_seen(const graph *g, const node *n)
{
	return n->seen;
}

/**
 * graph_node_set_seen() - Set the 'seen' status of a node.
 * @g: Graph containing the node.
 * @n: Node to set the status for.
 * @seen: Value to set the 'seen' status to.
 *
 * Returns: A pointer to the modified graph.
 */
graph *graph_node_set_seen(graph *g, node *n, bool seen)
{
	n->seen = seen;
	return g;
}

/**
 * graph_reset_seen() - Reset the 'seen' status of all nodes in the graph.
 * @g: Graph to reset the status for.
 *
 * Returns: A pointer to the modified graph.
 */
graph *graph_reset_seen(graph *g)
{
	for (int i = 0; array_1d_has_value(g->nodes, i); i++)
	{
		struct node *current_node = array_1d_inspect_value(g->nodes, i);
		current_node->seen = false;
	}
	return g;
}

/**
 * graph_insert_edge() - Insert an edge between two nodes.
 * @g: Graph to insert the edge into.
 * @n1: First node.
 * @n2: Second node.
 *
 * Returns: A pointer to the modified graph.
 */
graph *graph_insert_edge(graph *g, node *n1, node *n2)
{
	dlist_insert(n1->neighbours, n2, dlist_first(n1->neighbours));
	return g;
}

/**
 * graph_neighbours() - Get the neighbors of a node in the graph.
 * @g: Graph to search in.
 * @n: Node to get neighbors for.
 *
 * Returns: A list of neighboring nodes.
 */
dlist *graph_neighbours(const graph *g, const node *n)
{
	dlist_pos pos = dlist_first(n->neighbours);
	dlist *current_neighbours = dlist_empty(NULL);

	while (!dlist_is_end(n->neighbours, pos))
	{
		dlist_insert(current_neighbours, dlist_inspect(n->neighbours, pos), dlist_first(current_neighbours));
		pos = dlist_next(n->neighbours, pos);
	}
	return current_neighbours;
}

/**
 * graph_kill() - Clean up memory used by the graph.
 * @g: Graph to destroy.
 *
 * Frees memory used by the graph and its nodes.
 */
void graph_kill(graph *g)
{
	for (int i = array_1d_low(g->nodes); array_1d_has_value(g->nodes, i); i++)
	{
		node *dead = array_1d_inspect_value(g->nodes, i);
		dlist_kill(dead->neighbours);
		free(dead->name);
		free(dead);
	}
	array_1d_kill(g->nodes);
	free(g);
}

/**
 * graph_print() - Iterate over the graph elements and print their values.
 * @g: Graph to inspect.
 *
 * Iterates over the graph and prints its contents.
 */
void graph_print(const graph *g)
{
	printf("[ ");
	for (int i = array_1d_low(g->nodes); i <= array_1d_high(g->nodes); i++)
	{
		struct node *var1 = array_1d_inspect_value(g->nodes, i);
		if (array_1d_has_value(g->nodes, i))
		{
			printf("[%s]", var1->name);
		}
		else
		{
			printf(" []");
		}
		if (i < array_1d_high(g->nodes))
		{
			printf(", ");
		}
	}
	printf(" ]\n");
}