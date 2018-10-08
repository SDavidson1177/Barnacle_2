#pragma once

typedef int (*keyCompFunc) (void *, void *);
typedef bool(*destroyKeyFunc) (void *);
typedef bool(*destroyValueFunc) (void *);
typedef bool(*printKeyFunc) (void *);
typedef bool(*printValueFunc) (void *);

const int PREORDER = -1;
const int INORDER = 0;
const int POSTORDER = 1;

struct DictNode {
	void *key;
	void *value;
	int size;
	struct DictNode* left;
	struct DictNode* right;
};

class Dictionary {
private:
	struct DictNode* root;
public:
	keyCompFunc keyComp;
	destroyKeyFunc destroyKey;
	destroyValueFunc destroyValue;
	printKeyFunc printKey;
	printValueFunc printValue;

	Dictionary(keyCompFunc KeyCompare, destroyKeyFunc keyDest, destroyValueFunc valueDest,
		printKeyFunc keyPrnt, printValueFunc valuePrnt);
	
	~Dictionary();

	void addNode(void* key, void* value);

	void removeNode(void* key);

	void* lookup(void* key);

	void* get(void* key);

	void* p_get(void* key);

	void** p_lookup(void* key);

	void print(int order);

	int size();
};