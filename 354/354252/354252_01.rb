def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, m, n)
  ary = [1]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s + (k - m.to_r * j / i) * ncr(i, j) * ary[-j]}.to_i
  }
  ary
end

n = 500
ary = A(6, 3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

