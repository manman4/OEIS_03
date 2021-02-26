def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  s = 0
  (0..n).each{|i|
    (0..i / 2).each{|j|
      s += ncr(i - j, j) * ncr(n - i, i - j) ** 2
    }
  }
  s
end

n = 500
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
