
/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
 */

#include "sharedaemon.h"

/* Context life-span is two years by default. */
#define DEFAULT_CONTEXT_LIFESPAN 63072000


int inittx_context(tx_context_t *tx, shkey_t *name_key)
{
  shkey_t *data_key;
  shctx_t ctx;
  int err;

  err = shctx_get_key(name_key, &ctx);
  if (err)
    return (err);

  memcpy(&tx->ctx_name, name_key, sizeof(tx->ctx_name));
  tx->ctx_expire = ctx.ctx_expire;

  data_key = shkey_bin(ctx.ctx_data, ctx.ctx_data_len);
  memcpy(&tx->ctx_key, data_key, sizeof(tx->ctx_key));
  shkey_free(&data_key);

  memcpy(tx->ctx_data, ctx.ctx_data, MIN(ctx.ctx_data_len, sizeof(ctx.ctx_data)));
  tx->ctx_data_len = ctx.ctx_data_len;

  shctx_free(&ctx);

  shgeo_local(&tx->ctx_geo, SHGEO_PREC_DISTRICT);

  err = tx_init(NULL, (tx_t *)tx, TX_CONTEXT);
  if (err)
    return (err);

  return (0);
}

tx_context_t *alloc_context(shkey_t *name_key)
{
  tx_context_t *tx;
  int err;

  tx = (tx_context_t *)calloc(1, sizeof(tx_context_t));
  if (!tx)
    return (NULL);

  err = inittx_context(tx, name_key);
  if (err) {
    free(tx);
    return (NULL);
  }

  return (tx);
}

int inittx_context_data(tx_context_t *tx, char *name, unsigned char *data, size_t data_len)
{
  int err;

  if (!name)
    return (SHERR_INVAL);
  if (data_len > SHCTX_MAX_VALUE_SIZE)
    return (SHERR_INVAL);

  err = shctx_set(name, data, data_len);
  if (err)
    return (err);

  err = inittx_context(tx, shctx_key(name));
  if (err)
    return (err);

  return (0);
}

tx_context_t *alloc_context_data(char *name, void *data, size_t data_len)
{
  tx_context_t *tx;
  int err;

  tx = (tx_context_t *)calloc(1, sizeof(tx_context_t));
  if (!tx)
    return (NULL);

  err = inittx_context_data(tx, name, data, data_len);
  if (err) {
    free(tx);
    return (NULL);
  }

  return (tx);
}


#if 0
int inittx_context_ref(tx_context_t *tx, tx_t *ref_tx, shkey_t *ctx_key)
{
  shkey_t *ref_key;
  int err;

  if (!tx)
    return (SHERR_INVAL);

  ref_key = get_tx_key(ref_tx);
  if (!ref_key) {
    return (SHERR_INVAL);
  }

  return (inittx_context(tx, ref_key, ctx_key, ref_tx->tx_op, NULL));
}

tx_context_t *alloc_context_ref(tx_t *ref_tx, shkey_t *ctx_key)
{
  tx_context_t *tx;
  int err;

  tx = (tx_context_t *)calloc(1, sizeof(tx_context_t));
  if (!tx)
    return (NULL);
  
  err = inittx_context_ref(tx, ref_tx, ctx_key);
  if (err)
    return (NULL);

  return (tx);
}

tx_context_t *alloc_context_data(tx_t *ref_tx, void *data, size_t data_len)
{
  tx_context_t *ctx;
  shkey_t *key;

  key = shkey_bin(data, data_len);
  ctx = alloc_context_ref(ref_tx, key);
  shkey_free(&key);

  return (ctx);
}
#endif



int txop_context_init(shpeer_t *cli_peer, tx_context_t *ctx)
{


  tx_sign((tx_t *)ctx, &ctx->ctx_sig, &ctx->ctx_key);

  return (0);
}

int txop_context_confirm(shpeer_t *cli_peer, tx_context_t *ctx)
{
  int err;

  if (shtime_after(shtime(), ctx->ctx_expire))
    return (SHERR_KEYEXPIRED);

  err = tx_sign_confirm((tx_t *)ctx, &ctx->ctx_sig, &ctx->ctx_key);
  if (err)
    return (err);

  return (0);
}

int txop_context_recv(shpeer_t *cli_peer, tx_context_t *ctx)
{

  if (!ctx)
    return (SHERR_INVAL);


  return (0);
}


int txop_context_send(shpeer_t *cli_peer, tx_context_t *ctx)
{

  if (cli_peer)
    return (SHERR_OPNOTSUPP); /* must be broadcasted. */

  return (0);
}

int txop_context_wrap(shpeer_t *cli_peer, tx_context_t *ctx)
{
  wrap_bytes(&ctx->ctx_data_len, sizeof(ctx->ctx_data_len));
  return (0);
}
