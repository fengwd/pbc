#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pairing.h"
#include "a_param.h"
#include "d_param.h"
#include "e_param.h"
#include "f_param.h"
#include "bgn_param.h"
#include "parse.h"
#include "tracker.h"

static inline char *strclone(char *src)
{
    char *dst = malloc(strlen(src) + 1);
    if (dst) strcpy(dst, src);
    return dst;
}

int generic_is_almost_coddh(element_ptr a, element_ptr b,
	element_ptr c, element_ptr d, pairing_t pairing)
{
    int res = 0;
    element_t t0, t1;

    element_init(t0, pairing->GT);
    element_init(t1, pairing->GT);
    bilinear_map(t0, a, d, pairing);
    bilinear_map(t1, b, c, pairing);
    if (!element_cmp(t0, t1)) {
	res = 1;
    } else {
	element_mul(t0, t0, t1);
	if (element_is1(t0)) res = 1;
    }
    element_clear(t0);
    element_clear(t1);
    return res;
}

static void phi_warning(element_ptr out, element_ptr in, pairing_ptr pairing)
{
    (void) out;
    (void) in;
    (void) pairing;
    printf("Phi() not implemented for this pairing type yet!\n");
}

void pairing_init_inp_generic (pairing_t pairing, fetch_ops_t *fops, void *ctx)
{
    assert (fops);
    assert (ctx);
    char *s;
    token_t tok;

    token_init(tok);
    token_get_generic (tok, fops, ctx);
    if (tok->type != token_word) {
	return;
    }
    //word should be "type"
    token_get_generic (tok, fops, ctx);
    if (tok->type != token_word) {
	return;
    }
    //s = pairing->id = strclone(tok->s);
    s = strclone(tok->s);

    pairing->is_almost_coddh = generic_is_almost_coddh;
    pairing->phi = phi_warning;
    if (!strcmp(s, "a")) {
	a_param_t ap;

	a_param_init(ap);
	a_param_inp_generic (ap, fops, ctx);
	pairing_init_a_param(pairing, ap);
	a_param_clear(ap);
    } else if (!strcmp(s, "d")) {
	d_param_t cp;

	d_param_init(cp);
	d_param_inp_generic (cp, fops, ctx);
	pairing_init_c_param(pairing, cp);
	d_param_clear(cp);
    } else if (!strcmp(s, "e")) {
	e_param_t ep;

	e_param_init(ep);
	e_param_inp_generic (ep, fops, ctx);
	pairing_init_e_param(pairing, ep);
	e_param_clear(ep);
    } else if (!strcmp(s, "f")) {
	f_param_t fp;

	f_param_init(fp);
	f_param_inp_generic (fp, fops, ctx);
	pairing_init_f_param(pairing, fp);
	f_param_clear(fp);
    } else if (!strcmp(s, "bgn")) {
	bgn_param_t bgnp;

	bgn_param_init(bgnp);
	bgn_param_inp_generic (bgnp, fops, ctx);
	pairing_init_bgn_param(pairing, bgnp);
	bgn_param_clear(bgnp);
    }
    token_clear(tok);
}

void pairing_init_inp_buf (pairing_t pairing, const char *buf, size_t len)
{
    assert (buf);
    tracker_t t;
    tracker_init (&t, buf, len);
    pairing_init_inp_generic (pairing, &fops_buf, &t);
}

void pairing_init_inp_str(pairing_t pairing, FILE *stream)
{
    assert (stream);
    pairing_init_inp_generic (pairing, &fops_str, stream);
}

void pairing_clear(pairing_t pairing)
{
    //TODO
}
