def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A002426(n)
  a = [1, 1]
  (2..n).each{|i| a << ((2 * i - 1) *a[-1] + 3 * (i - 1) * a[-2]) / i}
  a[0..n]
end

def A(k, n)
  (0..k).inject(0){|s, i| s + (-1) ** (k - i) * ncr(k, i) * @a[i] ** n}
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

n = 93
@a = A002426(n)
ary = A327751(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
