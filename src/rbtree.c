#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));

  nil_node->color = RBTREE_BLACK;
  p->root = nil_node;
  p->nil = nil_node;

  return p;
}

void delete_node(node_t *cur_node, node_t *nil) {
  if (cur_node == nil) {
    return;
  }

  delete_node(cur_node->left, nil);
  delete_node(cur_node->right, nil);
  free(cur_node);
}

void delete_rbtree(rbtree *t) {
  if (t->root != t->nil) {
    delete_node(t->root, t->nil);
  }

  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *cur_node) {
  node_t *child_node = cur_node->right; 
  cur_node->right = child_node->left;

  if (child_node->left != t->nil) {
    child_node->left->parent = cur_node;
  }

  child_node->parent = cur_node->parent;  
  
  if (cur_node->parent == t->nil) {
    t->root = child_node;
  } else if (cur_node == cur_node->parent->left) {
    cur_node->parent->left = child_node;
  } else {
    cur_node->parent->right = child_node;
  }

  child_node->left = cur_node;
  cur_node->parent = child_node;
}

void right_rotate(rbtree *t, node_t *cur_node) {
  node_t *child_node = cur_node->left;
  cur_node->left = child_node->right;

  if (child_node->right != t->nil) {
    child_node->right->parent = cur_node;
  }

  child_node->parent = cur_node->parent;

  if (cur_node->parent == t->nil) {
    t->root = child_node;
  } else if (cur_node == cur_node->parent->right) {
    cur_node->parent->right = child_node;
  } else {
    cur_node->parent->left = child_node;
  }

  child_node->right = cur_node;
  cur_node->parent = child_node;
}

