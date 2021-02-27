# mŸˆÈ‰º‚ğæ‚èo‚·
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

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  ary1 = [1] + Array.new(n, 0)
  i, j = 1, 1
  while j <= n
    ary1[j] = 1
    i += 1
    j = f(i)
  end
  ary = [1]
  (1..n).each{|i|
    b_ary = Array.new(n + 1, 0)
    (0..n / i).each{|j| b_ary[i * j] = ary1[j]}
    ary = mul(ary, b_ary, n)
  }
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
