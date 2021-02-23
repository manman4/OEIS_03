def A(n)
  ary = [1]
  a, k = 1, 2
  while ary.size < n
    a = a / a.gcd(k) + k + 1
    ary << a
    k += 1
  end
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
