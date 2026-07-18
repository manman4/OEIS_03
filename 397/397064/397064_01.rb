# a(n) = 2^(n-1) - Sum_{k=1..floor(n/3)} 2^(n-3*k) * a(k).
def A397064(n)
  ary = [0]
  (1..n).each{|i| 
  ary << (1..i / 3).inject(2 ** (i - 1)){|s, k| s - 2 ** (i - 3 * k) * ary[k]}}
  ary[1..-1]
end
p A397064(40)
# (1..n).each{|i|
#   j = ary[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
