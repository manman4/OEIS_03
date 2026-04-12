# a(n) = b_n(n) + (1/n) * Sum_{k=1..n-1} k * c_n(k) * e_n(n-k),
# where b_n(k) = n^(k-1) for 1 <= k <= 3, and b_n(k) = 0 for k > 3,
# c_n(k) = b_n(k) - a(k) for 1 <= k <= n-1,
# and e_n(0) = 1, e_n(k) = (n/k) * Sum_{j=1..k} j * c_n(j) * e_n(k-j) for 1 <= k <= n-1.

def A394982(n)
  a = [0]
  (1..n).each{|i|
    b = [0] + (1..i).map{|k| k > 3 ? 0 : i ** (k - 1)}
    c = [0] + (1..i - 1).map{|k| b[k] - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i / k.to_r * (1..k).inject(0){|s, j| s + j * c[j] * e[k - j]}}
    a << b[i] + (1..i - 1).inject(0){|s, k| s + k * c[k] * e[i - k]}.to_i / i
  }
  a[1..-1]
end

p A394982(21)