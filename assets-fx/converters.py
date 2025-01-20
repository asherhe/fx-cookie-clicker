import fxconv
import csv
import struct

def convert(input, output, params, target):
  if params["custom-type"] == "csv":
    convert_csv(input, output, params, target)
    return 0
  else:
    return 1

# format specifiers used by struct.pack for each data type
STRUCT_FORMATS: dict[str, tuple[int, any]] = {
  "int8": ("b", int),
  "uint8": ("B", int),
  "int16": ("h", int),
  "uint16": ("H", int),
  "int32": ("i", int),
  "uint32": ("I", int),
  "int64": ("q", int),
  "uint64": ("Q", int),
  "float": ("f", float),
  "double": ("d", float),
}

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
          cols_raw[col].append(val)

  # convert to binary data
  # note: make sure to use big endian
  cols_data = {}
  for col in params["col-name"].keys():
    cols_data[col] = fxconv.ObjectData()
    col_type = params["col-type"][col]

    if col_type in STRUCT_FORMATS:
      fmt, conv = STRUCT_FORMATS[col_type]
      cols_data[col] += struct.pack(f">{n}{fmt}", *map(conv, cols_raw[col]))
    
    elif col_type[:5] == "char[":
      max_len = col_type[5:-1]
      try:
        max_len = int(max_len)
      except ValueError:
        raise fxconv.FxconvError(f"expected positive integer in csv col-type char[], got {col_type[5:-1]}")
      
      for string in cols_raw[col]:
        string = string[:max_len-1].ljust(max_len-1, "\0") + "\0"
        cols_data[col] += string.encode("ascii")
    
    else:
      raise fxconv.FxconvError(f"unknown csv col-type {col_type}")

  cols_data = [ ("_" + params["col-name"][col], bdata) for col, bdata in cols_data.items() ]

  fxconv.elf_multi(cols_data, output, **target)
