def A325907(n)
  a = [3]
  (2..n).each{|i|
    j = 10 ** (2 ** (i - 2))
    a << (j + 3) * (j - 1) / 3 - a[-1]
  }
  a
end
def A309597(n)
  A325907(n).map{|i| i * (i + 1) / 2}
end

n = 15
ary = A309597(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
