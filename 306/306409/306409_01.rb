def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  s = 0
  (0..n).each{|i| 
    (i + 1..n).each{|j|
      s += (-1) ** ((i + j) % 2) * ncr(i + j, i)
    }
  }
  -s
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
