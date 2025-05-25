# Pell number
def p(n)
  p_ary = [0, 1]
  (2..n).each{|i|
    p_ary << 2 * p_ary[i - 1] + p_ary[i - 2]
  }
  p_ary
end

def A(n)
  p_ary = p(n + 2)
  (0..n).map{|i| p_ary[i + 1] * p_ary[i + 2] - (-1) ** i}
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
