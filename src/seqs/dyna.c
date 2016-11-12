#include <assert.h>
#include "dyna.h"
#include "macros.h"

struct c4dyna *c4dyna_init(struct c4dyna *self, size_t it_size) {
  c4slab_init(&self->its, it_size);
  self->len = 0;
  return self;
}

void c4dyna_free(struct c4dyna *self) { c4slab_free(&self->its); }

void c4dyna_delete(struct c4dyna *self, size_t idx) {
  c4slab_delete(&self->its, idx, self->len);
}

void c4dyna_grow(struct c4dyna *self, size_t len) {
  c4slab_grow(&self->its, len);
}

void *c4dyna_idx(struct c4dyna *self, size_t idx) {
  return c4slab_idx(&self->its, idx, self->len);
}

void *c4dyna_insert(struct c4dyna *self, size_t idx) {
    if (self->len == self->its.len) { c4slab_grow(&self->its, self->len + 1); }
    self->len++;
    return c4slab_insert(&self->its, idx, self->len);
}

void *c4dyna_pop(struct c4dyna *self) {
  assert(self->len > 0);
  self->len--;
  return c4slab_idx(&self->its, self->len, self->len+1);
}

void *c4dyna_push(struct c4dyna *self) {
  return c4dyna_insert(self, self->len);
}

static void *seq_next(struct c4seq *_seq) {
  struct c4dyna_seq *seq = C4PTROF(c4dyna_seq, super, _seq);
  return (_seq->idx < seq->array->len)
    ? c4dyna_idx(seq->array, _seq->idx)
    : NULL;
}

struct c4seq *c4dyna_seq(struct c4dyna *self, struct c4dyna_seq *seq) {
  c4seq_init(&seq->super);
  seq->super.next = seq_next;
  seq->array = self;
  return &seq->super;
}

