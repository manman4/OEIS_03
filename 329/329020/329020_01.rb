def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  if k == 0
    return 1 if n == 0
    return 0
  end
  (0..(2 * k - 1) * n / (2 * k)).inject(0){|s, i| s + (-1) ** i * ncr(2 * n, i) * ncr((2 * k + 1) * n - 2 * k * i - 1, (2 * k - 1) * n - 2 * k * i)}
end

def A327751(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 50
ary = A327751(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
