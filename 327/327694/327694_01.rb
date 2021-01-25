# m次以下を取り出す
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary[0..m]
end

def s(k, m, n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0 && i % k == m}
  s
end

def A(n)
  ary = [1]
  a = [0] + (1..n).map{|i| -s(5, 2, i) - s(5, 3, i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end

def B(n)
  ary = A(n)
  a = ary
  (2..n).each{|i|
    b = Array.new(n + 1, 0)
    (0..n / i).each{|j| b[i * j] += ary[j]}
    a = mul(a, b, n)
  }
  a
end

n = 100
p ary = B(n)
(0..100).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
