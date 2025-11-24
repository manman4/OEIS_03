def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A059304(n)
  2**n * ncr(2*n, n)
end

# a(n) = 3*a(n-1) + A059304(n).
def A390923(n)
  ary = [1]
  (1..n).each{|i|
    ary << 3*ary[-1] + A059304(i)
  }
  ary
end

n = 1000
ary = A390923(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
  