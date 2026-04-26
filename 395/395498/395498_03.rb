$memo = {}

def t(n, k)
  key = [n, k]
  return $memo[key] if $memo.key?(key)

  val = if n == 0
    k == 0 ? 1 : 0
  else
    term1 = (k + 1) * t(n - 1, k)
    term2 = (n - k) * t(n - 1, k - 1)
    term3 = ((-1) ** n) * (k == n - 1 ? 1 : 0)
    term1 + term2 + term3
  end

  $memo[key] = val
end

# for(n=0, 12, for(k=0, n, print1(T(n,k),", ")));
# (0..12).each{|n|
#   (0..n).each{|k|
#     print "#{t(n, k)}, "
#   }
# }
# puts

# vector(13, n, T(n-1,0..3))
# (0..5).each{|k|
#   arr = (1..33).map{|n| t(n - 1, k)}
#   p arr
# }
def a395498(n)
  # a(n) = A219836(n,n-3).
  (0..n).map{|k| t(k, k - 3)}
end

n = 1000
ary = a395498(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.length > 1000
  print i
  print " "
  puts j
}