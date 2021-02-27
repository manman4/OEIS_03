require 'prime'

def f(n)
  p_ary = [0] + Prime.each(10 ** 8).to_a
  s = 0
  s_ary = (0..p_ary.size - 1).map{|i| s += p_ary[i]}
  ary = []
  (1..n).each{|i|
    m = s_ary[i]
    j = i + 1
    while s_ary[j] % m > 0
      j += 1
    end
    break if j == p_ary.size - 1
    ary << j
  }
  ary
end

n = 500
ary = f(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
