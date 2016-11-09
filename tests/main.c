#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "c4.h"
#include "col.h"
#include "ctx.h"
#include "err.h"
#include "ls.h"
#include "map.h"
#include "rec.h"
#include "tbl.h"

static int int_cmp(void *_x, void *_y) {
  int *x = _x, *y = _y;
  return *x < *y;
}

static void col_tests() {
  struct c4col_t ct;
  c4col_t_init(&ct, "foo");

  struct c4col c;
  c4col_init(&c, "bar", &ct);
  
  c4col_free(&c);
  c4col_t_free(&ct);
}

static struct c4err_t custom_type;

static void err_tests() {
  c4err_t_init(&custom_type, NULL, "custom"); // NULL super type
  
  C4TRY("outer") {
    struct c4err *err = NULL;
    C4TRY("inner") { err = C4THROW(&custom_type, "test throw"); }
    bool caught = false;
    
    C4CATCH(e, &custom_type) {
      assert(e == err);
      c4err_free(e); // handle err by freeing
      caught = true;
    }

    assert(caught);

    // make sure queue is empty, NULL matches any type
    C4CATCH(e, NULL) { assert(false); }    
  }
}

static void ls_splice_tests() {
  struct c4ls foo;
  struct c4ls bar;
  struct c4ls its[10];

  c4ls_init(&foo);
  c4ls_init(&bar);
  
  for (int i = 0; i < 5; i++) {
    c4ls_prepend(&foo, its+i);
    c4ls_prepend(&bar, its+i+5);    
  }

  c4ls_splice(&foo, bar.next, bar.prev);
  struct c4ls *it = &foo;
  
  for (int i = 0; i < 10; i++) {
    it = it->next;
    assert(it == its+i);
  }

  assert(it->next == &foo);
}

static void ls_tests() {
  ls_splice_tests();
}

static void map_tests() {
  struct c4map m;
  c4map_init(&m, int_cmp);
  c4map_free(&m);
}

static void rec_tests() {
  struct c4rec r;
  c4rec_init(&r); 
}

static void tbl_tests() {
  struct c4tbl t;
  c4tbl_init(&t); 
}

static struct c4ctx *ctx() {
  static struct c4ctx ctx;
  static bool init = true;

  if (init) {
    c4ctx_init(&ctx);
    init = false;
  }

  return &ctx;
}

int main() {
  c4init(ctx);

  C4TRY("run all tests") {
    col_tests();
    err_tests();
    ls_tests();
    map_tests();
    rec_tests();
    tbl_tests();

    C4THROW(&c4err, "test print");
  }

  c4ctx_free(ctx());
  c4free();
  return 0;
}
