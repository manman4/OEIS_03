  
def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  return 1 if n == 0
  (n * (0..n).inject(0){|s, i| s + (2 * n) ** i * ncr(n + i, 2 * i) / (n + i.to_r)}).to_i
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}