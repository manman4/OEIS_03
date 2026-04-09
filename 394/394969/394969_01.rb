# a(n) = n^(2*(n-1)) + (1/n) * Sum_{k=1..n-1} k * c_n(k) * e_n(n-k),
# where c_n(k) = n^(2*(k-1)) - a(k) for 1 <= k <= n-1,
# and e_n(0) = 1, e_n(k) = (n/k) * Sum_{j=1..k} j * c_n(j) * e_n(k-j) for 1 <= k <= n-1.

def A394969(n)
  a = [0]
  (1..n).each{|i|
    c = [0] + (1..i - 1).map{|k| i ** (2 * (k - 1)) - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i.to_r / k * (1..k).inject(0){|s, j| s + j * c[j] * e[k - j]}}
    a << i ** (2 * (i - 1)) + (1..i - 1).inject(0){|s, k| s + k * c[k] * e[i - k]}.to_i / i
  }
  a[1..-1]
end
p A394969(20)

# p (1..n).map{|i| ary[i - 1] / i.to_r}
# (1..n).each{|i|
#   j = ary[i - 1].denominator
#   if j > 1
#     puts i
#   end
# }
