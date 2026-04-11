# a(n) = n^(2*n-1) + (1/n) * Sum_{k=1..n-1} k * c_n(k) * e_n(n-k),
# where c_n(k) = n^(2*k)/k - a(k) for 1 <= k <= n-1,
# and e_n(0) = 1, e_n(k) = (n/k) * Sum_{j=1..k} j * c_n(j) * e_n(k-j) for 1 <= k <= n-1.

def A395075(n)
  a = [0]
  (1..n).each{|i|
    c = [0] + (1..i - 1).map{|k| i ** (3 * k - 3) / k.to_r - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i.to_r / k * (1..k).inject(0){|s, j| s + j * c[j] * e[k - j]}}
    a << i ** (3 * i - 4) + (1..i - 1).inject(0){|s, k| s + k * c[k] * e[i - k]} / i
  }
  a[1..-1]
end
n = 200
ary = A395075(n)
(1..n).each{|i|
  j = ary[i - 1]
  if j.denominator > 1
    puts "a(#{i}) is not an integer."
    break
  end
  j = j.to_i.to_s
  break if j.size > 1000
  print i
  print ' '
  puts j
}
