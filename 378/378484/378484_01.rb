def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} C(4*k,k)*C(4*(n-k),n-k).
def B(n)
  (0..n).inject(0){|s, i| s + ncr(4 * i, i) * ncr(4 * (n - i), n - i)}
end

def A(n)
  ary = [1]
  (1..n).each{|i| ary << (256 * ary[-1] + 18 * B(i) + 8 * ncr(4 * i, i)) / 27}
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
