def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  l = f(n)
  m = f(k)
  (1..n / k).inject(0){|s, i| s + (-1) ** (i - 1) * i ** (n - k * i - 1) * l / (m ** i * f(n - k * i))}.to_i
end

n= 1000
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}