# a(n)   =   n^(n-1) + (1/n) * Sum_{k=1..n-1} k * b_n(k) * e_n(n-k)
# b_n(k) =   n^(k-1) - a(k)
# e_n(0) = 1; e_n(k) = (n/k) * Sum_{j=1..k}   j * b_n(j) * e_n(k-j)
def A394969(n)
  a = [0]
  (1..n).each{|i|
    b = (0..i - 1).map{|k| i ** (2 * (k - 1)) - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i.to_r / k * (1..k).inject(0){|s, j| s + j * b[j] * e[k - j]}}
    a << i ** (2 * (i - 1)) + (1..i - 1).inject(0){|s, k| s + k * b[k] * e[i - k]}.to_i / i
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
