def d(n)
  s = 0
  (1..n).each{|i| s += 1 if n % i == 0}
  s
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  return 1 if n == 0
  s = 0
  n.downto(1){|i|
    if n % i == 0
      j, k = d(i), n / i
      if j >= k
        s += ncr(j, k)
      end
    end
  }
  s
end

n = 10000
(0..n).each{|i|
  print i
  print ' '
  puts A(i)
}
