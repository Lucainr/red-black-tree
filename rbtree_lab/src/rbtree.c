#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  node_t *nil = (node_t *)malloc(sizeof *nil);
  nil->color = RBTREE_BLACK;
  nil->left = nil->right = nil->parent = nil;
  p->nil = nil;
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *y = t->nil;
  node_t *x = t->root;
  node_t *z = (node_t *)malloc(sizeof(node_t));

  z->key = key;

  while (x != t->nil){  // z가 삽입될 위치를 찾는다.
    y = x;
    
    if (z->key < x->key){
      x = x->left;
    } else {
      x = x->right;
    }
  }
  
  z->parent = y;

  if (y == t->nil){
    t->root = z;
  } 
  else if (z->key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }

  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED; // RB 트리에서 삽입되는 새로운 노드의 색은 RED이다.

  // 삽입으로 인해 깨진 RB트리의 특성을 맞추기 위한 함수
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;

  if (y->left != t->nil) {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if (x->parent = t->nil) {
    t->root = y;
  }
  else if (x == x->parent->left) { 
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;

  if (y->right != t->nil) {
    y->right->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  }
  else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {
    // Case A : z의 부모가 조부모의 왼쪽 노드 일 때.
    if(z->parent == z->parent->parent->left) {
      node_t *uncle = z->parent->parent->right; // 삼촌 노드(부모의 형제)
      
      // CASE A.1 : 삼촌이 RED일 경우 색깔 변경
      if(uncle->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      
      // Case A.2 & A.3 : 삼촌이 BLACK인 경우 -> 색상 변경
      } else {
        // Case A.2 : z가 '오른쪽' 자식이라 꺽인 모양(triangle)일 때
        if (z == z->parent->right) {
          z = z->parent;
          left_rotate(t, z); // 왼쪽 회전으로 직선 모양(line)으로 만듦 (Case A.3 으로 변환)
        }
        // Case A.3 : z가 '왼쪽' 자식이라 직선 모양(line)일 때
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    // Case B : z의 부모가 조부모의 '오른쪽' 자식인 경우 (A와 대칭)
    } else {
      node_t *uncle = z->parent->parent->left; // 삼촌 노드
      
      // Case B.1 : 삼촌이 RED인 경우 -> 색상 변경
      if (uncle->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      // Case B.2 & B.3 : 삼촌이 BLACK인 경우 -> 회전 필요
      } else {
        // Case B.2 : z가 '왼쪽' 자식이라 꺾인 모양(triangle)일 때
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z); // 오른쪽 회전으로 직선 모양(line)으로 만듦 (Case B.3으로 변환)
        }
        // Case B.3 : z가 '오른쪽' 자식이라 직선 모양(line)일 때
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  // 루프(loop)가 끝나면, 트리의 루트는 항상 BLACK이어야 한다는 규칙을 적용.
  t->root->color = RBTREE_BLACK;
}