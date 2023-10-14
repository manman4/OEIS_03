def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def A(k, n)
  s = 0
  m = f((k - 1) * n)
  (0..n).to_a.repeated_permutation(k){|i|
    if (0..k - 1).inject(0){|p, j| p + i[j]} == n
      s += m / (0..k - 1).inject(1){|p, j| p * f(n - i[j])}
    end
  }
  s
end

n = 15
(0..n).each{|i|
  j = A(4, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
