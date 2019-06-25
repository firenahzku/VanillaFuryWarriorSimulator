#include "items.h"

std::unordered_map<std::string, Item> items;

void Item::readData(char * buffer) {
	int numRead = 0;
	sscanf(buffer, "%d%n", &this->type, &numRead);
	buffer += numRead;
	sscanf(buffer, "%lf%n", &this->weaponSpeed, &numRead);
	buffer += numRead;
	for (int i = 0; i < SIZE_ITEMSTATTYPE; i++) {
		sscanf(buffer, "%d%n", &this->itemStats[i], &numRead);
		buffer += numRead;
	}
	sscanf(buffer, "%d%n", &this->setBonus, &numRead);
	buffer += numRead;
	sscanf(buffer, "%d%n", &this->proc, &numRead);
	buffer += numRead;
	sscanf(buffer, "%d%n", &this->use, &numRead);
	// debug_print();
}

void Item::debug_print() {
	fprintf(stderr, "%d %.2lf ", this->type, this->weaponSpeed);
	for (int i = 0; i < SIZE_ITEMSTATTYPE; i++)
		fprintf(stderr, "%d ", this->itemStats[i]);
	fprintf(stderr, "%d %d %d\n", this->setBonus, this->proc, this->use);
}

void readItemsFromDB(const char * filename) {
	FILE * fin = fopen(filename, "r");
	char line[10000], block;
	char name[1000];
	bool first = true;
	while (fgets(line, 10000, fin)) {
		if (strlen(line) < 10) break;
		if (first) {
			first = false;
			continue;
		}
		// fprintf(stderr, "%s", line);
		int numRead = 0;
		sscanf(line, "%[^\t]\t%n", name, &numRead);
		Item newItem;
		newItem.readData(line + numRead);
		items[std::string(name)] = newItem;
	}
	fclose(fin);
}

void readGear(const char * filename, Item ** gear) {
	FILE * fin = fopen(filename, "r");
	char line[1005];
	for (int i = 0; i < SIZE_INVENTORYSLOT; i++) {
		fgets(line, 1000, fin);
		int len = strlen(line);
		while (line[len - 1] == '\n' || line[len - 1] == '\r')
			len--;
		line[len] = 0;
		// fprintf(stderr, "%s\n", line);
		if (items.count(std::string(line))) {
			gear[i] = &items[std::string(line)];
		}
		else {
			fprintf(stderr, "<Error> Item %s not found in the item list.", line);
			exit(0);
		}
	}
	fclose(fin);
}