import fxconv
import csv
import struct

def convert(input, output, params, target):
  if params["custom-type"] == "building-data":
    convert_building_data(input, output, params, target)
    return 0
  else:
    return 1

def convert_building_data(input, output, params, target):
  # data for building entries
  names = []
  cps = []
  prices = []
  upg_prices = []

  with open(input, "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
      row = {k.strip(): v.strip() for k, v in row.items()} # strip whitespace for all entries

      names.append(row["name"])
      cps.append(float(row["cps"]))
      prices.append(float(row["price"]))
      upg_prices.append(float(row["upg price"]))

  # number of buildings
  n = len(names)

  MAX_NAME_LEN = 6
  names_bytes = fxconv.ObjectData()
  for name in names:
    name = name[:MAX_NAME_LEN].ljust(MAX_NAME_LEN, "\0") + "\0"
    names_bytes += name.encode("ascii")

  # make sure to use big endian
  cps_bytes        = fxconv.ObjectData() + struct.pack(f">{n}d", *cps)
  prices_bytes     = fxconv.ObjectData() + struct.pack(f">{n}d", *prices)
  upg_prices_bytes = fxconv.ObjectData() + struct.pack(f">{n}d", *upg_prices)

  fxconv.elf_multi([
    ("_"+params["name-name"],      names_bytes),
    ("_"+params["name-cps"],       cps_bytes),
    ("_"+params["name-price"],     prices_bytes),
    ("_"+params["name-upg-price"], upg_prices_bytes),
  ], output, **target)
