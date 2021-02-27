def A(n)
  ary = [1] + Array.new(n - 1, 0)
  (1..n).each{|i|
    i2 = 2 * i * i
    a = ary.clone
    (0..n - 1).each{|j| a[(j + i2) % n] += ary[j]}
    ary = a
  }
  ary[(n * (n + 1) * (2 * n + 1) / 6) % n] / 2
end
def A300268(n)
  (1..n).map{|i| A(i)}
end

n = 3500
ary = A300268(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
