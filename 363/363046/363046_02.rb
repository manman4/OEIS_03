def A(n, k)
  ary = [[1] * (n + 1)]
  # S(n,k) = S(n,k-1) + S(n-k,k) for k<=n, S(n,k) = S(n,k-1) for k>n を計算する
  (1..n).each{|i|
    a = [0]
    (1..i).each{|j| a << a[-1] + ary[i - j][j]}
    (i + 1..n).each{|j| a << a[-1]}  # k > n の場合
    ary << a
  }
  # a(n) = sum(j=0, floor(n/k), T(n-k*j,k*j)) を計算する
  (0..n).map{|i|
    (0..i / k).inject(0){|s, j| s + ary[i - k * j][k * j]}
  }
end

n = 1100
m = 1000
ary = A(n, 4)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
