# a(n) = 2^(n-1) - Sum_{k=1..floor(n/4)} 2^(n-4*k) * a(k).
def A397213(n)
  a = [0]
  (1..n).each{|i|
  a << (1..i / 4).inject(2 ** (i - 1)){|s, k| s - 2 ** (i - 4 * k) * a[k]}}
  a[1..-1]
end

p A397213(40)
# (1..n).each{|i|
#   j = ary[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
