/*
 * Singly linked list merge-sort
 *
 * boxes:
 */

#include <stdlib.h>

int __nondet();

struct DItem {
	struct DItem* next;
	int value;
};

struct TLItem {
	struct TLItem* next;
	struct DItem* data;
};

int main() {

	struct TLItem* data = NULL;
	struct DItem* item, * item2;
	struct TLItem *lItem;

	// fill top level list with single data items
	while (__nondet()) {

		item = malloc(sizeof *item);
		item->next = NULL;
		item->value = __nondet();

		lItem = malloc(sizeof *lItem);
		if (data) {
			lItem->next = data->next;
			data->next = lItem;
		} else {
			lItem->next = lItem;
			data = lItem;
		}
		lItem->data = item;

		item = NULL;
		lItem = NULL;

	}

	if (!data)
		return 0;

	// merge subsequent pairs
	while (data->next != data) {

		item = data->data;
		item2 = data->next->data;

		lItem = data->next;
		data->next = lItem->next;
		free(lItem);

		struct DItem** dst = &data->data;

		while (item && item2) {

			if (item->value < item2->value) {
				*dst = item;
				item = item->next;
			} else {
				*dst = item2;
				item2 = item2->next;
			}

			dst = &(*dst)->next;

		}

		if (item) {
			*dst = item;
			item = NULL;
		} else if (item2) {
			*dst = item2;
			item2 = NULL;
		}

		dst = NULL;
		data = data->next;

	}

	// release the list
	item = data->data;
	free(data);

	while (item) {
		item2 = item;
		item = item->next;
		free(item2);
	}

	return 0;

}