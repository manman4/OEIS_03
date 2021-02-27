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

def A300268(n)
  s_ary = []
  ary = [1]
  (1.. n).each{|i|
    b_ary = Array.new(2 * i * i * i + 1, 0)
    b_ary[0], b_ary[-1] = 1, 1
    m = (i * (i + 1) / 2) ** 2
    ary = mul(ary, b_ary, m * 2)
    s = 0
    (m % i).step(ary.size - 1, i){|j|
      s += ary[j]
    }
    s_ary << s / 2
p s / 2
  }
  s_ary
end

n = 20
p ary = A300268(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
