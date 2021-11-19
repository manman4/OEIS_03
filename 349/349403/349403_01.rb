def A(n)
  n.to_s.split('').inject(0){|s, i| s + i.to_i}
end

def B(n)
  ary = [0]
  s = 1
  t = 0
  (1..n).each{|i|
    s *= i
    t += s
    ary << A(t)
  }
  ary
end

n = 66
p ary = B(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
