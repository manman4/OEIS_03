def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << (2..i).inject(1){|s, j| s + (-1) ** (j % 2) * ary[i / j]}
  }
  ary
end

n = 2 ** 13 - 1
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
