
/*
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
 */  

#include "sharedaemon.h"



int api_geo_set(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  return (SHERR_OPNOTSUPP);
}

int api_geo_place(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  shjson_t *result;
  shloc_t loc;
  shgeo_t geo;
  shnum_t lat;
  shnum_t lon;
  char *text;
  int alt;
  int err;

  text = shjson_array_str(param, NULL, 0);
  if (!text)
    return (SHERR_INVAL);

  memset(&geo, 0, sizeof(geo));
  err = shgeodb_place(text, &geo);
  if (err)
    return (err);

  shgeo_loc(&geo, &lat, &lon, &alt);
  result = shjson_obj_add(reply, "result");
  shjson_num_add(result, "latitude", (double)lat);
  shjson_num_add(result, "longitude", (double)lon);
  if (alt)
    shjson_num_add(result, "altitude", alt);

  memset(&loc, 0, sizeof(loc));
  err = shgeodb_loc(&geo, &loc);
  if (!err) {
    shjson_str_add(result, "name", loc.loc_name);
    shjson_str_add(result, "summary", loc.loc_summary);
    shjson_str_add(result, "locale", loc.loc_locale);
    shjson_str_add(result, "zone", loc.loc_zone);
    shjson_str_add(result, "type", loc.loc_type);
    shjson_num_add(result, "precision", loc.loc_prec);
  }

  return (0);
}

int api_geo_scan(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  shjson_t *result;
  shnum_t lat, lon;
  shgeo_t geo;
  shloc_t loc;
  int alt;
  int err;
  
  lat = shjson_array_num(param, NULL, 0);
  lon = shjson_array_num(param, NULL, 1);

  memset(&geo, 0, sizeof(geo));
  err = shgeodb_scan(lat, lon, 0.5, &geo);

  result = shjson_obj_add(reply, "result");
  if (!err) {
    shgeo_loc(&geo, &lat, &lon, &alt);
    shjson_num_add(result, "latitude", (double)lat);
    shjson_num_add(result, "longitude", (double)lon);
    if (alt)
      shjson_num_add(result, "altitude", alt);
  }

  if (!err) {
    memset(&loc, 0, sizeof(loc));
    err = shgeodb_loc(&geo, &loc);
    if (!err) {
      shjson_str_add(result, "name", loc.loc_name);
      shjson_str_add(result, "summary", loc.loc_summary);
      shjson_str_add(result, "locale", loc.loc_locale);
      shjson_str_add(result, "zone", loc.loc_zone);
      shjson_str_add(result, "type", loc.loc_type);
      shjson_num_add(result, "precision", loc.loc_prec);
    }
  }

  return (0);
}

int api_geo_ipaddr(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  return (SHERR_OPNOTSUPP);
}

