def A000089(n)
  (0..n - 1).count{|i| (i * i + 1) % n == 0}
end
def A(n)
  s = 0
  (0..n).map{|i| s += A000089(i)}
end

n = 10000
ary = A(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
