def A394982(n)
  a = [0]
  (1..n).each{|i|
    b = [0] + (1..i).map{|k| k > 3 ? 0 : i ** (k - 1)}
    c = [0] + (1..i - 1).map{|k| b[k] - a[k]}
    e = [1]
    (1..i - 1).each{|k| e << i.to_r / k * (1..k).inject(0){|s, j| s + j * c[j] * e[k - j]}}
    a << b[i] + (1..i - 1).inject(0){|s, k| s + k * c[k] * e[i - k]} / i
  }
  a
end

n = 500
ary = A394982(n)
(1..n).each{|i|
  j = ary[i]
  break if j.denominator > 1
  j = j.to_i.to_s
  break if j.size > 1000
  print i
  print ' '
  puts j
}
