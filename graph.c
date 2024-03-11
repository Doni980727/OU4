#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include <graph.h>
#include <array_2d.h>
#include <array_1d.h>
#include <dlist.h>

// Structure representing a graph
struct graph
{
	struct array_2d *matrix; // Adjacency matrix of nodes
	int max_nodes;			 // Maximum number of nodes allowed in the graph
	struct array_1d *nodes;	 // Matrix of nodes
};

// Structure representing a node in the graph
struct node
{
	bool seen;	// Flag to track whether the node has been seen
	char *name; // Name of the node
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
	g->matrix = array_2d_create(0, (max_nodes - 1), 0, (max_nodes - 1), NULL);
	g->max_nodes = max_nodes;
	g->nodes = array_1d_create(0, max_nodes - 1, NULL);

	// Iterate over the matrix to set the edges to 1 or NULL
	for (int i = 0; i < (g->max_nodes); i++)
	{

		for (int j = 0; j < (g->max_nodes); j++)
		{

			// If the indexes are equal, insert a pointer of the value 1
			if (i == j)
			{
				array_2d_set_value(g->matrix, (void *)1, i, j);
			}

			// Else insert a NULL pointer
			else
			{
				array_2d_set_value(g->matrix, NULL, i, j);
			}
		}
	}

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
				free(n);
				break;
			}
		}

		// If there is an empty slot, insert the node
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

	// Iterate over the nodes in the array, return if the node is found
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

	int index_i = 0;
	int index_j = 0;

	for (int i = 0; array_1d_has_value(g->nodes, i); i++)
	{

		struct node *current_node = array_1d_inspect_value(g->nodes, i);

		if (strncmp(n1->name, current_node->name, 40) == 0)
		{

			index_i = i;
		}

		if (strncmp(n2->name, current_node->name, 40) == 0)
		{

			index_j = i;
		}
	}

	array_2d_set_value(g->matrix, (void *)1, index_i, index_j);

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

	int index_i = 0;

	// Create an empty list
	dlist *l = dlist_empty(NULL);

	// Iterate over the array with the nodes
	for (int i = 0; array_1d_has_value(g->nodes, i); i++)
	{

		struct node *current_node = array_1d_inspect_value(g->nodes, i);

		// If the node is found, save the index
		if (strncmp(n->name, current_node->name, 40) == 0)
		{

			index_i = i;
			break;
		}
	}

	// Iterate over the columns in the matrx
	for (int j = 0; j < (g->max_nodes); j++)
	{

		// Check if the column has a value and it is not the same index as the row
		if (array_2d_has_value(g->matrix, index_i, j) && index_i != j)
		{

			// Inspect the node from the array and insert the node first in the list
			dlist_insert(l, array_1d_inspect_value(g->nodes, j), dlist_first(l));
		}
	}

	return l;
}

/**
 * graph_kill() - Clean up memory used by the graph.
 * @g: Graph to destroy.
 *
 * Frees memory used by the graph and its nodes.
 */
void graph_kill(graph *g)
{

	// Deallocate memory for the adjacency matrix
	array_2d_kill(g->matrix);

	// Deallocate memory for each node in the nodes array
	for (int i = 0; i < g->max_nodes; i++)
	{
		if (array_1d_has_value(g->nodes, i))
		{

			node *n = array_1d_inspect_value(g->nodes, i);
			free(n->name);
			free(n);
		}
	}

	// Deallocate memory for the array of nodes
	array_1d_kill(g->nodes);

	// Deallocate memory for the graph structure
	free(g);
}
