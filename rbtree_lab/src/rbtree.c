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
  // 트리의 루트(root)에서부터 시작하여 모든 노드를 재귀적으로 삭제
  recursion_delete_tree(t->root, t->nil);

  // 모든 노드가 삭제된 후, 센티널(nil) 노드의 메모리를 해제
  free(t->nil);

  // 마지막으로 트리 구조체 자체의 메모리를 해제
  free(t);
}

void recursion_delete_tree(node_t *t, node_t *nil) {
  // 현재 노드가 nil노드이면 더 이상 진행하지 않고 종료
  if (t == nil) {
    return;
  }

  // 왼쪽 자식 노드로 재귀 호출
  recursion_delete_tree(t->left, nil);

  // 오른쪽 자식 노드로 재귀 호출
  recursion_delete_tree(t->right, nil);

  // 왼쪽과 오른쪽 자식들이 모두 처리된 후, 현재 노드의 메모리를 해제
  free(t);
}

void transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil){ 
    t->root = v;
  }
  else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // z는 삽입할 새로운 노드이다.
  // key, color=RED, left =NIL, right=NIL 등으로 초기화 되어 있다고 가정.

  node_t *y = t->nil;  // y는 부모가 될 노드를 추적
  node_t *x = t->root; // x는 트리를 탐색하는 포인터이다.
  node_t *z = (node_t *)malloc(sizeof(node_t));

  z->key = key;

  while (x != t->nil){  // z가 삽입될 위치를 찾는다.
    y = x; // 부모가 될 y노드에 기본 트리의 root노드를 담아줌 (임시) 
    
    // binary tree의 삽입 방식과 유사
    if (z->key < x->key){ // 삽입하고 싶은 노드 z가 루트 노드인 x보다 작을 경우
      x = x->left;        // 루트 노드 x의 왼쪽으로
    } else {              // 삽입하고 싶은 노드 z가 루트 노드인 x보다 크거나 같을 경우 
      x = x->right;       // 루트 노드 x의 오른쪽으로
    }                     // 주어진 tree의 노드의 값을 루프(loop)를 통해 비교하여 자기 위치를 찾아감 
  }
  
  z->parent = y; // 삽입하고싶은 노드 z의 부모는 y가 될 것임

  if (y == t->nil){ // y의 값이 tree의 nil 노드라면
    t->root = z;    // 이 트리는 비어있는 트리이기 때문에 root 노드가 됨
  } 
  else if (z->key < y->key) { // 만약 삽입하고 싶은 노드 z보다 부모 y의 값 보다 작다면
    y->left = z;              // y의 왼쪽 자식이 된다.
  } else {                    // 삽입하고 싶은 노드 z보다 부모 y의 값 보다 크거나 같다면
    y->right = z;             // y의 오른쪽 자식이 된다.
  }

  // z의 자식과 색깔을 초기화.
  z->left = t->nil;           // 삽입된 z노드의 왼쪽 자식은 트리노드의 nil노드가 될 것이고
  z->right = t->nil;          // 오른쪽 자식도 nil 노드가 될 것
  z->color = RBTREE_RED; // RB 트리에서 삽입되는 새로운 노드의 색은 RED이다.

  // fix-up 함수를 호출하여 RB-Tree 속성을 유지하게 함. (속성을 위반했을 수도 있으니)
  rbtree_insert_fixup(t, z);

  // 새로 삽입된 노드의 포인터를 반환.
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;
  
  while (x != t->nil) {
    if (x->key == key){
      return x;
    } else if (x->key > key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  return t->nil;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *current = t->root;

  if (current == t->nil) {
    return t->nil;
  }

  while (current->left != t->nil) {
    current = current->left;
  }

  return current;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *current = t->root;

  if (current == t->nil) {
    return t->nil;
  }

  while (current->right != t->nil) {
    current = current->right;
  }

  return current;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;  // y는 시렞로 트리에서 제거될 노드 또는 그 위치를 대체할 노드
  node_t *x;      // x는 y의 원래 위치를 대체할 노드
  color_t y_original_color = y->color;  // y의 원래 색깔 저장

  // Case 1 : p의 왼쪽 자식이 없는 경우
  if (p->left == t->nil ) {
    x = p->right;
    transplant(t, p, p->right);
  }
  // Case 2 : p의 오른쪽 자식이 없는 경우
  else if (p->right == t->nil) {
    x = p->left;
    transplant(t, p, p->left);
  } 
  // Case 3 : p의 자식이 둘 다 있는 경우
  else {
    y = rbtree_min(p->right); // p의 오른쪽 서브트리에서 최솟값 노드 (successor)를 찾음
    y_original_color = y->color;
    x = y->right;

    if (y->parent == p) { // y가 p의 바로 오른쪽 자식인 경우
      x->parent = y;  // x의 부모를 y로 설정 (nil 노드일 경우에도 유효)
    } else {  // y가 p의 자손이지만 바로 오른쪽 자식은 아닌 경우
      transplant(t, y, y->right); // y의 오른쪽 자식을 y의 위치로 옮김
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t, p, y); // p의 위치를 y로 대체
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;  // y의 색깔을 p의 색깔로 변경
  }

  // y의 원래 색깔이 BLACK이었다면, RB트리 속성 복구 필요
  if (y_original_color == RBTREE_BLACK) {
    rbtree_delete_fixup(t, x);
  }

  free(p); // 삭제된 노드 p의 메모리 해제
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  size_t idx = 0; // 배열에 key를 저장할 현재 인덱스

  // 트리의 루트부터 중위 순회 시작
  inorder_traverse(t, t->root, arr, n, &idx);
  
  return 0;
}

// 중위 순회 보조 함수
// t : RED-BLACK TREE 구조체 (nil 노드 접근을 위해 필요)
// node : 현재 방문 중인 노드
// arr : key를 저장할 배열
// n : 배열의 최대 크기
// idx : 배열에 키를 저장할 현재 인덱스를 가리키는 포인터 (재귀 호출 간에 상태 유지를 위해 필요)
void inorder_traverse(const rbtree *t, node_t *node, key_t *arr, const size_t n, size_t *idx) {
  // 현재 노드가 nil 노드이거나, 배열이 이미 가득 찼다면 종료
  if (node == t->nil || *idx >= n) {
    return;
  }

  // 왼쪽 서브트리 순회
  inorder_traverse(t, node->left, arr, n, idx);

  // 현재 노드 처리 (배열에 키 저장)
  // 배열의 크기를 초과하지 않는지 다시 한번 확인
  if (*idx < n) {
    arr[*idx] = node->key;
    (*idx)++;
  } else {
    return; // 배열이 가득 찼으므로 더 이상 저장하지 않음
  }

  // 오른쪽 서브트리 순회
  inorder_traverse(t, node->right, arr, n, idx);
} 

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left; // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 회전한다

  if (y->left != t->nil) {  // y의 왼쪽 서브트리가 비어있지(nil) 않다면
    y->left->parent = x;    // x는 서브 트리 루트(root)의 부모가 된다
  }

  y->parent = x->parent;  // x의 부모가 y의 부모가 된다

  if (x->parent == t->nil) { // x가 루트(root)인 경우에는
    t->root = y;            // y가 루트(root)가 된다.
  }
  else if (x == x->parent->left) { // 그렇지 않다면, x가 왼쪽 자식인 경우
    x->parent->left = y;           // y는 왼쪽 자식이 된다
  } else {                         // 위의 두 조건에 다 해당되지 않는다면
    x->parent->right = y;          // x는 오른쪽 자식이었고, 이젠 y가 오른쪽 자식이 된다.
  }

  y->left = x;    // x를 y의 왼쪽 자식이 되게 한다
  x->parent = y;  // x의 부모는 y이다
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right; // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 회전한다

  if (y->right != t->nil) { // y의 오른쪽 서브트리가 비어있지(nil) 않다면
    y->right->parent = x;   // x는 서브 트리 루트(root)의 부모가 된다.
  }

  y->parent = x->parent;    // x의 부모가 y의 부모가 된다

  if (x->parent == t->nil) {  // x가 루트(root)인 경우에는
    t->root = y;              // y가 루트(root)가 된다
  }
  else if (x == x->parent->right) { // 그렇지 않다면, x가 오른쪽 자식인 경우
    x->parent->right = y;           // y는 오른쪽 자식이 된다
  } else {                          // 위 두 조건에 다 해당되지 않는다면
    x->parent->left = y;            // x는 왼쪽 자식이었고, 이젠 y가 왼쪽 자식이 된다.
  }

  y->right = x;   // x를 y의 오른쪽 자식이 되게 하고
  x->parent = y;  // x의 부모는 y이다
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

void rbtree_delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    // x가 왼쪽 자식인 경우
    if (x == x->parent->left) {
      node_t *w = x->parent->right; // w는 형제 노드

      // Case 1: 형제가 RED
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }

      // Case 2: 형제의 두 자식이 모두 BLACK
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // Case 3: 형제의 오른쪽 자식만 BLACK
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        
        // Case 4: 형제의 오른쪽 자식은 RED
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root; // 루프 종료
      }
    } 
    // x가 오른쪽 자식인 경우 (대칭)
    else {
      node_t *w = x->parent->left; // w는 형제 노드

      // Case 1: 형제가 RED
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }

      // Case 2: 형제의 두 자식이 모두 BLACK
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // Case 3: 형제의 왼쪽 자식만 BLACK
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }

        // Case 4: 형제의 왼쪽 자식은 RED
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root; // 루프 종료
      }
    }
  }
  x->color = RBTREE_BLACK;
}