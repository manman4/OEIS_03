a = [
  [ 1,     3,        6,         10,           15,              21,                28,                   36,                     45,                       55],
  [ 3,    13,       40,        108,          275,             681,              1664,                 4040,                   9779,                    23637,            57096, 137876],
  [ 6,    40,      213,       1049,         5034,           23984,            114069,               542295,                2577870,                 12253948],
  [10,   108,     1049,       9349,        80626,          692194,           5948291,             51139577,              439673502,               3779989098,      32497334055, 279386435639],
  [15,   275,     5034,      80626,      1222363,        18438929,         279285399,           4237530095,            64300829449,             975566486675,   14800469958185],
  [21,   681,    23984,     692194,     18438929,       487150371,       12947640143,         345142437669,          9203308475041,          245355064111139, 6540331954247241],
  [28,  1664,   114069,    5948291,    279285399,     12947640143,      603841648931,       28251882697663,       1322310119854705,        61875355046353061],
  [36,  4040,   542295,   51139577,   4237530095,    345142437669,    28251882697663,     2318527339461265,     190273063549680295],
  [45,  9779,  2577870,  439673502,  64300829449,   9203308475041] + [27359264067916806101] * 10,
  [55, 23637, 12253948, 3779989098, 975566486675, 245355064111139, 61875355046353061, 15609156135669687673, 3931128009418993765997, 988808811046283595068099]
]
p (0..9).map{|i| a[i][8]}