void rbtree_insert_fixup(rbtree *t, node_t *cur_node) {
  while (cur_node->parent->color != RBTREE_BLACK) {
    if (cur_node->parent == cur_node->parent->parent->left) {
      node_t *uncle_node = cur_node->parent->parent->right;

      if (uncle_node->color == RBTREE_RED) {
        cur_node->parent->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        cur_node->parent->parent->color = RBTREE_RED;
        cur_node = cur_node->parent->parent;
      } else {
        if (cur_node == cur_node->parent->right) {
          cur_node = cur_node->parent;
          left_rotate(t, cur_node);
        }

        cur_node->parent->color = RBTREE_BLACK;
        cur_node->parent->parent->color = RBTREE_RED;
        right_rotate(t, cur_node->parent->parent); 
      }
    } else {
      node_t *uncle_node = cur_node->parent->parent->left;

      if (uncle_node->color == RBTREE_RED) {
        cur_node->parent->color = RBTREE_BLACK;
        cur_node->parent->parent->color = RBTREE_RED;
        uncle_node->color = RBTREE_BLACK;
        cur_node = cur_node->parent->parent;
      } else {
        if (cur_node == cur_node->parent->left)  {
          cur_node = cur_node->parent;
          right_rotate(t, cur_node);
        }

        cur_node->parent->color = RBTREE_BLACK;
        cur_node->parent->parent->color = RBTREE_RED;
        left_rotate(t, cur_node->parent->parent);
      }
    }
  }
  
  if (t->root->color == RBTREE_RED) {
    t->root->color = RBTREE_BLACK;
  }
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->parent = t->nil;
  new_node->left = t->nil;
  new_node->right = t->nil;

  node_t *cur_node = t->root;
  node_t *parent_node = t->nil;

  while (cur_node != t->nil) {
    parent_node = cur_node;
    if (key < cur_node->key) {
      cur_node = cur_node->left;
    } else {
      cur_node = cur_node->right;
    }
  }

  new_node->parent = parent_node;

  if (parent_node == t->nil) {
    t->root = new_node;
  } else if (key < parent_node->key) {
    parent_node->left = new_node;
  } else {
    parent_node->right = new_node;
  }

  rbtree_insert_fixup(t, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur_node = t->root;

  while (cur_node != t->nil) {
    if (key < cur_node->key) {
      cur_node = cur_node->left;
    } else if (key > cur_node->key) {
      cur_node = cur_node->right;
    } else {
      return cur_node;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *cur_node = t->root;
  node_t *parent_node = t->nil;

  while (cur_node != t->nil) {
    parent_node = cur_node;
    cur_node = cur_node->left;
  }

  return parent_node;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *cur_node = t->root;
  node_t *parent_node = t->nil;
 
  while (cur_node != t->nil) {
    parent_node = cur_node;
    cur_node = cur_node->right;
  }

  return parent_node;
}

node_t *min_node(const rbtree *t, node_t *node) {
  node_t *cur_node = node;
  node_t *parent_node = t->nil;

  while (cur_node != t->nil) {
    parent_node = cur_node;
    cur_node = cur_node->left;
  }

  return parent_node;
}


void rbtree_transplant(rbtree *t, node_t *target_node, node_t *next_node) {
  if (target_node->parent == t->nil) {
    t->root = next_node;
  } else if (target_node == target_node->parent->left) {
    target_node->parent->left = next_node;
  } else {
    target_node->parent->right = next_node;
  }

  next_node->parent = target_node->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *cur_node) {
  node_t* bro_node;

  while (cur_node != t->root && cur_node->color == RBTREE_BLACK) {
    if (cur_node == cur_node->parent->left) {
      bro_node = cur_node->parent->right;

      if (bro_node->color == RBTREE_RED) {
        bro_node->color = RBTREE_BLACK;   
        cur_node->parent->color = RBTREE_RED;     
        left_rotate(t, cur_node->parent);
        bro_node = cur_node->parent->right;
      }

      if (bro_node->left->color == RBTREE_BLACK && bro_node->right->color == RBTREE_BLACK) {
        bro_node->color = RBTREE_RED;
        cur_node = cur_node->parent;
      } else {
        if (bro_node->right->color == RBTREE_BLACK) {
          bro_node->left->color = RBTREE_BLACK;
          bro_node->color = RBTREE_RED;
          right_rotate(t, bro_node);
          bro_node = cur_node->parent->right;
        }

        bro_node->color = cur_node->parent->color;   
        cur_node->parent->color = RBTREE_BLACK;
        bro_node->right->color = RBTREE_BLACK;
        left_rotate(t, cur_node->parent);
        cur_node = t->root;  
      }
    } else {
      bro_node = cur_node->parent->left;
      
      if (bro_node->color == RBTREE_RED) {
        bro_node->color = RBTREE_BLACK;
        cur_node->parent->color = RBTREE_RED;
        right_rotate(t, cur_node->parent);
        bro_node = cur_node->parent->left;
      }

      if (bro_node->right->color == RBTREE_BLACK && bro_node->left->color == RBTREE_BLACK) {
        bro_node->color = RBTREE_RED;
        cur_node = cur_node->parent;
      } else {
        if (bro_node->left->color == RBTREE_BLACK) {
          bro_node->right->color = RBTREE_BLACK;
          bro_node->color = RBTREE_RED;
          left_rotate(t, bro_node);
          bro_node = cur_node->parent->left;
        }

        bro_node->color = cur_node->parent->color;
        cur_node->parent->color = RBTREE_BLACK;
        bro_node->left->color = RBTREE_BLACK;
        right_rotate(t, cur_node->parent);
        cur_node = t->root;
      }
    }
  }

  cur_node->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *target_node = p;
  node_t *child_node = t->nil; 
  color_t delete_color = target_node->color;

  if (p->left == t->nil) {
    child_node = p->right;
    rbtree_transplant(t, p, p->right);
  } else if (p->right == t->nil) {
    child_node = p->left;
    rbtree_transplant(t, p, p->left);
  } else {
    target_node = min_node(t, p->right);
    delete_color = target_node->color;

    if (target_node != p->right) {
      child_node = target_node->right;
      rbtree_transplant(t, target_node, target_node->right);
      target_node->right = p->right;
      target_node->right->parent = target_node;
    } else {
      child_node->parent = target_node;
    }

    rbtree_transplant(t, p, target_node);
    target_node->left = p->left;
    target_node->left->parent = target_node;
    target_node->color = p->color;
  }

  if (delete_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, child_node);
  }

  free(p);

  return 0;
}

int add_to_array(const rbtree *t, key_t *arr, const size_t n, node_t *cur_node, int i) {
  if (i == n || cur_node == t->nil) { 
    return i;
  }

  if (cur_node->left != t->nil) {
    i = add_to_array(t, arr, n, cur_node->left, i);
  }

  arr[i++] = cur_node->key;

  if (cur_node->right != t->nil) {
    i = add_to_array(t, arr, n, cur_node->right, i);
  }

  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  add_to_array(t, arr, n, t->root, 0);
  
  return 0;
}