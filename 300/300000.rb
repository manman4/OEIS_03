def I(ary, n)
  a = [1]
  (0..n - 1).each{|i| a << -(0..i).inject(0){|s, j| s += ary[1 + i - j] * a[j]}}
  a
end


a =
[1,1,1,2,3,6,10,19,34,63,115,213,391,723,1333,
 2463,4547,8403,15522,28686,53006,97963,181042,
 334606,618415,1142994,2112545,3904592,7216810,
 13338856,24654268,45568784,84225393,155675230,
 287737327,531830605,982993368,1816887637,
 3358192905]
p ary = [1] + a.map{|i| -i}

p I(ary, 39)

