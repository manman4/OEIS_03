def d(n)
  cnt = 0
  (1..n).each{|i|
    cnt += 1 if n % i == 0
  }
  cnt
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 1000
(1..n).each{|i|
  j = f(i - 1) * d(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}