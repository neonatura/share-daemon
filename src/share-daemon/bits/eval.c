
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

int inittx_eval(tx_eval_t *eval, tx_event_t *event, tx_context_t *ctx, uint64_t user_id, shnum_t value)
{
  shkey_t *key;
  int err;

  shnum_set(value, &eval->eval_val);
  txeval_context_sign(eval, ctx);

  memcpy(&eval->eval_ctx, get_tx_key(ctx), sizeof(shkey_t));
  memcpy(&eval->eval_eve, get_tx_key(event), sizeof(shkey_t));

  key = shpam_ident_gen(user_id, &event->eve_peer);
  memcpy(&eval->eval_id, key, sizeof(eval->eval_id));
  shkey_free(&key);

  err = tx_init(NULL, (tx_t *)eval, TX_EVAL);
  if (err)
    return (err);

  return (0);
}

tx_eval_t *alloc_eval(tx_event_t *event, tx_context_t *ctx, uint64_t user_id, shnum_t value)
{
  tx_eval_t *eval;
  int err;

  eval = (tx_eval_t *)calloc(1, sizeof(tx_eval_t));
  if (!eval)
    return (NULL);

  err = inittx_eval(eval, event, ctx, user_id, value);
  if (err)
    return (NULL);

  return (eval);
}


/** Associated a particular context with releasing a eval. */
void txeval_context_sign(tx_eval_t *eval, tx_context_t *ctx)
{
  shkey_t *sig_key;

  if (!eval || !ctx)
    return;

  if (eval->eval_tx.tx_stamp == SHTIME_UNDEFINED)
    eval->eval_tx.tx_stamp = shtime();

  sig_key = shkey_cert(&ctx->ctx_sig,
      shkey_crc(&ctx->ctx_ref), eval->eval_tx.tx_stamp);
  memcpy(&eval->eval_sig, sig_key, sizeof(eval->eval_sig));
  shkey_free(&sig_key);

}

/** Determine whether the appropriate context is availale. */
int txeval_context_confirm(tx_eval_t *eval, tx_context_t *ctx)
{
  int err;

  err = shkey_verify(&eval->eval_sig, shkey_crc(&ctx->ctx_ref), 
    &ctx->ctx_sig, eval->eval_tx.tx_stamp);
  if (err)
    return (err);

  return (0);
}

int txop_eval_init(shpeer_t *cli_peer, tx_eval_t *eval)
{

  return (0);
}

int txop_eval_confirm(shpeer_t *peer, tx_eval_t *eval)
{
  tx_context_t *ctx;
  int err;

  ctx = (tx_context_t *)tx_load(TX_CONTEXT, &eval->eval_ctx);
  if (!ctx) {
    return (SHERR_INVAL); /* transaction chain is incomplete. */

}

  /* validate context of identity evaluation transaction. */
  err = txeval_context_confirm(eval, ctx);
  pstore_free(ctx);
  if (err)
    return (err);

  return (0);
}

int txop_eval_send(shpeer_t *peer, tx_eval_t *eval)
{
  return (0);
}

int txop_eval_recv(shpeer_t *peer, tx_eval_t *eval)
{
  return (0);
}

