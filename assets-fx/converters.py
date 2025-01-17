import fxconv
import csv
import struct

def convert(input, output, params, target):
  if params["custom-type"] == "csv":
    convert_csv(input, output, params, target)
    return 0
  else:
    return 1

def convert_csv(input, output, params, target):
  # raw data in each column
  cols_raw = {}

  for col in params["col-name"].keys():
    cols_raw[col] = []

  # number of rows
  n = 0

  with open(input, "r") as f:
    reader = csv.DictReader(f, skipinitialspace=True)
    for row in reader:
      n += 1
      for col, val in row.items():
        if col in cols_raw:
          cols_raw[col].append(val.strip())

  # convert to binary data
  # note: make sure to use big endian
  cols_data = {}
  for col in params["col-name"].keys():
    cols_data[col] = fxconv.ObjectData()
    col_type = params["col-type"][col]

    if col_type == "int":
      cols_data[col] += struct.pack(f">{n}i", *map(int, cols_raw[col]))

    elif col_type == "short":
      cols_data[col] += struct.pack(f">{n}h", *map(int, cols_raw[col]))
    
    elif col_type == "double":
      cols_data[col] += struct.pack(f">{n}d", *map(float, cols_raw[col]))
    
    elif col_type[:5] == "char[":
      max_len = int(col_type[5:-1])
      for string in cols_raw[col]:
        string = string[:max_len-1].ljust(max_len-1, "\0") + "\0"
        cols_data[col] += string.encode("ascii")

  cols_data = [ ("_" + params["col-name"][col], bdata) for col, bdata in cols_data.items() ]

  fxconv.elf_multi(cols_data, output, **target)
