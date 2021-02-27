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

def A(n)
  ary1 = Array.new(n + 1, 0)
  # n‚ª10000‚Ü‚ÅOK
  (0..10).each{|i| ary1[i * i * i] = 1}
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
