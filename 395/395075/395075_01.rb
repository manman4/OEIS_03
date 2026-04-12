# a(n) = n^(3*n-4) + (1/n) * Sum_{k=1..n-1} k * c_n(k) * e_n(n-k),
# where c_n(k) = n^(3*k-3)/k - a(k) for 1 <= k <= n-1,
# and e_n(0) = 1, e_n(k) = (n/k) * Sum_{j=1..k} j * c_n(j) * e_n(k-j) for 1 <= k <= n-1.

def A395075(n)
  a = [0]
  (1..n).each{|i|
    c = [0] + (1..i - 1).map{|k| i ** (3 * k - 3) / k.to_r - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i / k.to_r * (1..k).inject(0){|s, j| s + j * c[j] * e[k - j]}}
    a << i ** (3 * i - 4) + (1..i - 1).inject(0){|s, k| s + k * c[k] * e[i - k]}.to_i / i
  }
  a[1..-1]
end

p A395075(20)

