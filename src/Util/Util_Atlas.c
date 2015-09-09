/*
 * Util_Atlas.c
 *
 *  Created on: May 23, 2015
 *      Author: begah
 */

#include "Util.h"

static int PackingAltas_Maxint(int a, int b)
{
	return a > b ? a : b;
}

// Atlas based on Skyline Bin Packer by Jukka Jylänki

void PackAtlas_Free(struct PackAtlas* atlas)
{
	if (atlas == NULL)
		return;
	if (atlas->nodes != NULL)
		free(atlas->nodes);
	free(atlas);
}

struct PackAtlas* PackAtlas_Init(int w, int h, int nnodes)
{
	struct PackAtlas* atlas = NULL;

	// Allocate memory for the font stash.
	atlas = (struct PackAtlas*) malloc(sizeof(struct PackAtlas));
	check_mem(atlas);

	atlas->width = w;
	atlas->height = h;

	// Allocate space for skyline nodes
	atlas->nodes = (struct PackAtlas_Node*) malloc(
			sizeof(struct PackAtlas_Node) * nnodes);
	if (atlas->nodes == NULL)
		goto error;
	atlas->nnodes = 0;
	atlas->cnodes = nnodes;

	// Init root node.
	atlas->nodes[0].x = 0;
	atlas->nodes[0].y = 0;
	atlas->nodes[0].width = (short) w;
	atlas->nnodes++;

	return atlas;

	error: if (atlas)
		PackAtlas_Free(atlas);
	return NULL;
}

static int PackAtlas_InsertNode(struct PackAtlas* atlas, int idx, int x, int y, int w)
{
	int i;
	// Insert node
	if (atlas->nnodes + 1 > atlas->cnodes) {
		atlas->cnodes = atlas->cnodes == 0 ? 8 : atlas->cnodes * 2;
		atlas->nodes = (struct PackAtlas_Node*) realloc(atlas->nodes,
				sizeof(struct PackAtlas_Node) * atlas->cnodes);
		if (atlas->nodes == NULL)
			return 0;
	}
	for (i = atlas->nnodes; i > idx; i--)
		atlas->nodes[i] = atlas->nodes[i - 1];
	atlas->nodes[idx].x = (short) x;
	atlas->nodes[idx].y = (short) y;
	atlas->nodes[idx].width = (short) w;
	atlas->nnodes++;

	return 1;
}

static void PackAtlas_RemoveNode(struct PackAtlas* atlas, int idx)
{
	int i;
	if (atlas->nnodes == 0)
		return;
	for (i = idx; i < atlas->nnodes - 1; i++)
		atlas->nodes[i] = atlas->nodes[i + 1];
	atlas->nnodes--;
}

void PackAtlas_Expand(struct PackAtlas* atlas, int w, int h)
{
	// Insert node for empty space
	if (w > atlas->width)
		PackAtlas_InsertNode(atlas, atlas->nnodes, atlas->width, 0,
				w - atlas->width);
	atlas->width = w;
	atlas->height = h;
}

void PackAtlas_Reset(struct PackAtlas* atlas, int w, int h)
{
	atlas->width = w;
	atlas->height = h;
	atlas->nnodes = 0;

	// Init root node.
	atlas->nodes[0].x = 0;
	atlas->nodes[0].y = 0;
	atlas->nodes[0].width = (short) w;
	atlas->nnodes++;
}

static int PackAtlas_AddSkylineLevel(struct PackAtlas* atlas, int idx, int x, int y, int w, int h)
{
	int i;

	// Insert new node
	if (PackAtlas_InsertNode(atlas, idx, x, y + h, w) == 0)
		return 0;

	// Delete skyline segments that fall under the shaodw of the new segment.
	for (i = idx + 1; i < atlas->nnodes; i++) {
		if (atlas->nodes[i].x
				< atlas->nodes[i - 1].x + atlas->nodes[i - 1].width) {
			int shrink = atlas->nodes[i - 1].x + atlas->nodes[i - 1].width
					- atlas->nodes[i].x;
			atlas->nodes[i].x += (short) shrink;
			atlas->nodes[i].width -= (short) shrink;
			if (atlas->nodes[i].width <= 0) {
				PackAtlas_RemoveNode(atlas, i);
				i--;
			} else {
				break;
			}
		} else {
			break;
		}
	}

	// Merge same height skyline segments that are next to each other.
	for (i = 0; i < atlas->nnodes - 1; i++) {
		if (atlas->nodes[i].y == atlas->nodes[i + 1].y) {
			atlas->nodes[i].width += atlas->nodes[i + 1].width;
			PackAtlas_RemoveNode(atlas, i + 1);
			i--;
		}
	}

	return 1;
}

static int PackAtlas_RectFits(struct PackAtlas* atlas, int i, int w, int h)
{
	// Checks if there is enough space at the location of skyline span 'i',
	// and return the max height of all skyline spans under that at that location,
	// (think tetris block being dropped at that position). Or -1 if no space found.
	int x = atlas->nodes[i].x;
	int y = atlas->nodes[i].y;
	int spaceLeft;
	if (x + w > atlas->width)
		return -1;
	spaceLeft = w;
	while (spaceLeft > 0) {
		if (i == atlas->nnodes)
			return -1;
		y = PackingAltas_Maxint(y, atlas->nodes[i].y);
		if (y + h > atlas->height)
			return -1;
		spaceLeft -= atlas->nodes[i].width;
		++i;
	}
	return y;
}

int PackAtlas_Add(struct PackAtlas* atlas, int rw, int rh, int* rx, int* ry)
{
	int besth = atlas->height, bestw = atlas->width, besti = -1;
	int bestx = -1, besty = -1, i;

	// Bottom left fit heuristic.
	for (i = 0; i < atlas->nnodes; i++) {
		int y = PackAtlas_RectFits(atlas, i, rw, rh);
		if (y != -1) {
			if (y + rh < besth
					|| (y + rh == besth && atlas->nodes[i].width < bestw)) {
				besti = i;
				bestw = atlas->nodes[i].width;
				besth = y + rh;
				bestx = atlas->nodes[i].x;
				besty = y;
			}
		}
	}

	if (besti == -1)
		return 0;

	// Perform the actual packing.
	if (PackAtlas_AddSkylineLevel(atlas, besti, bestx, besty, rw, rh) == 0)
		return 0;

	*rx = bestx;
	*ry = besty;

	return 1;
}
