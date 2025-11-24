def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A098658(n)
  3**n * ncr(2*n, n)
end

# a(n) = 2*a(n-1) + A098658(n).
def A390924(n)
  ary = [1]
  (1..n).each{|i|
    ary << 2*ary[-1] + A098658(i)
  }
  ary
end

n = 1000
ary = A390924(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
  