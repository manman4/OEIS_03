n = 10000
ary = [1, 2, 3, 2, 1]
x = 0
(1..n).each{|i|
  s = ary[x]
  a = [s]
  while s < i
    x += 1
    x %= 5
    s += ary[x]
    a << ary[x]
  end
  j = a.join
  print i
  print ' '
  puts j.size
  x += 1
  x %= 5
}
