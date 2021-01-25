def A325912(n)
  a = [2]
  (1..n).each{|i| a << -a[-1] + 2 ** 2 ** i}
  a
end
def A325910(n)
  a = A325912(n - 1)
  [0] + (1..n).map{|i| (a[i - 1] - (i % 2)).to_s(2).to_i}
end
p A325910(10)
