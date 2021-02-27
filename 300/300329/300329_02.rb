def A(n)
  ary = [1] + Array.new(n - 1, 0)
  (1..n).each{|i|
    i1 = 2 * i
    a = ary.clone
    (0..n - 1).each{|j| a[(j + i1) % n] += ary[j]}
    ary = a
  }
  ary[(n * (n + 1) / 2 + n - 1) % n]
end

n = 5000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
