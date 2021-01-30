def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += i * f(i)
    end
  }
  s
end

n = 500
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
