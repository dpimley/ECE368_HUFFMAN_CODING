#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char * * argv){
  if (argc != 2){
    printf("ERROR: Incorrect number of inputs. Usage is huff <input file>.\n");
    return EXIT_FAILURE;
  }

  FILE * inf = fopen(argv[1], "r");

  if (inf == NULL){
    printf("ERROR: Input file pointer initialized to NULL.\n");
    return EXIT_FAILURE;
  }

  long * freq_count = calloc(ASCII_COUNT, sizeof(long));

  read_file(inf, freq_count);
  
  p_queue * heap_head = createHeap(freq_count);

  t_node * huff_tree = build_huff_tree(heap_head);

  print_pre_order(huff_tree);

  free(huff_tree);

  fclose(inf);
  return EXIT_SUCCESS;
}

void read_file(FILE * infile, long * char_count){
  int c;
  while (1){
    c = fgetc(infile);
    if (c == EOF)
      break;
    char_count[c]++;
  }
  return;
}

t_node * create_t_node(long count, int label){
  t_node * node = malloc(sizeof(t_node));
  node->count = count;
  node->label = label;
  return node;
}

void insert_heap(p_queue * heap_head, t_node * ins_node){
  if (heap_head->size == 0){
    heap_head->t_arr[0] = ins_node;
    heap_head->size++;
  }
  else{
    heap_head->size++;
    heap_head->t_arr[heap_head->size - 1] = ins_node;
    int child_idx = heap_head->size - 1;
    int parent_idx = (child_idx - 1) / 2;
    while (heap_head->t_arr[parent_idx]->count > ins_node->count && child_idx > 0){
      heap_head->t_arr[child_idx] = heap_head->t_arr[parent_idx];
      child_idx = parent_idx;
      parent_idx = (child_idx - 1) / 2;
    }
    heap_head->t_arr[child_idx] = ins_node;
  }
  return;
}

p_queue * createHeap(long * freq){
  p_queue * heap_head = malloc(sizeof(p_queue));
  heap_head->t_arr = malloc(ASCII_COUNT * sizeof(t_node *));
  heap_head->size = 0;
  int i;
  for (i = 0; i < ASCII_COUNT; i++){
    if (freq[i]){
      t_node * ins_node = create_t_node(freq[i], i);
      insert_heap(heap_head, ins_node);
    }
  }
  return heap_head;
}

t_node * remove_min(p_queue * heap_head){
  if (heap_head->size > 0){
    t_node * min_element = heap_head->t_arr[0];
    t_node * last_element = heap_head->t_arr[heap_head->size - 1];
    heap_head->t_arr[heap_head->size - 1] = NULL;
    heap_head->t_arr[0] = last_element;
    heap_head->size--;
    downward_heapify(heap_head->t_arr, heap_head->size, 0);
    return min_element;
  }
  return 0;
}

void downward_heapify(t_node * * t_arr, int size, int root){
  int left_idx = (2 * root) + 1;
  int right_idx = (2 * root) + 2;
  t_node * min_node = NULL;
  if (left_idx >= size && right_idx >= size){
    return;
  }
  if (left_idx < size && t_arr[root]->count <= t_arr[left_idx]->count){
    if (right_idx < size && t_arr[root]->count <= t_arr[right_idx]->count){
      return;
    }
  }
  if (right_idx < size && t_arr[right_idx]->count <= t_arr[left_idx]->count){
    min_node = t_arr[right_idx];
    t_arr[right_idx] = t_arr[root];
    t_arr[root] = min_node;
    downward_heapify(t_arr, size, right_idx);
  }
  else{
    min_node = t_arr[left_idx];
    t_arr[left_idx] = t_arr[root];
    t_arr[root] = min_node;
    downward_heapify(t_arr, size, left_idx);
  }
 return; 
}

t_node * build_huff_tree(p_queue * heap_head){
  while (heap_head->size != 1){
    t_node * tmp_left = remove_min(heap_head);
    t_node * tmp_right = remove_min(heap_head);
    t_node * top = create_t_node(tmp_left->count + tmp_right->count, 42);

    top->left = tmp_left;
    top->right = tmp_right;

    insert_heap(heap_head, top);
  }
  return heap_head->t_arr[0];
}

void print_header(t_node * head, FILE * outfile, int depth){
  

  if (head->left == NULL && head->right == NULL){
    fputc("1", outfile);
    fputc(head->label, outfile);
    return;
  }

  fputc("0", outfile);

  print_header(head->left, outfile, depth + 1);
  print_header(head->right, outfile, depth + 1);

  if (!depth)
    fputc("0", outfile);
  return;
}
