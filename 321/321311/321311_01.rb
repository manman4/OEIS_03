def A2(n)
  ary = [0, 15, 36, 99, 292]
  (5..n).each{|i| ary << 3 * ary[-1]}
  ary[0..n]
end

def A1(n)
  ary = [0, 3, 5]
  (3..n).each{|i| ary << 2 * ary[-1]}
  ary[0..n]
end

def A(n)
  p a = A2(n)
  p b = A1(n + 1)
  (0..n).map{|i| a[i] - b[i + 1]}
end

n = 28
p ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
