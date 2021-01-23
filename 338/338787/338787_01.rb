def d(n)
  sum = 0
  (1..n).each{|i|
    sum += i if i % 2 == 1 and n % i == 0
  }
  sum
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  return 0 if n == 1
  (f(n - 1) * (1..n - 1).inject(0){|s, i| s + d(i) * d(n - i) / i.to_r}).to_i
end

n = 1000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 