# number of divisors of n
def d(n)
  (1..n).count{|i| n % i == 0}
end

def A(n)
  d(3 * n - 1) / 2
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}