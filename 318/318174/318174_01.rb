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
  s = 1
  a, b = 1, 5
  ary = [1]
  (1..n).each{|i|
    s *= a * b * 12
    s /= (i * i)
    a += 12
    b += 12
    ary << s
  }
  ary
end

def B(n)
  s = 1
  a, b = 13, 5
  ary = [1]
  (1..n).each{|i|
    s *= a * b * 12
    s /= (i * i)
    a += 12
    b += 12
    ary << s
  }
  a = []
  (0..n).each{|i|
    break if ary[i] % (i + 1) > 0
    a << ary[i] / (i + 1)
  }
  a
end

n = 500
=begin
a = A(n)
p b == (0..n).map{|i| a[i] * (12 * i + 1) / (i + 1)}
p (0..n).map{|i| a[i] % (i + 1)}
p mul(a, b, n)
=end
ary = B(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
