import fxconv
import csv
import struct

def convert(input, output, params, target):
  if params["custom-type"] == "csv":
    convert_csv(input, output, params, target)
    return 0
  if params["custom-type"] == "strs":
    convert_strs(input, output, params, target)
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

# convert a csv file into several symbols
# (each column is its own symbol)
# column names are in params["col-name"]
# column types are in params["col-type"]
def convert_csv(input, output, params, target):
  # raw data in each column
  cols_raw: dict[str, list[str]] = {}

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
  cols_data: dict[str: fxconv.ObjectData] = {}
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
        if max_len <= 0:
          raise ValueError
      except ValueError:
        raise fxconv.FxconvError(f"expected positive integer in csv col-type char[], got {col_type[5:-1]}")
      
      for string in cols_raw[col]:
        cols_data[col] += str_to_bytes(string, max_len)
    
    else:
      raise fxconv.FxconvError(f"unknown csv col-type {col_type}")

  cols_data = [ ("_" + params["col-name"][col], bdata) for col, bdata in cols_data.items() ]

  fxconv.elf_multi(cols_data, output, **target)

# convert a file with one string on each line
# params["length"] tells us the length of the strings
# this means the converter should output an array of strings of length params["length"]+1
def convert_strs(input, output, params, target):
  lines: list[str] = []
  with open(input, "r") as f:
    lines = f.readlines()
  
  str_len = params["length"]
  try:
    str_len = int(str_len)
    if str_len <= 0:
      raise ValueError
  except ValueError:
    raise fxconv.FxconvError(f"expected positive integer for length, got {params["length"]}")

  data = fxconv.ObjectData()
  for l in lines:
    data += str_to_bytes(l[:-1] if l[-1]=="\n" else l, str_len+1)

  fxconv.elf(data, output, "_"+params["name"], **target)

# convert a string into a sequence of bytes of length l (includes null terminator)
def str_to_bytes(s: str, l: int) -> bytes:
  s = s[:l-1].ljust(l-1, "\0") + "\0"
  return s.encode("ascii")
