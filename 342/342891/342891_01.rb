def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def S(n, k)
  (0..11).inject(1){|s, i| s * ncr(n + i, k) / ncr(k + i, k).to_r}.to_i
end

def A(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << S(i, j)
    }
  }
  ary
end

n = 139
ary = A(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
