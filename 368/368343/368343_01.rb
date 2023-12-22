# Square array T(n,k), n >= 3, k >= 0, read by antidiagonals downwards, where T(n,k) = Sum_{j=0..n} k^(n-j) * floor(j/3).
def T(n, k)
  (0..n).inject(0){|s, i| s + k ** (n - i) * (i / 3)}
end

def A368343(n)
  (3..n).map{|i| (3..i).map{|j| T(j, i - j)}}.flatten
end

n = 142
ary = A368343(n)
(3..ary.size + 2).each{|i|
  j = ary[i - 3]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
