def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def least_k(n)
  k = 1
  while ncr(k * k, k) % (n * n) != 0
    k += 1
  end
  k
end

n = 1000
(1..n).each{|i|
  print i
  print ' '
  puts least_k(i)
}