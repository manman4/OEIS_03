def f(r)
  return 1 if r == 0
  cnt = 0
  (1..Math.sqrt(r)).each{|i|
    j = r - i * i
    cnt += 1 if j == (Math.sqrt(j).to_i ** 2)
  }
  4 * cnt
end

def A(n)
  return 0 if n == 0
  m = f(n)
  return n ** (m / 2) if m > 0
  0
end

(0..10000).each{|i| 
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
