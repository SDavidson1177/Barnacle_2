#include "Dictionary.h"
#include <assert.h>
#include <iostream>

using namespace std;

Dictionary::Dictionary(keyCompFunc KeyCompare, destroyKeyFunc keyDest, destroyValueFunc valueDest,
	printKeyFunc keyPrnt, printValueFunc valuePrnt) {
	this->keyComp = KeyCompare;
	this->destroyKey = keyDest;
	this->destroyValue = valueDest;
	this->printKey = keyPrnt;
	this->printValue = valuePrnt;
	this->root = nullptr;
}

static void destroyDictNode(struct DictNode* dn, destroyKeyFunc destroyKey, destroyValueFunc destroyValue) {
	if (dn) {
		destroyKey(dn->key);
		destroyValue(dn->value);
		struct DictNode* tempLeft = dn->left;
		struct DictNode* tempRight = dn->right;
		delete dn;
		destroyDictNode(tempLeft, destroyKey, destroyValue);
		destroyDictNode(tempRight, destroyKey, destroyValue);
	}
}

static struct DictNode* removeDictNode(void* key, Dictionary* d, struct DictNode* dn) {
	if (!dn) return nullptr;

	if (d->keyComp(key, dn->key) < 0) {
		dn->size -= 1;
		removeDictNode(key, d, dn->left);
	}
	else if (d->keyComp(key, dn->key) > 0) {
		dn->size -= 1;
		removeDictNode(key, d, dn->right);
	}
	else if (dn->left == nullptr) {
		struct DictNode *backup = dn->right;
		d->destroyValue(dn->value);
		d->destroyKey(dn->key);
		delete dn;
		return backup;
	}
	else if (dn->right == nullptr) {
		struct DictNode *backup = dn->left;
		d->destroyValue(dn->value);
		d->destroyKey(dn->key);
		delete dn;
		return backup;

		// neither child is nullptr:
	}
	else {
		// find the next largest key, and its parent
		struct DictNode *next = dn->right;
		struct DictNode *parent_of_next = nullptr;
		while (next->left) {
			parent_of_next = next;
			next = next->left;
		}
		// remove the old value
		d->destroyValue(dn->value);
		d->destroyKey(dn->key);
		// replace the key/value of this node
		dn->key = next->key;
		dn->value = next->value;
		(dn->size)--;
		// remove the next largest node
		if (parent_of_next) {
			parent_of_next->left = next->right;
		}
		else {
			// the next largest was our right child
			dn->right = next->right;
		}
		delete next;
	}
	return dn;
}

Dictionary::~Dictionary() {
	destroyDictNode(this->root, this->destroyKey, this->destroyValue);
}

void Dictionary::addNode(void* key, void* value) {
	if (!this->root) {
		this->root = new struct DictNode;
		this->root->key = key;
		this->root->value = value;
		this->root->size = 1;
		this->root->left = nullptr;
		this->root->right = nullptr;
		return;
	}

	struct DictNode* trail = nullptr;
	struct DictNode* temp = this->root;
	bool right = false;

	while (temp) {
		if (this->keyComp(key, temp->key) < 0) {
			temp->size++;
			trail = temp;
			right = false;
			temp = temp->left;
		}else if(this->keyComp(key, temp->key) > 0) {
			temp->size++;
			right = true;
			trail = temp;
			temp = temp->right;
		}
		else {
			this->destroyKey(temp->key);
			this->destroyValue(temp->value);
			temp->key = key;
			temp->value = value;
			return;
		}
	}

	temp = new struct DictNode;
	temp->size = 0;
	temp->key = key;
	temp->value = value;
	temp->left = nullptr;
	temp->right = nullptr;

	if (right) {
		trail->right = temp;
	}
	else {
		trail->left = temp;
	}
}

void Dictionary::removeNode(void* key) {
	assert(this->root);
	removeDictNode(key, this, this->root);
}

void** Dictionary::p_lookup(void* key) {
	struct DictNode* curr = this->root;

	while (curr) {
		if (this->keyComp(key, curr->key) < 0) {
			curr = curr->left;
		}
		else if (this->keyComp(key, curr->key) > 0) {
			curr = curr->right;
		}
		else {
			return &(curr->value);
		}
	}

	return nullptr;
}

void* Dictionary::lookup(void* key) {
	struct DictNode* curr = this->root;

	while (curr) {
		if (this->keyComp(key, curr->key) < 0) {
			curr = curr->left;
		}
		else if (this->keyComp(key, curr->key) > 0) {
			curr = curr->right;
		}
		else {
			return curr->value;
		}
	}

	return nullptr;
}

void* Dictionary::p_get(void* key) {
	struct DictNode* curr = this->root;

	while (curr) {
		if (this->keyComp(key, curr->key) < 0) {
			curr = curr->left;
		}
		else if (this->keyComp(key, curr->key) > 0) {
			curr = curr->right;
		}
		else {
			return &(curr->key);
		}
	}

	return nullptr;
}

void* Dictionary::get(void* key) {
	struct DictNode* curr = this->root;

	while (curr) {
		if (this->keyComp(key, curr->key) < 0) {
			curr = curr->left;
		}
		else if (this->keyComp(key, curr->key) > 0) {
			curr = curr->right;
		}
		else {
			return curr->key;
		}
	}

	return nullptr;
}

static void printDictNodes(Dictionary* d, int order, struct DictNode* dn) {
	if (dn) {
		if (order == PREORDER) {
			cout << "Key: ";
			d->printKey(dn->key);
			cout << " | Value: ";
			d->printValue(dn->value);
			cout << endl;
			printDictNodes(d, order, dn->left);
			printDictNodes(d, order, dn->right);
		}
		else if (order == INORDER) {
			printDictNodes(d, order, dn->left);
			cout << "Key: ";
			d->printKey(dn->key);
			cout << " | Value: ";
			d->printValue(dn->value);
			cout << endl;
			printDictNodes(d, order, dn->right);
		}
		else if (order == POSTORDER) {
			printDictNodes(d, order, dn->left);
			printDictNodes(d, order, dn->right);
			cout << "Key: ";
			d->printKey(dn->key);
			cout << " | Value: ";
			d->printValue(dn->value);
			cout << endl;
		}
	}
}

void Dictionary::print(int order) {
	assert(order == PREORDER || order == POSTORDER || order == INORDER);
	printDictNodes(this, order, this->root);
}

int Dictionary::size() {
	return this->root->size;
}