def A325912(n)
  a = [2]
  (1..n).each{|i| a << -a[-1] + 2 ** 2 ** i}
  a
end
def A325910(n)
  a = A325912(n - 1)
  [0] + (1..n).map{|i| (a[i - 1] - (i % 2)).to_s(2).to_i}
end
def A309597(n)
  a = A325910(n - 1)
  ary = [6]
  s = 0
  (2..n).each{|i|
    s += a[i - 1] * 10 ** 2 ** (i - 2)
    ary << s + (5 * 10 ** (2 ** i - 1) + 4) / 9 + a[i - 1] * 10 ** 2 ** (i - 1)
  }
  ary
end
n = 23
a = A309597(n)

def A325907(n)
  a = [3]
  (2..n).each{|i|
    j = 10 ** (2 ** (i - 2))
    a << (j + 3) * (j - 1) / 3 - a[-1]
  }
  a
end
def A309597_2(n)
  A325907(n).map{|i| i * (i + 1) / 2}
end
b = A309597_2(n)

p a == b
