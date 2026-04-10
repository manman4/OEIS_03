# a(n) = n^(2*n-1) + (1/n) * Sum_{k=1..n-1} k * c_n(k) * e_n(n-k),
# where c_n(k) = n^(2*k)/k - a(k) for 1 <= k <= n-1,
# and e_n(0) = 1, e_n(k) = (n/k) * Sum_{j=1..k} j * c_n(j) * e_n(k-j) for 1 <= k <= n-1.

def A395024(n)
  a = [0]
  (1..n).each{|i|
    c = [0] + (1..i - 1).map{|k| i ** (2 * k) / k.to_r - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i.to_r / k * (1..k).inject(0){|s, j| s + j * c[j] * e[k - j]}}
    a << i ** (2 * i - 1) + (1..i - 1).inject(0){|s, k| s + k * c[k] * e[i - k]}.to_i / i
  }
  a[1..-1]
end
p A395024(20)

# n = 20
# p ary = A395024(n)
# (1..n).each{|i|
#   j = ary[i - 1].denominator
#   if j > 1
#     puts i
#   end
# }
