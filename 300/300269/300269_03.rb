def A(n)
  ary = [1] + Array.new(n - 1, 0)
  (1..n).each{|i|
    i3 = 2 * i * i * i
    a = ary.clone
    (0..n - 1).each{|j| a[(j + i3) % n] += ary[j]}
    ary = a
  }
  ary[((n * (n + 1)) ** 2 / 4) % n] / 2
end
def A300269(n)
  (1..n).map{|i| A(i)}
end
p A300269(100)

p (1..100).map{|i| ((i * (i + 1)) ** 2 / 4) % i}
