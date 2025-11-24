def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A000984(n)
  ncr(2*n, n)
end

# a(n) = 5*a(n-1) + (-1)^n * A000984(n).
def A390137(n)
  ary = [1]
  (1..n).each{|i|
    ary << 5 * ary[-1] + (-1)**i * A000984(i)
  }
  ary
end

n = 1000
ary = A390137(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
  