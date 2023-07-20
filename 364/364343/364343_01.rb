# a(n) = (n/2) * Sum_{d|n} (-1)^(d+1) * (d+1)
def A(n)
  s = 0
  (1..n).each{|i|
    s += (-1) ** ((i + 1) % 2) * (i + 1) if n % i == 0
  }
  n * s / 2
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}