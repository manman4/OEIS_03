def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1]
  (1..n).each{|i| 
    ary << (0..(i - 1) / k).inject(0){|s, j| s + ncr(i - 1, k * j) * ary[j]}
  }
  ary
end

n = 1000
ary = A(3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}