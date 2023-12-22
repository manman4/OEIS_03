# Square array T(n,k), n >= 2, k >= 0, read by antidiagonals downwards, where T(n,k) = Sum_{j=0..n} k^(n-j) * floor(j/2).
def T(n, k)
  (0..n).inject(0){|s, i| s + k ** (n - i) * (i / 2)}
end

def A368296(n)
  (2..n).map{|i| (2..i).map{|j| T(j, i - j)}}.flatten
end

n = 141
ary = A368296(n)
(2..ary.size + 1).each{|i|
  j = ary[i - 2]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
