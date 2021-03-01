def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (-1) ** (n % 2) * (0..n / 2).inject(0){|s, i| s + (-k) ** (n - i) * ncr(n - i, i)}
end

def B(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 139
ary = B(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
