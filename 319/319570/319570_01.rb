def f(k, n)
  return false if n % 10 == 0
  m = n.to_s.reverse.to_i
  mm = Math.sqrt(m).round
  return (mm ** 2 == m) && (mm != k.to_s.reverse.to_i)
end

def pl(n)
  n.to_s.reverse.to_i == n
end

ary = []
(1..10 ** 6).each{|i|
  j = i * i
  if !pl(j) && f(i, j)
    ary << i
  end
}
p ary
p ary.map{|i| i * i}
(1..1).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}

ary =
[33,99,3168,6501,20508,21468,110922,219111,303577,
 304877,1100922,1109211,1110922,1119111,2191011,
 2191111,3080367,3140793,10110922,11009111,
 11009122,11009221,11019111,11091011,11091022,
 11091111,11091121,11091122,11091202,11091211]
p ary.map{|i| i * i}
