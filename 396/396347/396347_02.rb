# a(0) = 0; for n > 0, a(n) = a(n-1) - n if nonnegative and not already in the sequence, otherwise a(n) = a(n-1) + n - 1.
def A(n)
  a = [0]
  (1..n).each{|i|
    if a.include?(a[-1] - i) || a[-1] - i < 0
      a << a[-1] + i - 1
    else
      a << a[-1] - i
    end
  }
  a
end

n = 100000
ary = A(n)
(0..10000).each{|i|
  print i
  print ' '  
  # ary[k] = iとなる最小のkを出力
  puts ary.index(i)
}