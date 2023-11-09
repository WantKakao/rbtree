#include "rbtree.h"

#include <stdlib.h>

node_t *new_node(int key)
{
  node_t *x = (node_t *)calloc(1, sizeof(node_t));
  x->color = RBTREE_RED;
  x->key = key;
  x->left = NULL;
  x->parent = NULL;
  x->right = NULL;

  return x;
}

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));
  NIL->color = RBTREE_BLACK;
  NIL->key = 0;
  NIL->left = NULL;
  NIL->parent = NULL;
  NIL->right = NULL;
  p->nil = NIL;
  p->root = NIL;

  return p;
}

void free_node(rbtree *t, node_t *k)
{
  if (k->left != t->nil)
    free_node(t, k->left);
  if (k->right != t->nil)
    free_node(t, k->right);
  free(k);
}

void delete_rbtree(rbtree *t)
{
  node_t *k = t->root;
  if (k != t->nil)
    free_node(t, k);

  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil) // 왜?
    y->left->parent = x;
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  x->parent = y;
  y->left = x;
}

void right_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left;
  x->left = y->right;
  if (y->left != t->nil) // 왜?
    y->right->parent = x;
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  x->parent = y;
  y->right = x;
}

void insert_fixup(rbtree *t, node_t *z)
{
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *z = new_node(key);
  node_t *y = t->nil;
  node_t *x = t->root;
  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }
  z->parent = y;
  if (y == t->nil)
    t->root = z;
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  insert_fixup(t, z);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *z = t->root;
  while (z != t->nil)
  {
    if (z->key == key)
      return z;
    else if (z->key < key)
      z = z->right;
    else
      z = z->left;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  if (t->root == t->nil)
    return NULL;
  node_t *z = t->root;
  while (z->left != t->nil)
    z = z->left;

  return z;
}

node_t *rbtree_max(const rbtree *t)
{
  if (t->root == t->nil)
    return NULL;
  node_t *z = t->root;
  while (z->right != t->nil)
    z = z->right;

  return z;
}

void transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

void delete_fixup(rbtree *t, node_t *x)
{
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    // 경우 1, 2, 3, 4
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right;
      // 경우 1
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // 경우 2
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 경우 3
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    // 경우 5, 6, 7, 8
    else
    {
      node_t *w = x->parent->left;
      // 경우 5
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // 경우 6
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 경우 7
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // 경우 8
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  node_t *y = p;
  node_t *x;
  color_t y_original_color = y->color;
  if (p->right == t->nil)
  {
    x = p->left;
    transplant(t, p, p->left);
  }
  else if (p->left == t->nil)
  {
    x = p->right;
    transplant(t, p, p->right);
  }
  else
  {
    // 전임자가 대체
    y = p->left;
    while (y->right != t->nil) // 왜?
    {
      y = y->right;
    }
    y_original_color = y->color;
    x = y->left;
    if (y->parent == p)
      x->parent = y;
    else
    {
      transplant(t, y, y->left);
      y->left = p->left;
      y->left->parent = y;
    }
    transplant(t, p, y);
    y->right = p->right;
    y->right->parent = y;
    y->color = p->color;
  }
  if (y_original_color == RBTREE_BLACK)
    delete_fixup(t, x);
  free(p);

  return 0;
}

void inorder_traversal(const rbtree *t, node_t *curr, key_t *arr, size_t *count)
{
  if (curr == t->nil)
    return;

  inorder_traversal(t, curr->left, arr, count);
  arr[(*count)++] = curr->key;
  inorder_traversal(t, curr->right, arr, count);
  return;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  if (t->root == t->nil)
    return 0;

  size_t count = 0;
  inorder_traversal(t, t->root, arr, &count);

  return 0;
}
