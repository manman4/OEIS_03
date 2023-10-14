def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  s = 0
  (1..n).each{|i|
    s += phi(n / i) * ncr(i + k - 1, k) if n % i == 0
  }
  s
end

def A343516(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << A(i - j + 1, j)
    }
  }
  ary
end

n = 140
ary = A343516(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
  