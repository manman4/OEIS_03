def A(n)
  ary = [1] + Array.new(n - 1, 0)
  (1..n).each{|i|
    it = i * (i + 1)
    a = ary.clone
    (0..n - 1).each{|j| a[(j + it) % n] += ary[j]}
    ary = a
  }
  (0..n - 1).map{|i| ary[(n * (n + 1) * (n + 2) / 6 + i) % n]}
end

n = 140
ary = (1..n).map{|i| A(i)}.flatten
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
