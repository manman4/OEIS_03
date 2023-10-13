def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  m = f(n)
  s = 0
  (1..n).each{|i|
    s += m / (i.to_r * (n / i) ** i) if n % i == 0
  }
  s
end

n = 1000
(1..n).each{|i|
  j = A(i).to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

